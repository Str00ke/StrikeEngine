#include "Model/Texture.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Model/Model.hpp"
#include <vector>

namespace StrikeEngine
{
	bool Texture::Create(StrikeRenderer* renderer, Model* model)
	{
		/*if (!CreateDescriptorSetLayout()) return false;

		if (!CreateDescriptorPool()) return false;

		if (!AllocateDescriptorSet()) return false;*/

		int width = 0, height = 0, dataSize = 0;
		std::vector<char> textureData;
		if (GetPath() == "") //If no textures are set
		{
			width = 1024;
			height = 1024;
			dataSize = width * height * 4;
			textureData.assign(dataSize, -1); //TODO: don't put hardcoded values like that pls
		}
		else
		{
			textureData = Tools::GetImageData(GetPath().c_str(), 4, &width, &height, nullptr, &dataSize);
			if (textureData.size() == 0)
				return false;
		}



		//VkDeviceSize bufferSize = width * height * 4;
		//renderer->GetVulkanParameters().StagingBuffer.Size = width * height * 4;

		//if (!renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, renderer->GetVulkanParameters().StagingBuffer))
		//{
		//	std::cout << "Could not create vertex buffer" << std::endl;
		//	return false;
		//}

		//void* data;
		//vkMapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory, 0, bufferSize, 0, &data);
		//memcpy(data, textureData.data(), static_cast<size_t>(bufferSize));
		//vkUnmapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory);



		////-----Create image-----
		//if (!CreateImage(renderer, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ImgParams.Handle))
		//{
		//	std::cout << "Could not create image" << std::endl;
		//	return false;
		//}

		//if (!AllocateImageMemory(renderer, m_ImgParams.Handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_ImgParams.Memory))
		//{
		//	std::cout << "Could not allocate memory for image" << std::endl;
		//	return false;
		//}

		//if (vkBindImageMemory(renderer->GetVulkanParameters().Device, m_ImgParams.Handle, m_ImgParams.Memory, 0) != VK_SUCCESS)
		//{
		//	std::cout << "Could not bind memory to an image" << std::endl;
		//	return false;
		//}
		////----------

		//TransitionImageLayout(renderer, m_ImgParams.Handle, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		//renderer->CopyBufferToImage(renderer->GetVulkanParameters().StagingBuffer.Handle, m_ImgParams.Handle, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		//TransitionImageLayout(renderer, m_ImgParams.Handle, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


		//vkDestroyBuffer(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Handle, nullptr);
		//vkFreeMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory, nullptr);


		//if (!CreateImageView(renderer, m_ImgParams, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT))
		//{
		//	std::cout << "Could not create image view" << std::endl;
		//	return false;
		//}

		//if (!CreateSampler(renderer, &m_ImgParams.Sampler))
		//{
		//	std::cout << "Could not create sampler" << std::endl;
		//	return false;
		//}

		///*if (!CopyTextureData(&textureData[0], dataSize, width, height))
		//{
		//	std::cout << "Could not upload texture data to device memory" << std::endl;
		//	return false;
		//}*/

		//if (!UpdateDescriptorSet(model)) return false;


		return true;

	}

	bool Texture::CreateDepthTexture(StrikeRenderer* renderer)
	{
		//VkFormat depthFormat = renderer->FindDepthFormat();
		//if (!CreateImage(renderer, renderer->GetVulkanParameters().SwapChain.Extent.width, renderer->GetVulkanParameters().SwapChain.Extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ImgParams.Handle))
		//	return false;

		///*if (!AllocateImageMemory(Vulkan.DepthImage.Handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Vulkan.DepthImage.Memory))
		//{
		//	std::cout << "Could not allocate memory for image" << std::endl;
		//	return false;
		//}*/

		//VkMemoryRequirements memRequirements;
		//vkGetImageMemoryRequirements(renderer->GetVulkanParameters().Device, m_ImgParams.Handle, &memRequirements);


		//VkMemoryAllocateInfo allocInfo{};
		//allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		//allocInfo.allocationSize = memRequirements.size;
		//allocInfo.memoryTypeIndex = renderer->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		//if (vkAllocateMemory(renderer->GetVulkanParameters().Device, &allocInfo, nullptr, &m_ImgParams.Memory) != VK_SUCCESS) {
		//	throw std::runtime_error("failed to allocate image memory!");
		//}

		//if (vkBindImageMemory(renderer->GetVulkanParameters().Device, m_ImgParams.Handle, m_ImgParams.Memory, 0) != VK_SUCCESS)
		//{
		//	std::cout << "Could not bind memory to an image" << std::endl;
		//	return false;
		//}

		//if (!CreateImageView(renderer, m_ImgParams, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT))
		//	return false;

		return true;
	}
}


