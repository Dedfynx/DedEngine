#include "PointLightSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>

namespace DedOs {
	struct PointLightPushConstant {
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};

	PointLightSystem::PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
		:hDevice{ device }
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	PointLightSystem::~PointLightSystem()
	{
	}

	void PointLightSystem::update(FrameInfo& fInfo, UBO& ubo) {
		int lightIndex = 0;
		auto rotateLight = glm::rotate(
			glm::mat4(1.f),
			fInfo.deltaTime,
			{ 0.f, -1.f, 0.f });

		for (auto& kv: fInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.pointLight == nullptr) { continue; }
			
			assert(lightIndex < MAX_LIGHTS && "Can't add more Point lights");
			obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.f));
			//update ubo
			ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
			ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);

			lightIndex++;
		}
		ubo.nbLights = lightIndex;
	}
	void PointLightSystem::render(FrameInfo& fInfo)
	{
		hpipeline->bind(fInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			fInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1,
			&fInfo.globalDescriptors,
			0,
			nullptr
		);

		for (auto& kv : fInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.pointLight == nullptr) { continue; }
			PointLightPushConstant push{};
			push.position = glm::vec4(obj.transform.translation, 1.f);
			push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			push.radius = obj.transform.scale.x;

			vkCmdPushConstants(
				fInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstant),
				&push
			);
			vkCmdDraw(fInfo.commandBuffer, 6, 1, 0, 0);
		}
	}
	void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstant);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pNext;
		pipelineLayoutInfo.flags;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(hDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}
	void PointLightSystem::createPipeline(VkRenderPass renderpass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPCI(pipelineConfig);
		
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderpass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		hpipeline = std::make_unique<Pipeline>(
			hDevice,
			"pointLight.vert.spv",
			"pointLight.frag.spv",
			pipelineConfig
			);
	}
}