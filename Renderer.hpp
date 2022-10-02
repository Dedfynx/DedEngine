#pragma once

#include "Window.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"

//std
#include <memory>
#include <vector>
#include <cassert>

namespace DedOs {
	class Renderer {
	public:
		Renderer(Window& win, Device& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();

		VkRenderPass getSwapchainRenderPass() const { return hSwapchain->getRenderPass(); };
		float getAspectRatio() const {
			return hSwapchain->extentAspectRatio();
		};
		void beginSwapchainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapchainRenderPass(VkCommandBuffer commandBuffer);

		bool isFrameInProgress() const {
			return isFrameStarted;
		}
		
		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get Frame Index when frame not in progress");
			return currentFrameIndex;

		}
	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();


		Window& hWin;
		Device& hDevice;
		std::unique_ptr<Swapchain> hSwapchain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{false};

	};
}