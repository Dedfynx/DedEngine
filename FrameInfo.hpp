#pragma once

#include "Camera.hpp"
#include "GameObject.hpp"

#include <vulkan/vulkan.h>



namespace DedOs {
#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct UBO {
		glm::mat4 projectionMatrix{ 1.f };
		glm::mat4 viewMatrix{ 1.f };
		//glm::mat4 inverseViewMatrix{ 1.f };
		glm::vec4 ambientLightColor{ 1.f,1.f,1.f,.02f };//(R,G,B,Intensity)
		PointLight pointLights[MAX_LIGHTS];
		int nbLights;
	};

	struct FrameInfo {
		int frameIndex;
		float deltaTime;
		VkCommandBuffer commandBuffer;
		Camera &hCamera;
		VkDescriptorSet globalDescriptors;
		GameObject::Map& gameObjects;
	};
}