#include "App.hpp"

namespace DedOs {
	

	App::App()
	{
		globalPool = DescriptorPool::Builder(hDevice)
			.setMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Swapchain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadGameObjects();

	}
	App::~App()
	{
	}
	void App::run()
	{
		DedImgui imgui{
			hDevice,hWin,hRenderer.getSwapchainRenderPass(),hRenderer.getImageCount()
		};

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
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		Texture texture = Texture(hDevice, "textures/image.jpg");
		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = texture.getSampler();
		imageInfo.imageView = texture.getImageView();
		imageInfo.imageLayout = texture.getImageLayout();

		std::vector<VkDescriptorSet> globalDescritorSets(Swapchain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescritorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &imageInfo)
				.build(globalDescritorSets[i]);


		}

		RenderSystem renderSystem{ hDevice, hRenderer.getSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ hDevice, hRenderer.getSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
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
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

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
				fInfo.blinnExp = imgui.f;
				//update
				UBO ubo{};
				ubo.projectionMatrix = camera.getProjectionMatrix();
				ubo.viewMatrix = camera.getViewMatrix();
				ubo.inverseViewMatrix = camera.getInverseViewMatrix();
				pointLightSystem.update(fInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render
				hRenderer.beginSwapchainRenderPass(commandBuffer);
				imgui.newFrame();
				renderSystem.renderGameObjects(fInfo);
				pointLightSystem.render(fInfo);
				imgui.runExample();
				imgui.render(commandBuffer);
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

		/*model = Model::createModelFromFile(hDevice, "models/cessna.obj");
		auto avion = GameObject::createGameObject();
		avion.model = model;
		avion.transform.translation = { .0f ,-1.5f ,.0f };
		avion.transform.rotation = { glm::pi<float>(),.0f,.0f};
		avion.transform.scale = glm::vec3{ .1f };
		gameObjects.emplace(avion.getId(), std::move(avion));*/


		model = Model::createModelFromFile(hDevice, "models/quad.obj");
		auto sol = GameObject::createGameObject();
		sol.model = model;
		sol.transform.translation = { .0f ,.5f ,0.f };
		sol.transform.scale = glm::vec3{ 3.f };
		gameObjects.emplace(sol.getId(), std::move(sol));
		
		std::vector<glm::vec3> lightColors{
		  {1.f, .1f, .1f},
		  {.1f, .1f, 1.f},
		  {.1f, 1.f, .1f},
		  {1.f, 1.f, .1f},
		  {.1f, 1.f, 1.f},
		  {1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = GameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f });
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}



	}

	
}