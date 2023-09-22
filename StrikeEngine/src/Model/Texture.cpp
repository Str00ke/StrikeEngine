#include "Model/Texture.hpp"
#include "Renderer/StrikeRenderer.hpp"
#include "Model/Model.hpp"
#include "vulkan.h"
#include <vector>


namespace StrikeEngine
{
	bool Texture::AllocateImageMemory(StrikeRenderer* renderer, VkImage image, VkMemoryPropertyFlagBits property, VkDeviceMemory* memory)
	{
		VkMemoryRequirements imgMemoryRequirements;
		vkGetImageMemoryRequirements(renderer->GetVulkanParameters().Device, m_ImgParams.Handle, &imgMemoryRequirements);

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(renderer->GetVulkanParameters().PhysicalDevice, &memoryProperties);

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
		{
			if ((imgMemoryRequirements.memoryTypeBits & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & property))
			{
				VkMemoryAllocateInfo memoryAllocateInfo =
				{
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					nullptr,
					imgMemoryRequirements.size,
					i
				};

				if (vkAllocateMemory(renderer->GetVulkanParameters().Device, &memoryAllocateInfo, nullptr, memory) == VK_SUCCESS)
					return true;
			}
		}

		return false;
	}

	bool Texture::CreateSampler(StrikeRenderer* renderer, VkSampler* sampler)
	{
		VkSamplerCreateInfo samplerCreateInfo =
		{
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			nullptr,
			0,
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VK_FALSE,
			1.0f,
			VK_FALSE,
			VK_COMPARE_OP_ALWAYS,
			0.0f,
			0.0f,
			VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
			VK_FALSE
		};

		return vkCreateSampler(renderer->GetVulkanParameters().Device, &samplerCreateInfo, nullptr, sampler) == VK_SUCCESS;
	}

