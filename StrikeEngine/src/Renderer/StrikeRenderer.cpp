#include "Renderer/StrikeRenderer.hpp"
#include <vector>
#include "Vulkan/VkFunctions.hpp"
#include "Core/Time.hpp"
#include "Model/Vertex.hpp"
#include "Model/RenderableResourceController.hpp"
#include "Model/Model.hpp"
#include "Model/Renderable.hpp"
#include "Model/Camera.hpp"


namespace StrikeEngine
{
	StrikeRenderer* StrikeRenderer::m_instance = nullptr;

	StrikeRenderer* StrikeRenderer::Instance()
	{
		if (m_instance == nullptr)
			throw std::runtime_error("Renderer instance is nullptr");

		return m_instance;
	}

	/*void recreateSwapChain() {
		int width = 0, height = 0;
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device);

		cleanupSwapChain();

		createSwapChain();
		createImageViews();
		createDepthResources();
		createFramebuffers();
	}


	void cleanupSwapChain() {
		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);
	
		...
	}*/
	bool StrikeRenderer::AllocateBufferMemory(VkBuffer buffer, VkMemoryPropertyFlags property, VkDeviceMemory* memory)
	{
		VkMemoryRequirements bufferMemoryRequirements;
		vkGetBufferMemoryRequirements(Vulkan.Device, buffer, &bufferMemoryRequirements);

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(Vulkan.PhysicalDevice, &memoryProperties);

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
		{
			if ((bufferMemoryRequirements.memoryTypeBits & (1 << i)) && ((memoryProperties.memoryTypes[i].propertyFlags & property)))
			{
				VkMemoryAllocateInfo memoryAllocateInfo =
				{
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					nullptr,
					bufferMemoryRequirements.size,
					i
				};

				if (vkAllocateMemory(Vulkan.Device, &memoryAllocateInfo, nullptr, memory) == VK_SUCCESS)
					return true;
			}
		}

		return false;
	}

	

	bool StrikeRenderer::AllocateCommandBuffers(VkCommandPool pool, uint32_t count, VkCommandBuffer* commandBuffers)
	{
		VkCommandBufferAllocateInfo cmdBufferAllocateInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			pool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			count
		};

		if (vkAllocateCommandBuffers(Vulkan.Device, &cmdBufferAllocateInfo, commandBuffers) != VK_SUCCESS)
		{
			std::cout << "Could not allocate command buffer" << std::endl;
			return false;
		}

