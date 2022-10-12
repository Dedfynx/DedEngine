#pragma once
#include "Device.hpp"

#include <string>

namespace DedOs {
	class Texture {
	public:
		Texture(Device& device, const std::string& filepath);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(const Texture&&) = delete;
		Texture&& operator=(const Texture&&) = delete;

		VkSampler getSampler() { return sampler; }
		VkImageView getImageView() { return imageView; }
		VkImageLayout getImageLayout() { return imageLayout; }

	private:
		void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
		Device& hDevice;

		VkSampler sampler;
		VkImage image;
		VkDeviceMemory imageMemory;
		VkImageView imageView;
		VkFormat imageFormat;
		VkImageLayout imageLayout;
	};
}