	bool Texture::CreateImageView(StrikeRenderer* renderer, ImageParams& imgParams, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo imgViewCreateInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			imgParams.Handle,
			VK_IMAGE_VIEW_TYPE_2D,
			format,//VK_FORMAT_R8G8B8A8_UNORM,
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			},
			{
				aspectFlags,
				0,
				1,
				0,
				1
			}
		};

		return vkCreateImageView(renderer->GetVulkanParameters().Device, &imgViewCreateInfo, nullptr, &imgParams.View) == VK_SUCCESS;
	}

	bool Texture::CreateImage(StrikeRenderer* renderer, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image)
	{
		VkImageCreateInfo imgCreateInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			0,
			VK_IMAGE_TYPE_2D,
			format,//VK_FORMAT_R8G8B8A8_UNORM,
			{
				width,
				height,
				1
			},
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			tiling,
			usage,//VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr,
			VK_IMAGE_LAYOUT_UNDEFINED
		};

		return vkCreateImage(renderer->GetVulkanParameters().Device, &imgCreateInfo, nullptr, &image) == VK_SUCCESS;
	}

	bool Texture::TransitionImageLayout(StrikeRenderer* renderer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer cmdBuffer = renderer->BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier
		{
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			0,
			0,
			oldLayout,
			newLayout,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			image,
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				1,
				0,
				1
			}
		};

		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags destStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
			throw std::invalid_argument("Unsupported layout transition");

		vkCmdPipelineBarrier(cmdBuffer, srcStage, destStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		renderer->EndSingleTimeCommands(cmdBuffer);

		return true;
	}

	bool Texture::CopyTextureData(StrikeRenderer* renderer, char* textureData, uint32_t dataSize, uint32_t width, uint32_t height)
	{

		//Prepare data in the staging buffer
		void* stagingBufferMemoryPtr;
		if (vkMapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory, 0, dataSize, 0, &stagingBufferMemoryPtr) != VK_SUCCESS)
		{
			std::cout << "Could not map memory and upload texture data to a staging buffer" << std::endl;
			return false;
		}

		memcpy(stagingBufferMemoryPtr, textureData, dataSize);

		VkMappedMemoryRange flushRange =
		{
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			renderer->GetVulkanParameters().StagingBuffer.Memory,
			0,
			dataSize
		};

		vkFlushMappedMemoryRanges(renderer->GetVulkanParameters().Device, 1, &flushRange);

		vkUnmapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory);

		//Prepare cmd buffer to copy data from the staging buffer to the vertex buffer
		VkCommandBufferBeginInfo cmdBufferBeginInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			nullptr
		};

		VkCommandBuffer cmdBuffer = renderer->GetVulkanParameters().RenderingResources[0].CommandBuffer;

		vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);

		VkImageSubresourceRange imgSubresourceRange =
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		};

		VkImageMemoryBarrier imgMemoryBarrierFromUndefinedToTransferDst =
		{
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			nullptr,//Vulkan.Image.Handle,
			imgSubresourceRange
		};

		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgMemoryBarrierFromUndefinedToTransferDst);

		VkBufferImageCopy bufferImageCopyInfo =
		{
			0,
			0,
			0,
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				0,
				1
			},
			{
				0,
				0,
				0
			},
			{
				width,
				height,
				1
			}
		};

		vkCmdCopyBufferToImage(cmdBuffer, renderer->GetVulkanParameters().StagingBuffer.Handle, m_ImgParams.Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyInfo);

		VkImageMemoryBarrier imgMemoryBarrierFromTransferToShaderRead =
		{
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			m_ImgParams.Handle,
			imgSubresourceRange
		};

		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgMemoryBarrierFromTransferToShaderRead);

		vkEndCommandBuffer(cmdBuffer);

		//Submit cmd buffer and copy data from the staging buffer to the vertex buffer
		VkSubmitInfo submitInfo =
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			0,
			nullptr,
			nullptr,
			1,
			&cmdBuffer,
			0,
			nullptr
		};

		if (vkQueueSubmit(renderer->GetVulkanParameters().GraphicsQueue.Handle, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
			return false;

		vkDeviceWaitIdle(renderer->GetVulkanParameters().Device);

		return true;

	}

	bool Texture::Create(StrikeRenderer* renderer, Model* model)
	{
		if (!CreateDescriptorSetLayout()) return false;

		if (!CreateDescriptorPool()) return false;

		if (!AllocateDescriptorSet()) return false;


		int width = 0, height = 0, dataSize = 0;
		std::vector textureData = Tools::GetImageData(GetPath().c_str(), 4, &width, &height, nullptr, &dataSize);
		if (textureData.size() == 0)
			return false;


		VkDeviceSize bufferSize = width * height * 4;
		renderer->GetVulkanParameters().StagingBuffer.Size = width * height * 4;

		if (!renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, renderer->GetVulkanParameters().StagingBuffer))
		{
			std::cout << "Could not create vertex buffer" << std::endl;
			return false;
		}

		void* data;
		vkMapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory, 0, bufferSize, 0, &data);
		memcpy(data, textureData.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory);



		//-----Create image-----
		if (!CreateImage(renderer, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ImgParams.Handle))
		{
			std::cout << "Could not create image" << std::endl;
			return false;
		}

		if (!AllocateImageMemory(renderer, m_ImgParams.Handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_ImgParams.Memory))
		{
			std::cout << "Could not allocate memory for image" << std::endl;
			return false;
		}

		if (vkBindImageMemory(renderer->GetVulkanParameters().Device, m_ImgParams.Handle, m_ImgParams.Memory, 0) != VK_SUCCESS)
		{
			std::cout << "Could not bind memory to an image" << std::endl;
			return false;
		}
		//----------

		TransitionImageLayout(renderer, m_ImgParams.Handle, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		renderer->CopyBufferToImage(renderer->GetVulkanParameters().StagingBuffer.Handle, m_ImgParams.Handle, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		TransitionImageLayout(renderer, m_ImgParams.Handle, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


		vkDestroyBuffer(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Handle, nullptr);
		vkFreeMemory(renderer->GetVulkanParameters().Device, renderer->GetVulkanParameters().StagingBuffer.Memory, nullptr);


		if (!CreateImageView(renderer, m_ImgParams, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT))
		{
			std::cout << "Could not create image view" << std::endl;
			return false;
		}

		if (!CreateSampler(renderer, &m_ImgParams.Sampler))
		{
			std::cout << "Could not create sampler" << std::endl;
			return false;
		}

		/*if (!CopyTextureData(&textureData[0], dataSize, width, height))
		{
			std::cout << "Could not upload texture data to device memory" << std::endl;
			return false;
		}*/

		if (!UpdateDescriptorSet(model)) return false;


		return true;

	}

	bool Texture::CreateDepthTexture(StrikeRenderer* renderer)
	{
		VkFormat depthFormat = renderer->FindDepthFormat();
		if (!CreateImage(renderer, renderer->GetVulkanParameters().SwapChain.Extent.width, renderer->GetVulkanParameters().SwapChain.Extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ImgParams.Handle))
			return false;

		/*if (!AllocateImageMemory(Vulkan.DepthImage.Handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Vulkan.DepthImage.Memory))
		{
			std::cout << "Could not allocate memory for image" << std::endl;
			return false;
		}*/

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(renderer->GetVulkanParameters().Device, m_ImgParams.Handle, &memRequirements);


		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = renderer->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(renderer->GetVulkanParameters().Device, &allocInfo, nullptr, &m_ImgParams.Memory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		if (vkBindImageMemory(renderer->GetVulkanParameters().Device, m_ImgParams.Handle, m_ImgParams.Memory, 0) != VK_SUCCESS)
		{
			std::cout << "Could not bind memory to an image" << std::endl;
			return false;
		}

		if (!CreateImageView(renderer, m_ImgParams, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT))
			return false;

		return true;
	}

	bool Texture::CreateDescriptorSetLayout()
	{
		/*std::vector<VkDescriptorSetLayoutBinding> layoutBindings =
		{
			{
				0,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{
				1,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT,
				nullptr
			}

		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo =
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32_t>(layoutBindings.size()),
			layoutBindings.data()
		};

		if (vkCreateDescriptorSetLayout(Vulkan.Device, &descriptorSetLayoutCreateInfo, nullptr, &Vulkan.DescriptorSet.Layout) != VK_SUCCESS)
		{
			std::cout << "Could not create descriptor set layout" << std::endl;
			return false;
		}*/

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(StrikeRenderer::Instance()->GetVulkanParameters().Device, &layoutInfo, nullptr, &m_DescParams.Layout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		return true;
	}

	bool Texture::CreateDescriptorPool()
	{
		std::vector<VkDescriptorPoolSize> poolSizes =
		{
			{
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1
			},
			{
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1
			}
		};

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			nullptr,
			0,
			1,
			static_cast<uint32_t>(poolSizes.size()),
			poolSizes.data()
		};

		if (vkCreateDescriptorPool(StrikeRenderer::Instance()->GetVulkanParameters().Device, &descriptorPoolCreateInfo, nullptr, &m_DescParams.Pool) != VK_SUCCESS)
		{
			std::cout << "Could not create descriptor pool" << std::endl;
			return false;
		}

		return true;
	}

	bool Texture::AllocateDescriptorSet()
	{
		/*VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			nullptr,
			Vulkan.DescriptorSet.Pool,
			1,
			&Vulkan.DescriptorSet.Layout
		};

		if (vkAllocateDescriptorSets(Vulkan.Device, &descriptorSetAllocateInfo, &Vulkan.DescriptorSet.Handle) != VK_SUCCESS)
		{
			std::cout << "Could not allocate descriptor set" << std::endl;
			return false;
		}*/

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescParams.Pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescParams.Layout;

		if (vkAllocateDescriptorSets(StrikeRenderer::Instance()->GetVulkanParameters().Device, &allocInfo, &m_DescParams.Handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}


		return true;
	}

	bool Texture::UpdateDescriptorSet(Model* model)
	{
		/*VkDescriptorImageInfo imgInfo =
		{
			Vulkan.Image.Sampler,
			Vulkan.Image.View,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkDescriptorBufferInfo bufferInfo =
		{
			Vulkan.UniformBuffer.Handle,
			0,
			Vulkan.UniformBuffer.Size
		};

		std::vector<VkWriteDescriptorSet> descriptorWrites =
		{
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				Vulkan.DescriptorSet.Handle,
				0,
				0,
				1,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				&imgInfo,
				nullptr,
				nullptr
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				Vulkan.DescriptorSet.Handle,
				1,
				0,
				1,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				nullptr,
				&bufferInfo,
				nullptr
			}
		};

		vkUpdateDescriptorSets(Vulkan.Device, static_cast<uint32_t>(descriptorWrites.size()), &descriptorWrites[0], 0, nullptr);
		*/

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = model->GetUniformBuffer().Handle;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_ImgParams.View;
		imageInfo.sampler = m_ImgParams.Sampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_DescParams.Handle;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_DescParams.Handle;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;


		



		vkUpdateDescriptorSets(StrikeRenderer::Instance()->GetVulkanParameters().Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		/*
		std::vector<std::array<VkWriteDescriptorSet, 2>> descriptorWritesArr = {};

		for (size_t i = 0; i < toRend.size(); ++i)
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = toRend[i]->GetTexture().GetParams().View;
			imageInfo.sampler = toRend[i]->GetTexture().GetParams().Sampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = Vulkan.DescriptorSet.Handle;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = Vulkan.DescriptorSet.Handle;
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;



			descriptorWritesArr.push_back(descriptorWrites);

		}


		vkUpdateDescriptorSets(Vulkan.Device, static_cast<uint32_t>(descriptorWritesArr.size()), descriptorWritesArr.data(), 0, nullptr);
		*/

		return true;
	}

	ImageParams& Texture::GetImgParams()
	{
		return m_ImgParams;
	}

	DescriptorSetParameters& Texture::GetDescParams()
	{
		return m_DescParams;
	}

}


