#pragma once

#include "Camera.hpp"
#include "GameObject.hpp"

#include <vulkan/vulkan.h>

namespace DedOs {
	struct FrameInfo {
		int frameIndex;
		float deltaTime;
		VkCommandBuffer commandBuffer;
		Camera &hCamera;
		VkDescriptorSet globalDescriptors;
		GameObject::Map& gameObjects;
	};
}