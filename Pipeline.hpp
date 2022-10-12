#pragma once
#include <string>
#include <vector>

#include "Device.hpp"

namespace DedOs{
	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;

		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline {

	public:
		Pipeline(Device &device, const std::string vertFilePath, const std::string fragFilePath, const PipelineConfigInfo &pci);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		static void defaultPCI(PipelineConfigInfo& configInfo);
		static void enableAlphaBlending(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string filePath);
		Device& hDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertSM;
		VkShaderModule fragSM;
		void createGraphicsPipeline(const std::string vertFilePath, const std::string fragFilePath, const PipelineConfigInfo& pci);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* sm);
	};
}