#pragma once

//core
#include "Window.hpp"
#include "Device.hpp"

//libs
#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_glfw.h"
#include "libs/imgui/imgui_impl_vulkan.h"

//std
#include <stdexcept>

namespace DedOs {
	static void check_vk_result(VkResult err) {
		if (err == 0) return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0) abort();
	}

	class DedImgui {
	public:
		DedImgui(Device& device, Window& win, VkRenderPass renderPass, uint32_t imageCount);
		~DedImgui();

		DedImgui(const DedImgui&) = delete;
		DedImgui& operator=(const DedImgui&) = delete;

		void newFrame();
		void render(VkCommandBuffer commandBuffer);

		// Example state
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		void runExample();
		float f;

	private:
		void init(Window& win, VkRenderPass renderPass, uint32_t imageCount);

		Device& hDevice;
		VkDescriptorPool imguiPool{};
	};
}