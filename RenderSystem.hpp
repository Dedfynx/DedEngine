#pragma once

#include "Pipeline.hpp"
#include "Device.hpp"
#include "Model.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "FrameInfo.hpp"

//std
#include <memory>
#include <vector>

namespace DedOs {
	class RenderSystem {
	public : 
		RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void renderGameObjects(FrameInfo &fInfo);



	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderpass);

		Device& hDevice;
		std::unique_ptr<Pipeline> hpipeline;
		VkPipelineLayout pipelineLayout;

		
	};
}