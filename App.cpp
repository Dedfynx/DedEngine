#include "App.hpp"
#include "Camera.hpp"
#include "KeyboardController.hpp"
#include "Buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>

namespace DedOs {
	struct UBO {
		glm::mat4 projectionMatrix{ 1.f };
		glm::vec4 ambientLightColor{ 1.f,1.f,1.f,.02f };//(R,G,B,Intensity)
		//Soleil
		//glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
		//Point Light
		glm::vec3 lightPosition{-1.f};
		alignas(16)glm::vec4 lightColor{1.f}; //(R,G,B,Intensity)
	};

	App::App()
	{
		globalPool = DescriptorPool::Builder(hDevice)
			.setMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadGameObjects();

	}
	App::~App()
	{
	}
	void App::run()
	{
		std::vector<std::unique_ptr<Buffer>> uboBuffers( Swapchain::MAX_FRAMES_IN_FLIGHT );
		for (size_t i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<Buffer>(
				hDevice,
				sizeof(UBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(hDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescritorSets(Swapchain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescritorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescritorSets[i]);


		}

		RenderSystem renderSystem{ hDevice, hRenderer.getSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		Camera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		auto viewerObject = GameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!hWin.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			auto deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveXZ(hWin.getWindow(), deltaTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = hRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			if (auto commandBuffer = hRenderer.beginFrame()) {
				int frameIndex = hRenderer.getFrameIndex();

				//FrameInfo
				FrameInfo fInfo{
					frameIndex,
					deltaTime,
					commandBuffer,
					camera,
					globalDescritorSets[frameIndex],
					gameObjects
				};
				//update
				UBO ubo{};
				ubo.projectionMatrix = camera.getProjectionMatrix() * camera.getViewMatrix();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
				//render
				hRenderer.beginSwapchainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(fInfo);
				hRenderer.endSwapchainRenderPass(commandBuffer);
				hRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(hDevice.device());
	}

	void App::loadGameObjects()
	{
		std::shared_ptr<Model> model;

		model = Model::createModelFromFile(hDevice,"models/smooth_vase.obj");
		auto smoothVase = GameObject::createGameObject();
		smoothVase.model = model;
		smoothVase.transform.translation = { .5f ,.5f ,0.f };
		smoothVase.transform.scale = glm::vec3{ 3.f };
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		model = Model::createModelFromFile(hDevice, "models/flat_vase.obj");
		auto flatVase = GameObject::createGameObject();
		flatVase.model = model;
		flatVase.transform.translation = { -.5f ,.5f ,0.f };
		flatVase.transform.scale = glm::vec3{ 3.f };
		gameObjects.emplace(flatVase.getId(),std::move(flatVase));

		model = Model::createModelFromFile(hDevice, "models/quad.obj");
		auto sol = GameObject::createGameObject();
		sol.model = model;
		sol.transform.translation = { .0f ,.5f ,0.f };
		sol.transform.scale = glm::vec3{ 3.f };
		gameObjects.emplace(sol.getId(), std::move(sol));


	}

	
}