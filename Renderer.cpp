#include "Renderer.hpp"

#include <stdexcept>
#include <array>

namespace DedOs {

	Renderer::Renderer(Window& win, Device& device) : hWin{ win }, hDevice{ device }
	{

		recreateSwapChain();
		createCommandBuffers();

	}
	Renderer::~Renderer()
	{
		freeCommandBuffers();
	}

	VkCommandBuffer Renderer::beginFrame()
	{
		assert(!isFrameStarted && "Frame already in progress");

		auto result = hSwapchain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire image");
		}
		
		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffers");
		}
		return commandBuffer;
	}

	void Renderer::endFrame()
	{
		assert(isFrameStarted && "Cant end frame if not frame started");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffers");

		}

		auto result = hSwapchain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || hWin.wasWindowResize()) {
			hWin.resetWindowResizeFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::beginSwapchainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cant begin render pass if not frame started");
		assert(commandBuffer== getCurrentCommandBuffer() && "Cant begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = hSwapchain->getRenderPass();
		renderPassInfo.framebuffer = hSwapchain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = hSwapchain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f,0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(hSwapchain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(hSwapchain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, hSwapchain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void Renderer::endSwapchainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cant end render pass if not frame started");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cant end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);

	}

	void Renderer::createCommandBuffers()
	{
		commandBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = hDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(hDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}

	}
	void Renderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			hDevice.device(),
			hDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());

		commandBuffers.clear();
	}

	void Renderer::recreateSwapChain()
	{
		auto extent = hWin.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = hWin.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(hDevice.device());
		//hSwapchain = nullptr;
		if (hSwapchain == nullptr) {
			hSwapchain = std::make_unique<Swapchain>(hDevice, extent);
		}
		else {
			std::shared_ptr<Swapchain> oldSwapchain = std::move(hSwapchain);
			hSwapchain = std::make_unique<Swapchain>(hDevice, extent, oldSwapchain);
			if (!oldSwapchain->compareSwapFormats(*hSwapchain.get())) {
				throw std::runtime_error("Swap Chain image or depth format has changed");
			}
		}

	}


}