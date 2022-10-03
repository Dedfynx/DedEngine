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

	class PointLightSystem {
	public:
		PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& fInfo, UBO &ubo);
		void render(FrameInfo& fInfo);



	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderpass);

		Device& hDevice;
		std::unique_ptr<Pipeline> hpipeline;
		VkPipelineLayout pipelineLayout;


	};
}