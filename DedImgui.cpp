#include "DedImgui.hpp"

namespace DedOs {
	DedImgui::DedImgui(Device& device, Window& win, VkRenderPass renderPass, uint32_t imageCount) : hDevice{device}
	{
		init(win, renderPass, imageCount);
	}
	DedImgui::~DedImgui()
	{
		vkDestroyDescriptorPool(hDevice.device(), imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void DedImgui::newFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void DedImgui::render(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImDrawData* drawdata = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
	}
	void DedImgui::runExample()
	{
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can
		// browse its code to learn more about Dear ImGui!).
		if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named
		// window.
		{
			static int counter = 0;

			ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!" and append into it.

			ImGui::Text(
				"This is some useful text.");  // Display some text (you can use a format strings too)
			ImGui::Checkbox(
				"Demo Window",
				&show_demo_window);  // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.1f, 1000.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color",
				(float*)&clear_color);  // Edit 3 floats representing a color

			if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true
				// when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text(
				"Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window) {
			ImGui::Begin(
				"Another Window",
				&show_another_window);  // Pass a pointer to our bool variable (the window will have a
			// closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me")) show_another_window = false;
			ImGui::End();
		}
	}

	void DedImgui::init(Window& win, VkRenderPass renderPass, uint32_t imageCount)
	{
		f = 30.f;
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		if (vkCreateDescriptorPool(hDevice.device(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up descriptor pool for imgui");
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
  // Initialize imgui for vulkan
		ImGui_ImplGlfw_InitForVulkan(win.getWindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = hDevice.getInstance();
		init_info.PhysicalDevice = hDevice.getPhysicalDevice();
		init_info.Device = hDevice.device();
		init_info.QueueFamily = hDevice.getGraphicsQueueFamily();
		init_info.Queue = hDevice.graphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = imguiPool;
		//uint32_t                        Subpass;
		init_info.MinImageCount = 2;
		init_info.ImageCount = imageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = VK_NULL_HANDLE; //Todo: VMA
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, renderPass);

		auto commandBuffer = hDevice.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		hDevice.endSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();

	}
}