		return true;
	}

	VkCommandBuffer StrikeRenderer::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocateInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			Vulkan.CommandPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1
		};

		VkCommandBuffer cmdBuffer;
		vkAllocateCommandBuffers(Vulkan.Device, &allocateInfo, &cmdBuffer);
		VkCommandBufferBeginInfo cmdBufferBeginInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			nullptr
		};

		vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);

		return cmdBuffer;
	}

	bool StrikeRenderer::CreateVkInstance()
	{
		if (enableValidationLayers && !CheckValidationLayerSupport())
			throw std::runtime_error("Validation layers requested, but not available");

		uint32_t extensionsCount = 0;
		if ((vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr) != VK_SUCCESS) || extensionsCount == 0)
		{
			std::cout << "Error occured during instance extensions enumeration" << std::endl;
			return false;
		}

		std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
		if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, availableExtensions.data()) != VK_SUCCESS)
		{
			std::cout << "Error occured during instance extensions enumeration" << std::endl;
			return false;
		}



		std::vector<const char*> extensions =
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		};

		for (size_t i = 0; i < extensions.size(); ++i)
		{
			if (!CheckExtensionAvailability(extensions[i], availableExtensions))
			{
				std::cout << "Could not find instance extension name \"" << extensions[i] << "\"!" << std::endl;
				return false;
			}
		}

		VkApplicationInfo appInfo =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			"Strike Engine Vulkan Renderer",
			VK_MAKE_VERSION(1, 0, 0),
			"Strike Engine",
			VK_MAKE_VERSION(1, 0, 0),
			VK_MAKE_VERSION(1, 0, 0)
		};

		VkInstanceCreateInfo instanceCreateInfo =
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0,
			&appInfo,
			0,
			nullptr,
			static_cast<uint32_t>(extensions.size()),
			extensions.data()
		};

		if (enableValidationLayers)
		{
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &Vulkan.Instance) != VK_SUCCESS)
		{
			std::cout << "Could not create Vulkan Instance!" << std::endl;
			return false;
		}
		return true;

	}

	bool StrikeRenderer::CreateDevice()
	{
		uint32_t numDevices = 0;
		if ((vkEnumeratePhysicalDevices(Vulkan.Instance, &numDevices, nullptr) != VK_SUCCESS) || (numDevices == 0))
		{
			std::cout << "Error occurred during physical devices enumeration" << std::endl;
			return false;
		}

		std::vector<VkPhysicalDevice> physDevices(numDevices);
		if (vkEnumeratePhysicalDevices(Vulkan.Instance, &numDevices, &physDevices[0]) != VK_SUCCESS)
		{
			std::cout << "Error occurred during physical devices enumeration" << std::endl;
			return false;
		}

		uint32_t selectedGraphicsQueueFamilyIndex = UINT32_MAX;
		uint32_t selectedPresentQueueFamilyIndex = UINT32_MAX;

		VkPhysicalDevice selectedPhysDevice = VK_NULL_HANDLE;
		uint32_t selectedQueueFamilyIndex = UINT32_MAX;
		for (uint32_t i = 0; i < numDevices; ++i)
		{
			if (CheckPhysicalDeviceProperties(physDevices[i], selectedGraphicsQueueFamilyIndex, selectedPresentQueueFamilyIndex))
			{
				Vulkan.PhysicalDevice = physDevices[i];
				break;
			}
		}

		if (Vulkan.PhysicalDevice == VK_NULL_HANDLE)
		{
			std::cout << "Could not select physical device based on the chosen properties" << std::endl;
			return false;
		}

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;
		std::vector<float> queuePriorities = { 1.0f };

		queueCreateInfo.push_back(
			{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr,
				0,
				selectedGraphicsQueueFamilyIndex,
				static_cast<uint32_t>(queuePriorities.size()),
				queuePriorities.data()
			});

		if (selectedGraphicsQueueFamilyIndex != selectedPresentQueueFamilyIndex)
		{
			queueCreateInfo.push_back(
				{
					VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					nullptr,
					0,
					selectedPresentQueueFamilyIndex,
					static_cast<uint32_t>(queuePriorities.size()),
					queuePriorities.data()
				});
		}

		std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		VkDeviceCreateInfo deviceCreateInfo =
		{
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32_t>(queueCreateInfo.size()),
			queueCreateInfo.data(),
			0,
			nullptr,
			static_cast<uint32_t>(extensions.size()),
			extensions.data(),
			nullptr
		};

		if (vkCreateDevice(Vulkan.PhysicalDevice, &deviceCreateInfo, nullptr, &Vulkan.Device))
		{
			std::cout << "Could not create Vulkan device" << std::endl;
			return false;
		}

		Vulkan.GraphicsQueue.FamilyIndex = selectedGraphicsQueueFamilyIndex;
		Vulkan.PresentQueue.FamilyIndex = selectedPresentQueueFamilyIndex;
		return true;
	}

	bool StrikeRenderer::CheckPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, uint32_t& selectedGraphicsQueueFamilyIndex, uint32_t& selectedPresentQueueFamilyIndex)
	{
		uint32_t extensions_count = 0;
		if ((vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensions_count, nullptr) != VK_SUCCESS) ||
			(extensions_count == 0)) {
			std::cout << "Error occurred during physical device " << physicalDevice << " extensions enumeration!" << std::endl;
			return false;
		}

		std::vector<VkExtensionProperties> available_extensions(extensions_count);
		if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensions_count, available_extensions.data()) != VK_SUCCESS) {
			std::cout << "Error occurred during physical device " << physicalDevice << " extensions enumeration!" << std::endl;
			return false;
		}

		std::vector<const char*> device_extensions = {
		  VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		for (size_t i = 0; i < device_extensions.size(); ++i) {
			if (!CheckExtensionAvailability(device_extensions[i], available_extensions)) {
				std::cout << "Physical device " << physicalDevice << " doesn't support extension named \"" << device_extensions[i] << "\"!" << std::endl;
				return false;
			}
		}

		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures   device_features;

		vkGetPhysicalDeviceProperties(physicalDevice, &device_properties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &device_features);

		uint32_t major_version = VK_VERSION_MAJOR(device_properties.apiVersion);

		if ((major_version < 1) ||
			(device_properties.limits.maxImageDimension2D < 4096)) {
			std::cout << "Physical device " << physicalDevice << " doesn't support required parameters!" << std::endl;
			return false;
		}

		uint32_t queue_families_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, nullptr);
		if (queue_families_count == 0) {
			std::cout << "Physical device " << physicalDevice << " doesn't have any queue families!" << std::endl;
			return false;
		}

		std::vector<VkQueueFamilyProperties>  queue_family_properties(queue_families_count);
		std::vector<VkBool32>                 queue_present_support(queue_families_count);

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, queue_family_properties.data());

		uint32_t graphics_queue_family_index = UINT32_MAX;
		uint32_t present_queue_family_index = UINT32_MAX;

		for (uint32_t i = 0; i < queue_families_count; ++i) {
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, Vulkan.PresentationSurface, &queue_present_support[i]);



			if ((queue_family_properties[i].queueCount > 0) &&
				(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				// Select first queue that supports graphics
				if (graphics_queue_family_index == UINT32_MAX) {
					graphics_queue_family_index = i;
				}

				// If there is queue that supports both graphics and present - prefer it
				if (queue_present_support[i]) {
					selectedGraphicsQueueFamilyIndex = i;
					selectedPresentQueueFamilyIndex = i;
					return true;
				}
			}
		}

		// We don't have queue that supports both graphics and present so we have to use separate queues
		for (uint32_t i = 0; i < queue_families_count; ++i) {
			if (queue_present_support[i]) {
				present_queue_family_index = i;
				break;
			}
		}

		// If this device doesn't support queues with graphics and present capabilities don't use it
		if ((graphics_queue_family_index == UINT32_MAX) ||
			(present_queue_family_index == UINT32_MAX)) {
			std::cout << "Could not find queue family with required properties on physical device " << physicalDevice << "!" << std::endl;
			return false;
		}

		selectedGraphicsQueueFamilyIndex = graphics_queue_family_index;
		selectedPresentQueueFamilyIndex = present_queue_family_index;
		return true;
	}

	bool StrikeRenderer::CheckExtensionAvailability(const char* extensionName, const std::vector<VkExtensionProperties>& availableExtensions)
	{
		for (size_t i = 0; i < availableExtensions.size(); ++i)
		{
			if (strcmp(availableExtensions[i].extensionName, extensionName) == 0)
				return true;
		}

		return false;
	}

	bool StrikeRenderer::CreatePresentationSurface()
	{
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo =
		{
			VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			nullptr,
			0,
			m_strikeWin->GetParams().Instance,
			m_strikeWin->GetParams().Handle
		};

		if (vkCreateWin32SurfaceKHR(Vulkan.Instance, &surfaceCreateInfo, nullptr, &Vulkan.PresentationSurface) == VK_SUCCESS)
			return true;

		std::cout << "Could not create presentation surface" << std::endl;
	}

	bool StrikeRenderer::CreateSwapChainImageViews()
	{

		for (size_t i = 0; i < Vulkan.SwapChain.Images.size(); ++i) {
			VkImageViewCreateInfo imgViewCreateInfo =
			{
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				nullptr,
				0,
				Vulkan.SwapChain.Images[i].Handle,
				VK_IMAGE_VIEW_TYPE_2D,
				Vulkan.SwapChain.Format,
				{
					VK_COMPONENT_SWIZZLE_IDENTITY,
					VK_COMPONENT_SWIZZLE_IDENTITY,
					VK_COMPONENT_SWIZZLE_IDENTITY,
					VK_COMPONENT_SWIZZLE_IDENTITY
				},
				{
					VK_IMAGE_ASPECT_COLOR_BIT,
					0,
					1,
					0,
					1
				}
			};

			if (vkCreateImageView(Vulkan.Device, &imgViewCreateInfo, nullptr, &Vulkan.SwapChain.Images[i].View) != VK_SUCCESS)
			{
				std::cout << "Could not create image view for framebuffer" << std::endl;
				return false;
			}
		}

		m_CanRender = true;

		return true;
	}

	bool StrikeRenderer::CreateSwapChain()
	{
		m_CanRender = false;

		if (Vulkan.Device != VK_NULL_HANDLE)
			vkDeviceWaitIdle(Vulkan.Device);

		for (size_t i = 0; i < Vulkan.SwapChain.Images.size(); ++i)
		{
			if (Vulkan.SwapChain.Images[i].View != VK_NULL_HANDLE)
			{
				vkDestroyImageView(Vulkan.Device, Vulkan.SwapChain.Images[i].View, nullptr);
				Vulkan.SwapChain.Images[i].View = VK_NULL_HANDLE;
			}
		}
		Vulkan.SwapChain.Images.clear();

		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &surfaceCapabilities) != VK_SUCCESS)
		{
			std::cout << "Could not check presentation surface capabilities" << std::endl;
			return false;
		}

		uint32_t formatsCount;
		if ((vkGetPhysicalDeviceSurfaceFormatsKHR(Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &formatsCount, nullptr) != VK_SUCCESS) || (formatsCount == 0))
		{
			std::cout << "Error occured during presentation surface formats enumeration" << std::endl;
			return false;
		}

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
		if (vkGetPhysicalDeviceSurfaceFormatsKHR(Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &formatsCount, surfaceFormats.data()) != VK_SUCCESS)
		{
			std::cout << "Error occured during presentation surface formats enumeration" << std::endl;
			return false;
		}

		uint32_t presentModesCount;
		if ((vkGetPhysicalDeviceSurfacePresentModesKHR(Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &presentModesCount, nullptr) != VK_SUCCESS) || (presentModesCount == 0))
		{
			std::cout << "Error occured during presentation surface present modes enumeration" << std::endl;
			return false;
		}

		std::vector<VkPresentModeKHR> presentModes(presentModesCount);
		if ((vkGetPhysicalDeviceSurfacePresentModesKHR(Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &presentModesCount, presentModes.data()) != VK_SUCCESS))
		{
			std::cout << "Error occured during presentation surface formats enumeration" << std::endl;
			return false;
		}

		uint32_t desiredNumberOfImages = GetSwapChainNumImages(surfaceCapabilities);
		VkSurfaceFormatKHR desiredFormat = GetSwapChainFormat(surfaceFormats);
		VkExtent2D desiredExtent = GetSwapChainExtent(surfaceCapabilities);
		VkImageUsageFlags desiredUsage = GetSwapChainUsageFlags(surfaceCapabilities);
		VkSurfaceTransformFlagBitsKHR desiredTransform = GetSwapChainTransform(surfaceCapabilities);
		VkPresentModeKHR desiredPresentMode = GetSwapChainPresentMode(presentModes);
		VkSwapchainKHR oldSwapChain = Vulkan.SwapChain.Handle;

		if (static_cast<int>(desiredUsage) == -1)
			return false;

		if (static_cast<int>(desiredPresentMode) == -1)
			return false;

		if ((desiredExtent.width == 0) || (desiredExtent.height == 0)) {
			//If size null, don't render anything, don't create swap chain, but don't kill app (window may be minimized)
			return true;
		}

		VkSwapchainCreateInfoKHR swapChainCreateInfo =
		{
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,
			Vulkan.PresentationSurface,
			desiredNumberOfImages,
			desiredFormat.format,
			desiredFormat.colorSpace,
			desiredExtent,
			1,
			desiredUsage,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr,
			desiredTransform,
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			desiredPresentMode,
			VK_TRUE,
			oldSwapChain
		};

		if (vkCreateSwapchainKHR(Vulkan.Device, &swapChainCreateInfo, nullptr, &Vulkan.SwapChain.Handle) != VK_SUCCESS)
		{
			std::cout << "Could not create swap chain" << std::endl;
			return false;
		}

		if (oldSwapChain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(Vulkan.Device, oldSwapChain, nullptr);

		Vulkan.SwapChain.Format = desiredFormat.format;

		uint32_t imageCount = 0;
		if ((vkGetSwapchainImagesKHR(Vulkan.Device, Vulkan.SwapChain.Handle, &imageCount, nullptr) != VK_SUCCESS) || imageCount == 0)
		{
			std::cout << "Could not get swap chain images" << std::endl;
			return false;
		}
		Vulkan.SwapChain.Images.resize(imageCount);

		std::vector<VkImage> images(imageCount);
		if (vkGetSwapchainImagesKHR(Vulkan.Device, Vulkan.SwapChain.Handle, &imageCount, images.data()) != VK_SUCCESS)
		{
			std::cout << "Could not get swap chain images" << std::endl;
			return false;
		}

		for (size_t i = 0; i < Vulkan.SwapChain.Images.size(); ++i)
		{
			Vulkan.SwapChain.Images[i].Handle = images[i];
		}

		Vulkan.SwapChain.Extent = desiredExtent;
		return CreateSwapChainImageViews();
	}

	bool StrikeRenderer::CreateFrameBuffers(VkFramebuffer& frameBuffer, VkImageView imageView, Model* model)
	{
		if (frameBuffer != VK_NULL_HANDLE)
			vkDestroyFramebuffer(Vulkan.Device, frameBuffer, nullptr);

		std::array<VkImageView, 2> attachments =
		{
			imageView,
			model->GetDepthTexture().GetImgParams().View
		};

		VkFramebufferCreateInfo frameBufferCreateInfo =
		{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			Vulkan.RenderPass,
			static_cast<uint32_t>(attachments.size()),
			attachments.data(),
			Vulkan.SwapChain.Extent.width,
			Vulkan.SwapChain.Extent.height,
			1
		};

		if (vkCreateFramebuffer(Vulkan.Device, &frameBufferCreateInfo, nullptr, &frameBuffer) != VK_SUCCESS)
		{
			std::cout << "Could not create a framebuffer" << std::endl;
			return false;
		}

		return true;
	}

	VkParams& StrikeRenderer::GetVulkanParameters()
	{
		return Vulkan;
	}

	StrikeWindow* StrikeRenderer::GetStrikeWindow()
	{
		return m_strikeWin;
	}

	Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> StrikeRenderer::CreateShaderModule(const char* filename)
	{
		const std::vector<char> code = Tools::GetBinaryFileContents(filename);
		if (code.size() == 0)
			return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>();

		VkShaderModuleCreateInfo shaderModuleCreateInfo =
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,
			code.size(),
			reinterpret_cast<const uint32_t*>(&code[0])
		};

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(Vulkan.Device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			std::cout << "Could not create shader module from a " << filename << " file" << std::endl;
			return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>();
		}

		return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>(shaderModule, vkDestroyShaderModule, Vulkan.Device);
	}

	bool StrikeRenderer::CopyVertexData()
	{
		/*
		//Prepare data in the staging buffer
		std::vector<float>& vertexData = Vulkan.Model.vertices;//GetVertexData();

		void* stagingBufferMemoryPointer;
		if (vkMapMemory(Vulkan.Device, Vulkan.StagingBuffer.Memory, 0, Vulkan.Model.VertexBuffer.Size, 0, &stagingBufferMemoryPointer) != VK_SUCCESS)
		{
			std::cout << "Could not map memory an upload data to a staging buffer" << std::endl;
			return false;
		}

		memcpy(stagingBufferMemoryPointer, Vulkan.Model.vertices.data(), Vulkan.Model.VertexBuffer.Size);

		VkMappedMemoryRange flushRange =
		{
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			Vulkan.StagingBuffer.Memory,
			0,
			Vulkan.Model.VertexBuffer.Size
		};
		vkFlushMappedMemoryRanges(Vulkan.Device, 1, &flushRange);

		vkUnmapMemory(Vulkan.Device, Vulkan.StagingBuffer.Memory);

		//Prepare command buffer to copy data from the staging buffer to the vertex buffer
		VkCommandBufferBeginInfo cmdBufferBeginInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			nullptr
		};

		VkCommandBuffer cmdBuffer = Vulkan.RenderingResources[0].CommandBuffer;

		vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);

		VkBufferCopy bufferCopyInfo =
		{
			0,
			0,
			Vulkan.Model.VertexBuffer.Size
		};
		vkCmdCopyBuffer(cmdBuffer, Vulkan.StagingBuffer.Handle, Vulkan.Model.VertexBuffer.Handle, 1, &bufferCopyInfo);

		VkBufferMemoryBarrier bufferMemoryBarrier =
		{
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			VK_ACCESS_MEMORY_WRITE_BIT,
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			Vulkan.Model.VertexBuffer.Handle,
			0,
			VK_WHOLE_SIZE
		};
		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);

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

		if (vkQueueSubmit(Vulkan.GraphicsQueue.Handle, 1, &submitInfo, VK_NULL_HANDLE))
			return false;

		vkDeviceWaitIdle(Vulkan.Device);
		*/
		return true;


	}

	bool StrikeRenderer::CopyUniformBufferData()
	{
		/*
		const std::array<float, 16> uniformData = GetUniformBufferData();

		void* stagingBufferMemoryPointer;
		if (vkMapMemory(Vulkan.Device, Vulkan.StagingBuffer.Memory, 0, Vulkan.UniformBuffer.Size, 0, &stagingBufferMemoryPointer) != VK_SUCCESS)
		{
			std::cout << "Could not map memory and upload data to a staging buffer" << std::endl;
			return false;
		}

		memcpy(stagingBufferMemoryPointer, uniformData.data(), Vulkan.UniformBuffer.Size);

		VkMappedMemoryRange flushRange =
		{
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			Vulkan.StagingBuffer.Memory,
			0,
			Vulkan.UniformBuffer.Size
		};

		vkFlushMappedMemoryRanges(Vulkan.Device, 1, &flushRange);

		vkUnmapMemory(Vulkan.Device, Vulkan.StagingBuffer.Memory);

		//Prepare cmd buffer to copy data from the staging buffer to the uniform buffer
		VkCommandBuffer cmdBuffer = Vulkan.RenderingResources[0].CommandBuffer;

		VkCommandBufferBeginInfo cmdBufferBeginInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			nullptr
		};

		vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);

		VkBufferCopy bufferCopyInfo =
		{
			0,
			0,
			Vulkan.UniformBuffer.Size
		};
		vkCmdCopyBuffer(cmdBuffer, Vulkan.StagingBuffer.Handle, Vulkan.UniformBuffer.Handle, 1, &bufferCopyInfo);

		VkBufferMemoryBarrier bufferMemoryBarrier =
		{
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			Vulkan.UniformBuffer.Handle,
			0,
			VK_WHOLE_SIZE
		};
		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);

		vkEndCommandBuffer(cmdBuffer);

		//Submit the cmd buffer and copy data from the staging buffer to the vertex buffer
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

		if (vkQueueSubmit(Vulkan.GraphicsQueue.Handle, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
			return false;

		vkDeviceWaitIdle(Vulkan.Device);
		*/
		return true;

	}

	bool StrikeRenderer::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound) return false;
		}

		return true;
	}

	bool StrikeRenderer::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer cmdBuffer = BeginSingleTimeCommands();

		VkBufferImageCopy region =
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

		vkCmdCopyBufferToImage(cmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		EndSingleTimeCommands(cmdBuffer);
		return true;
	}

	bool StrikeRenderer::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment =
		{
			0,
			Vulkan.SwapChain.Format,
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentDescription depthAttachment =
		{
			0,
			FindDepthFormat(),
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};


		VkAttachmentReference colorAttachmentRef =
		{
			0,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkAttachmentReference depthAttachmentRef =
		{
			1,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpassDesc =
		{
			0,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			0,
			nullptr,
			1,
			&colorAttachmentRef,
			nullptr,
			&depthAttachmentRef,
			0,
			nullptr
		};

		VkSubpassDependency dependency =
		{
			VK_SUBPASS_EXTERNAL,
			0,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_DEPENDENCY_BY_REGION_BIT
		};

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

		VkRenderPassCreateInfo renderPassCreateInfo =
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32_t>(attachments.size()),
			attachments.data(),
			1,
			&subpassDesc,
			1,
			&dependency
		};

		if (vkCreateRenderPass(Vulkan.Device, &renderPassCreateInfo, nullptr, &Vulkan.RenderPass) != VK_SUCCESS)
		{
			std::cout << "Could not create render pass" << std::endl;
			return false;
		}
		return true;
	}

	bool StrikeRenderer::CreateRenderingResources()
	{
		/*if (!CreateCommandBuffers())
			return false;*/

		if (!CreateSemaphore())
			return false;

		if (!CreateFences())
			return false;

		return true;
	}

	bool StrikeRenderer::CreateStagingBuffer()
	{
		Vulkan.StagingBuffer.Size = 1000000;
		if (!CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, Vulkan.StagingBuffer))
		{
			std::cout << "Could not staging buffer" << std::endl;
			return false;
		}

		return true;
	}

	bool StrikeRenderer::CreatePipelineLayout(Model* model)
	{
		VkPipelineLayoutCreateInfo layoutCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			1,
			&model->GetTexture().GetDescParams().Layout,
			0,
			nullptr
		};

		if (vkCreatePipelineLayout(Vulkan.Device, &layoutCreateInfo, nullptr, &Vulkan.PipelineLayout) != VK_SUCCESS)
		{
			std::cout << "Could not create pipeline layout" << std::endl;
			return false;
		}

		return true;
	}

	bool StrikeRenderer::CreatePipeline()
	{
		Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> vertexShaderModule = CreateShaderModule("C:/Users/cgarrigues/_Personal/StrikeEngine/StrikeEngine/Data/vert.spv");
		Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> fragmentShaderModule = CreateShaderModule("C:/Users/cgarrigues/_Personal/StrikeEngine/StrikeEngine/Data/frag.spv");

		if (!vertexShaderModule || !fragmentShaderModule)
			return false;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfo =
		{
			//Vertex shader
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,
				VK_SHADER_STAGE_VERTEX_BIT,
				vertexShaderModule.Get(),
				"main",
				nullptr
			},

			//Fragment shader
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				fragmentShaderModule.Get(),
				"main",
				nullptr
			}
		};

		VkVertexInputBindingDescription vertexBindingDesc =
		{
			0,
			sizeof(Vertex),
			VK_VERTEX_INPUT_RATE_VERTEX
		};

		std::vector<VkVertexInputAttributeDescription> vertexAttributeDesc =
		{
			{ 0, vertexBindingDesc.binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)},
			{ 1, vertexBindingDesc.binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) },
			{ 2, vertexBindingDesc.binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, texCoord) },
		};

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			&vertexBindingDesc,
			static_cast<uint32_t>(vertexAttributeDesc.size()),
			vertexAttributeDesc.data()
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE
		};

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			nullptr,
			1,
			nullptr
		};

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_FALSE,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
			VK_FALSE,
			0.0f,
			0.0f,
			0.0f,
			1.0f,
		};

		VkPipelineMultisampleStateCreateInfo multisampleStageCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_SAMPLE_COUNT_1_BIT,
			VK_FALSE,
			1.0f,
			nullptr,
			VK_FALSE,
			VK_FALSE,
		};

		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS,
			VK_FALSE,
			VK_FALSE,
			{},
			{},
			0.0f,
			1.0f,
		};

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState =
		{
			VK_FALSE,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			1,
			&colorBlendAttachmentState,
			{ 0.0f, 0.0f, 0.0f, 0.0f }
		};

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32_t>(dynamicStates.size()),
			dynamicStates.data()
		};

		VkGraphicsPipelineCreateInfo pipelineCreateInfo =
		{
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32_t>(shaderStageCreateInfo.size()),
			shaderStageCreateInfo.data(),
			&vertexInputStateCreateInfo,
			&inputAssemblyStateCreateInfo,
			nullptr,
			&viewportStateCreateInfo,
			&rasterizationStateCreateInfo,
			&multisampleStageCreateInfo,
			&depthStencilCreateInfo,
			&colorBlendStateCreateInfo,
			&dynamicStateCreateInfo,
			Vulkan.PipelineLayout,
			Vulkan.RenderPass,
			0,
			VK_NULL_HANDLE,
			-1
		};

		if (vkCreateGraphicsPipelines(Vulkan.Device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &Vulkan.GraphicsPipeline) != VK_SUCCESS)
		{
			std::cout << "Could not create graphics pipeline" << std::endl;
			return false;
		}

		return true;
	}
	
	bool StrikeRenderer::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryProperty, BufferParameters& buffer)
	{
		VkBufferCreateInfo bufferCreateInfo =
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			nullptr,
			0,
			buffer.Size,
			usageFlags,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr
		};

		if (vkCreateBuffer(Vulkan.Device, &bufferCreateInfo, nullptr, &buffer.Handle) != VK_SUCCESS)
		{
			std::cout << "Could not create buffer" << std::endl;
			return false;
		}

		if (!AllocateBufferMemory(buffer.Handle, memoryProperty, &buffer.Memory))
		{
			std::cout << "Could not allocate memory for a buffer" << std::endl;
			return false;
		}

		if (vkBindBufferMemory(Vulkan.Device, buffer.Handle, buffer.Memory, 0) != VK_SUCCESS)
		{
			std::cout << "Could not bind memory to a buffer" << std::endl;
			return false;
		}
		return true;
	}

	bool StrikeRenderer::CreateFences()
	{
		VkFenceCreateInfo fenceCreateInfo =
		{
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			nullptr,
			VK_FENCE_CREATE_SIGNALED_BIT
		};

		for (size_t i = 0; i < Vulkan.RenderingResources.size(); ++i)
		{
			if (vkCreateFence(Vulkan.Device, &fenceCreateInfo, nullptr, &Vulkan.RenderingResources[i].Fence) != VK_SUCCESS)
			{
				std::cout << "Could not create a fence" << std::endl;
				return false;
			}
		}


		return true;
	}

	bool StrikeRenderer::CreateSemaphore()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo =
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			nullptr,
			0
		};

		for (size_t i = 0; i < Vulkan.RenderingResources.size(); ++i)
		{
			if ((vkCreateSemaphore(Vulkan.Device, &semaphoreCreateInfo, nullptr, &Vulkan.RenderingResources[i].ImageAvailableSemaphore) != VK_SUCCESS) || (vkCreateSemaphore(Vulkan.Device, &semaphoreCreateInfo, nullptr, &Vulkan.RenderingResources[i].FinishedRenderingSemaphore) != VK_SUCCESS))
			{
				std::cout << "Could not create semaphore" << std::endl;
				return false;
			}
		}

		return true;
	}

	bool StrikeRenderer::CreateCommandBuffers()
	{
		if (!CreateCommandPool(Vulkan.GraphicsQueue.FamilyIndex, &Vulkan.CommandPool))
		{
			std::cout << "Could not create command pool" << std::endl;
			return false;
		}

		for (size_t i = 0; i < Vulkan.RenderingResources.size(); ++i)
		{
			if (!AllocateCommandBuffers(Vulkan.CommandPool, 1, &Vulkan.RenderingResources[i].CommandBuffer))
			{
				std::cout << "Could not allocate command buffer" << std::endl;
				return false;
			}
		}
		return true;
	}

	bool StrikeRenderer::CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPool* pool)
	{
		VkCommandPoolCreateInfo cmdPoolCreateInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			nullptr,
			VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			queueFamilyIndex
		};

		if (vkCreateCommandPool(Vulkan.Device, &cmdPoolCreateInfo, nullptr, pool) != VK_SUCCESS)
		{
			std::cout << "Could not create command pool" << std::endl;
			return false;
		}

		return true;
	}

	void StrikeRenderer::DestroyBuffer(BufferParameters& buffer)
	{
		if (buffer.Handle != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(Vulkan.Device, buffer.Handle, nullptr);
			buffer.Handle = VK_NULL_HANDLE;
		}

		if (buffer.Memory != VK_NULL_HANDLE)
		{
			vkFreeMemory(Vulkan.Device, buffer.Memory, nullptr);
			buffer.Memory = VK_NULL_HANDLE;
		}
	}

	bool StrikeRenderer::Draw()
	{

		static size_t resourceIndex = 0;
		RenderingResourceData& currentRenderingResource = Vulkan.RenderingResources[resourceIndex];
		VkSwapchainKHR swapChain = Vulkan.SwapChain.Handle;
		uint32_t imageIndex;

		resourceIndex = (resourceIndex + 1) % VkParams::ResourcesCount;

		if (vkWaitForFences(Vulkan.Device, 1, &currentRenderingResource.Fence, VK_FALSE, 1000000000) != VK_SUCCESS)
		{
			std::cout << "Waiting for fence takes too long" << std::endl;
			return false;
		}
		Camera::Instance()->Update();
		for (auto& model : toRend)
		{
			model->UpdateUniformBuffer();
		}

		vkResetFences(Vulkan.Device, 1, &currentRenderingResource.Fence);

		VkResult res = vkAcquireNextImageKHR(Vulkan.Device, swapChain, UINT64_MAX, currentRenderingResource.ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		switch (res)
		{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			return OnWindowSizeChanged();
		default:
			std::cout << "Problem occurred during swap chain image acquisition" << std::endl;
			return false;
		}

		vkResetCommandBuffer(currentRenderingResource.CommandBuffer, 0);
		if (!PrepareFrame(currentRenderingResource.CommandBuffer, Vulkan.SwapChain.Images[imageIndex], currentRenderingResource.FrameBuffer))
			return false;

		/*VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo =
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			1,
			&currentRenderingResource.ImageAvailableSemaphore,
			&waitDstStageMask,
			1,
			&currentRenderingResource.CommandBuffer,
			1,
			&currentRenderingResource.FinishedRenderingSemaphore
		};*/

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { currentRenderingResource.ImageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &currentRenderingResource.CommandBuffer;

		VkSemaphore signalSemaphores[] = { currentRenderingResource.FinishedRenderingSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(Vulkan.GraphicsQueue.Handle, 1, &submitInfo, currentRenderingResource.Fence) != VK_SUCCESS)
			return false;

		VkPresentInfoKHR presentInfo =
		{
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			nullptr,
			1,
			&currentRenderingResource.FinishedRenderingSemaphore,
			1,
			&swapChain,
			&imageIndex,
			nullptr
		};

		res = vkQueuePresentKHR(Vulkan.PresentQueue.Handle, &presentInfo);

		switch (res)
		{
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_SUBOPTIMAL_KHR:
			return OnWindowSizeChanged();
		default:
			std::cout << "Problem occurred during image presentation" << std::endl;
			return false;
		}

		return true;
	}

	bool StrikeRenderer::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo =
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			0,
			nullptr,
			nullptr,
			1,
			&commandBuffer,
			0,
			nullptr
		};

		vkQueueSubmit(Vulkan.GraphicsQueue.Handle, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(Vulkan.GraphicsQueue.Handle);

		vkFreeCommandBuffers(Vulkan.Device, Vulkan.CommandPool, 1, &commandBuffer);
		return true;
	}

	VkFormat StrikeRenderer::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(Vulkan.PhysicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		throw std::runtime_error("Failed to find supported format");
	}

	VkFormat StrikeRenderer::FindDepthFormat()
	{
		return FindSupportedFormat(
			{
				VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
			},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	uint32_t StrikeRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(Vulkan.PhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}

	bool StrikeRenderer::GetDeviceQueue()
	{
		vkGetDeviceQueue(Vulkan.Device, Vulkan.GraphicsQueue.FamilyIndex, 0, &Vulkan.GraphicsQueue.Handle);
		vkGetDeviceQueue(Vulkan.Device, Vulkan.PresentQueue.FamilyIndex, 0, &Vulkan.PresentQueue.Handle);
		return true;
	}

	uint32_t StrikeRenderer::GetSwapChainNumImages(VkSurfaceCapabilitiesKHR& surfaceCapabilities)
	{
		uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
		if ((surfaceCapabilities.maxImageCount > 0) && (imageCount > surfaceCapabilities.maxImageCount))
			imageCount = surfaceCapabilities.maxImageCount;

		return imageCount;
	}

	VkSurfaceFormatKHR StrikeRenderer::GetSwapChainFormat(std::vector<VkSurfaceFormatKHR>& surfaceFormats)
	{
		//If the only format in list is undefined, any format can be chosen, because there's no format preferred
		if ((surfaceFormats.size() == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
			return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };


		//The type required here is the most widely used, so check if the list contains it
		for (VkSurfaceFormatKHR& surfaceFormat : surfaceFormats)
		{
			if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM)
				return surfaceFormat;
		}

		return surfaceFormats[0];
	}

	VkExtent2D StrikeRenderer::GetSwapChainExtent(VkSurfaceCapabilitiesKHR& surfaceCapabilities)
	{
		//if value is -1, define extent by ourselves (but must fit into min max values!)...
		if (surfaceCapabilities.currentExtent.width = -1)
		{
			VkExtent2D swapChainExtent = { 1024, 768 }; //!
			if (swapChainExtent.width < surfaceCapabilities.minImageExtent.width)
				swapChainExtent.width = surfaceCapabilities.minImageExtent.width;

			if (swapChainExtent.height < surfaceCapabilities.minImageExtent.height)
				swapChainExtent.height = surfaceCapabilities.minImageExtent.height;

			if (swapChainExtent.width > surfaceCapabilities.maxImageExtent.width)
				swapChainExtent.width = surfaceCapabilities.maxImageExtent.width;

			if (swapChainExtent.height > surfaceCapabilities.maxImageExtent.height)
				swapChainExtent.height = surfaceCapabilities.maxImageExtent.height;

			return swapChainExtent;
		}

		//Although the swap chain image size will be equal to app window's size.
		return surfaceCapabilities.currentExtent;
	}

	VkImageUsageFlags StrikeRenderer::GetSwapChainUsageFlags(VkSurfaceCapabilitiesKHR& surfaceCapabilities)
	{
		//Color attachment flag must always be supported
		//Other usage flags must always be checked if they are supported 
		if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		std::cout << "VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported by the swap chain" << std::endl
			<< "Supported swap chain's image usages include:" << std::endl
			<< (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT ? " VK_IMAGE_USAGE_TRANSFER_SRC\n" : "")
			<< (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT ? " VK_IMAGE_USAGE_TRANSFER_DST\n" : "")
			<< (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT ? " VK_IMAGE_USAGE_SAMPLED\n" : "")
			<< (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT ? " VK_IMAGE_USAGE_STORAGE\n" : "")
			<< (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ? " VK_IMAGE_USAGE_COLOR_ATTACHMENT\n" : "")
			<< (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? " VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT\n" : "")
			<< (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT ? " VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT\n" : "")
			<< (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT ? " VK_IMAGE_USAGE_INPUT_ATTACHMENT" : "")
			<< std::endl;

		return static_cast<VkImageUsageFlags>(-1);
	}

	VkSurfaceTransformFlagBitsKHR StrikeRenderer::GetSwapChainTransform(VkSurfaceCapabilitiesKHR& surfaceCapabilities)
	{
		//If current device orientation is different than his default one, images will be transformed.
		//But if current transform is different than specified transform, perfomances will suffer from this.
		//If it's the case, here we just use the same transform as the current one

		if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
			return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		else
			surfaceCapabilities.currentTransform;
	}

	VkPresentModeKHR StrikeRenderer::GetSwapChainPresentMode(std::vector<VkPresentModeKHR>& presentModes)
	{
		//If available, use mailbox present mode. If not, FIFO is always available.
		for (VkPresentModeKHR& presentMode : presentModes)
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return presentMode;

		for (VkPresentModeKHR& presentMode : presentModes)
			if (presentMode == VK_PRESENT_MODE_FIFO_KHR)
				return presentMode;

		std::cout << "FIFO present mode is not supported by the swap chain" << std::endl;
		return static_cast<VkPresentModeKHR>(-1);
	}

	const std::vector<float>& StrikeRenderer::GetVertexData() const
	{

		static const std::vector<float> vertexData =
		{
			 -170.0f, -170.0f, 0.0f, 1.0f,
		  -0.1f, -0.1f,
		  //
		  -170.0f, 170.0f, 0.0f, 1.0f,
		  -0.1f, 1.1f,
		  //
		  170.0f, -170.0f, 0.0f, 1.0f,
		  1.1f, -0.1f,
		  //
		  170.0f, 170.0f, 0.0f, 1.0f,
		  1.1f, 1.1f,
		};

		//static const std::vector<float> vertexData =
		//{
		//	-0.5f,	-0.5f,	1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		//	0.5f,	-0.5f,	0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		//	0.5f,	0.5f,	0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		//	-0.5f,	0.5f,	1.0f, 1.0f, 1.0f, 1.0f, 1.0f
		//};

		return vertexData;
	}

	const std::array<float, 16> StrikeRenderer::GetUniformBufferData() const
	{
		float halfWidth = static_cast<float>(Vulkan.SwapChain.Extent.width) * 0.5f;
		float halfHeight = static_cast<float>(Vulkan.SwapChain.Extent.height) * 0.5f;

		return Tools::GetOrthographicProjectionMatrix(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
	}

	bool StrikeRenderer::HasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	bool StrikeRenderer::InitVulkan()
	{

		if (!LoadVulkan()) return false;

		if (!LoadExportedEntryPoints()) return false;

		if (!LoadGlobalLevelEntryPoints()) return false;

		if (!CreateVkInstance()) return false;

		if (!LoadInstanceLevelEntryPoints()) return false;

		if (!CreatePresentationSurface()) return false;

		if (!CreateDevice()) return false;

		if (!LoadDeviceLevelEntryPoints()) return false;

		if (!GetDeviceQueue()) return false;

		if (!CreateSwapChain()) return false;

		if (!CreateStagingBuffer()) return false;

		if (!CreateRenderPass()) return false;

		if (!CreateCommandBuffers()) return false;

		for (size_t i = 0; i < toRend.size(); ++i)
		{
			if (!toRend[i]->Create()) return false;
			if (!CreatePipelineLayout(toRend[i])) return false;
		}


		if (!CreatePipeline()) return false;
		
		if (!CreateRenderingResources()) return false;

		return true;
	}

	bool StrikeRenderer::LoadVulkan()
	{
		//#ifdef VK_USE_PLATFORM_WIN32_KHR
		VulkanLib = LoadLibraryA("vulkan-1.dll");
		//#endif

		if (VulkanLib == nullptr)
		{
			std::cout << "Could not load Vulkan Library!" << std::endl;
			return false;
		}

		return true;
	}

	bool StrikeRenderer::LoadExportedEntryPoints()
	{
		//#ifdef VK_USE_PLATFORM_WIN32_KHR
#define LoadProcAddress GetProcAddress
//#endif

#define VK_EXPORTED_FUNCTION(func)																\
		if( !(func = (PFN_##func)LoadProcAddress( VulkanLib, #func )) )								\
		{																							\
				std::cout << "Could not load exported function: " << #func << "!" << std::endl;		\
				return false;																		\
		}

#include "Vulkan/VkFuncList.inl"
		return true;
	}

	bool StrikeRenderer::LoadGlobalLevelEntryPoints()
	{
#define VK_GLOBAL_LEVEL_FUNCTION(func)															\
		if (!(func = (PFN_##func)vkGetInstanceProcAddr(nullptr, #func)))							\
		{																							\
			std::cout << "Could not load global level function: " << #func << "!" << std::endl;		\
			return false;																			\
		}

#include "Vulkan/VkFuncList.inl"
		return true;
	}

	bool StrikeRenderer::LoadInstanceLevelEntryPoints()
	{
#define VK_INSTANCE_LEVEL_FUNCTION(func)														\
		if (!(func = (PFN_##func)vkGetInstanceProcAddr(Vulkan.Instance, #func)))					\
		{																							\
			std::cout << "Could not load instance level function: " << #func << "!" << std::endl;	\
			return false;																			\
		}

#include "Vulkan/VkFuncList.inl"
		return true;
	}

	bool StrikeRenderer::LoadDeviceLevelEntryPoints()
	{
#define VK_DEVICE_LEVEL_FUNCTION(func)															\
		if (!(func = (PFN_##func)vkGetDeviceProcAddr(Vulkan.Device, #func)))						\
		{																							\
			std::cout << "Could not load device level function: " << #func << "!" << std::endl;		\
			return false;																			\
		}

#include "Vulkan/VkFuncList.inl"
		return true;
	}

	bool StrikeRenderer::OnWindowSizeChanged()
	{
		if (Vulkan.Device != VK_NULL_HANDLE) {
			vkDeviceWaitIdle(Vulkan.Device);
		}

		if (CreateSwapChain()) {
			if (m_CanRender) {
				if ((Vulkan.Device != VK_NULL_HANDLE) &&
					(Vulkan.StagingBuffer.Handle != VK_NULL_HANDLE)) {
					vkDeviceWaitIdle(Vulkan.Device);
					return CopyUniformBufferData();
				}
				return true;
			}
			return true;
		}

		return false;
	}

	bool StrikeRenderer::PrepareFrame(VkCommandBuffer cmdBuffer, const ImageParameters& imgParams, VkFramebuffer& frameBuffer)
	{
		for (size_t i = 0; i < toRend.size(); ++i)
		{
			if (!CreateFrameBuffers(frameBuffer, imgParams.View, toRend[i]))
				return false;
		}
		

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vkBeginCommandBuffer(cmdBuffer, &beginInfo);

		VkImageSubresourceRange imgSubresourceRange =
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		};

		if (Vulkan.PresentQueue.Handle != Vulkan.GraphicsQueue.Handle)
		{
			VkImageMemoryBarrier barrierFromPresentToDraw =
			{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				nullptr,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				Vulkan.PresentQueue.FamilyIndex,
				Vulkan.GraphicsQueue.FamilyIndex,
				imgParams.Handle,
				imgSubresourceRange
			};
			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromPresentToDraw);
		}

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
		clearValues[1].depthStencil = { 1.0, 0 };


		VkRenderPassBeginInfo renderPassBeginInfo =
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			Vulkan.RenderPass,
			frameBuffer,
			{
				{ 0, 0 },
				Vulkan.SwapChain.Extent,
			},
			static_cast<uint32_t>(clearValues.size()),
			clearValues.data()
		};

		vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Vulkan.GraphicsPipeline);

		VkViewport viewport =
		{
			0.0f,
			0.0f,
			static_cast<float>(Vulkan.SwapChain.Extent.width),
			static_cast<float>(Vulkan.SwapChain.Extent.height),
			0.0f,
			1.0f,
		};

		VkRect2D scissors =
		{
			{ 0, 0 },
			{ Vulkan.SwapChain.Extent.width, Vulkan.SwapChain.Extent.height }
		};

		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissors);

		VkDeviceSize offset = 0;
		size_t size = toRend.size();
		for (size_t i = 0; i < size; ++i)
		{
			Renderable* r = dynamic_cast<Renderable*>(toRend[i]); //TODO: Opti
			if (r->IsRendered())
			{
				VkBuffer buf = toRend[i]->GetMesh().GetVertexBuffer().m_Handle.Handle;
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &buf, &offset);

				vkCmdBindIndexBuffer(cmdBuffer, toRend[i]->GetMesh().GetIndexBuffer().m_Handle.Handle, 0, VK_INDEX_TYPE_UINT32);
				auto test = toRend[i]->GetTexture();
				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Vulkan.PipelineLayout, 0, 1, &toRend[i]->GetTexture().GetDescParams().Handle, 0, nullptr);
				//vkCmdDraw(cmdBuffer, 4, 1, 0, 0);
				vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(toRend[i]->GetMesh().GetIndexBuffer().m_Buffer.size()), 1, 0, 0, 0);
			}
		}


		vkCmdEndRenderPass(cmdBuffer);

		if (Vulkan.GraphicsQueue.Handle != Vulkan.PresentQueue.Handle)
		{
			VkImageMemoryBarrier barrierFromDrawToPresent =
			{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				nullptr,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				Vulkan.GraphicsQueue.FamilyIndex,
				Vulkan.PresentQueue.FamilyIndex,
				imgParams.Handle,
				imgSubresourceRange
			};
			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromDrawToPresent);
		}

		if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS)
		{
			std::cout << "Could not record command buffer" << std::endl;
			return false;
		}
		return true;
	}

	StrikeRenderer::StrikeRenderer(StrikeWindow* strikeWindow) :
		VulkanLib(),
		m_strikeWin(strikeWindow),
		Vulkan()
	{
		if (m_instance != nullptr)
			throw std::runtime_error("Renderer already allocated");

		m_instance = this;
	}

	StrikeRenderer::~StrikeRenderer()
	{

		if (Vulkan.Device != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(Vulkan.Device);


			for (size_t i = 0; i < Vulkan.RenderingResources.size(); ++i)
			{
				if (Vulkan.RenderingResources[i].FrameBuffer != VK_NULL_HANDLE)
					vkDestroyFramebuffer(Vulkan.Device, Vulkan.RenderingResources[i].FrameBuffer, nullptr);

				if (Vulkan.RenderingResources[i].CommandBuffer != VK_NULL_HANDLE)
					vkFreeCommandBuffers(Vulkan.Device, Vulkan.CommandPool, 1, &Vulkan.RenderingResources[i].CommandBuffer);

				if (Vulkan.RenderingResources[i].ImageAvailableSemaphore != VK_NULL_HANDLE)
					vkDestroySemaphore(Vulkan.Device, Vulkan.RenderingResources[i].ImageAvailableSemaphore, nullptr);

				if (Vulkan.RenderingResources[i].FinishedRenderingSemaphore != VK_NULL_HANDLE)
					vkDestroySemaphore(Vulkan.Device, Vulkan.RenderingResources[i].FinishedRenderingSemaphore, nullptr);

				if (Vulkan.RenderingResources[i].FrameBuffer != VK_NULL_HANDLE)
					vkDestroyFence(Vulkan.Device, Vulkan.RenderingResources[i].Fence, nullptr);

			}
			if (Vulkan.CommandPool != VK_NULL_HANDLE)
			{
				vkDestroyCommandPool(Vulkan.Device, Vulkan.CommandPool, nullptr);
				Vulkan.CommandPool = VK_NULL_HANDLE;
			}

			//Model* model = static_cast<Model*>(RenderableResourceController::renderedArray[0]);
			//auto& test = model->GetMesh().GetVertexBuffer().m_Handle.Handle;
			//DestroyBuffer(test);
			//DestroyBuffer(Vulkan.StagingBuffer);


			if (Vulkan.GraphicsPipeline != VK_NULL_HANDLE)
			{
				vkDestroyPipeline(Vulkan.Device, Vulkan.GraphicsPipeline, nullptr);
				Vulkan.GraphicsPipeline = VK_NULL_HANDLE;
			}

			if (Vulkan.PipelineLayout != VK_NULL_HANDLE)
			{
				vkDestroyPipelineLayout(Vulkan.Device, Vulkan.PipelineLayout, nullptr);
				Vulkan.PipelineLayout = VK_NULL_HANDLE;
			}

			if (Vulkan.RenderPass != VK_NULL_HANDLE)
			{
				vkDestroyRenderPass(Vulkan.Device, Vulkan.RenderPass, nullptr);
				Vulkan.RenderPass = VK_NULL_HANDLE;
			}

			for (size_t i = 0; i < toRend.size(); ++i)
			{
				if (toRend[i]->GetTexture().GetDescParams().Pool != VK_NULL_HANDLE)
				{
					vkDestroyDescriptorPool(Vulkan.Device, toRend[i]->GetTexture().GetDescParams().Pool, nullptr);
					toRend[i]->GetTexture().GetDescParams().Pool = VK_NULL_HANDLE;
				}

				if (toRend[i]->GetTexture().GetDescParams().Layout != VK_NULL_HANDLE)
				{
					vkDestroyDescriptorSetLayout(Vulkan.Device, toRend[i]->GetTexture().GetDescParams().Layout, nullptr);
					toRend[i]->GetTexture().GetDescParams().Layout = VK_NULL_HANDLE;
				}
			}
			

			//if (model->GetTexture().GetParams().Sampler != VK_NULL_HANDLE)
			//{
			//	vkDestroySampler(Vulkan.Device, model->GetTexture().GetParams().Sampler, nullptr);
			//	model->GetTexture().GetParams().Sampler = VK_NULL_HANDLE;
			//}

			//if (model->GetTexture().GetParams().View != VK_NULL_HANDLE)
			//{
			//	vkDestroyImageView(Vulkan.Device, model->GetTexture().GetParams().View, nullptr);
			//	model->GetTexture().GetParams().View = VK_NULL_HANDLE;
			//}

			//if (model->GetTexture().GetParams().Handle != VK_NULL_HANDLE)
			//{
			//	vkDestroyImage(Vulkan.Device, model->GetTexture().GetParams().Handle, nullptr);
			//	model->GetTexture().GetParams().Handle = VK_NULL_HANDLE;
			//}

			//if (model->GetTexture().GetParams().Memory != VK_NULL_HANDLE)
			//{
			//	vkFreeMemory(Vulkan.Device, model->GetTexture().GetParams().Memory, nullptr);
			//	model->GetTexture().GetParams().Memory = VK_NULL_HANDLE;
			//}
		}

		if (Vulkan.Device != VK_NULL_HANDLE) {
			vkDeviceWaitIdle(Vulkan.Device);

			for (size_t i = 0; i < Vulkan.SwapChain.Images.size(); ++i) {
				if (Vulkan.SwapChain.Images[i].View != VK_NULL_HANDLE) {
					vkDestroyImageView(Vulkan.Device, Vulkan.SwapChain.Images[i].View, nullptr);
				}
			}

			if (Vulkan.SwapChain.Handle != VK_NULL_HANDLE)
				vkDestroySwapchainKHR(Vulkan.Device, Vulkan.SwapChain.Handle, nullptr);

			vkDestroyDevice(Vulkan.Device, nullptr);
		}

		if (Vulkan.PresentationSurface != VK_NULL_HANDLE)
			vkDestroySurfaceKHR(Vulkan.Instance, Vulkan.PresentationSurface, nullptr);

		if (Vulkan.Instance != VK_NULL_HANDLE)
			vkDestroyInstance(Vulkan.Instance, nullptr);

		if (VulkanLib)
			FreeLibrary(VulkanLib);

	}

	
}