#pragma once
#include <string>
#include "Asset.hpp"
#include "vulkan/vulkan.h"


namespace StrikeEngine
{
	struct ImageParams
	{
		VkImage Handle;
		VkImageView View;
		VkSampler Sampler;
		VkDeviceMemory Memory;

		ImageParams() :
			Handle(VK_NULL_HANDLE),
			View(VK_NULL_HANDLE),
			Sampler(VK_NULL_HANDLE),
			Memory(VK_NULL_HANDLE)
		{}
	};

	struct DescriptorSetParameters
	{
		VkDescriptorPool Pool;
		VkDescriptorSetLayout Layout;
		VkDescriptorSet Handle;

		DescriptorSetParameters() :
			Pool(VK_NULL_HANDLE),
			Layout(VK_NULL_HANDLE),
			Handle(VK_NULL_HANDLE)
		{}
	};

	class StrikeRenderer;
	class Texture : public Asset
	{
	public:

		bool Create(StrikeRenderer* renderer);
		bool CreateDepthTexture(StrikeRenderer* renderer);
		ImageParams& GetImgParams();
		DescriptorSetParameters& GetDescParams();

	private:
		bool CreateImage(StrikeRenderer* renderer, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image);
		bool AllocateImageMemory(StrikeRenderer* renderer, VkImage image, VkMemoryPropertyFlagBits property, VkDeviceMemory* memory);
		bool CreateImageView(StrikeRenderer* renderer, ImageParams& imgParams, VkFormat format, VkImageAspectFlags aspectFlags);
		bool CreateSampler(StrikeRenderer* renderer, VkSampler* sampler);
		bool TransitionImageLayout(StrikeRenderer* renderer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		bool CopyTextureData(StrikeRenderer* renderer, char* textureData, uint32_t dataSize, uint32_t width, uint32_t height);
		bool CreateDescriptorSetLayout();
		bool CreateDescriptorPool();
		bool AllocateDescriptorSet();
		bool UpdateDescriptorSet();

		ImageParams m_ImgParams;
		DescriptorSetParameters m_DescParams;
	};

};

