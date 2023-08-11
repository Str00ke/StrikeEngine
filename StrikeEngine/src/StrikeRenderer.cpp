#include "StrikeRenderer.hpp"
#include <vector>
#include "VkFunctions.hpp"


namespace StrikeEngine
{

	StrikeRenderer::StrikeRenderer(StrikeWindow* strikeWindow) :
		VulkanLib(),
		m_strikeWin(strikeWindow),
		Vulkan()
	{}

	StrikeRenderer::~StrikeRenderer()
	{
		//--ChildClear
		if (Vulkan.Device != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(Vulkan.Device);

			if ((Vulkan.GraphicsQueueCmdBuffers.size() > 0) && (Vulkan.GraphicsQueueCmdBuffers[0] != VK_NULL_HANDLE))
			{
				vkFreeCommandBuffers(Vulkan.Device, Vulkan.GraphicsQueueCmdPool, static_cast<uint32_t>(Vulkan.GraphicsQueueCmdBuffers.size()), Vulkan.GraphicsQueueCmdBuffers.data());
				Vulkan.GraphicsQueueCmdBuffers.clear();
			}

			if (Vulkan.GraphicsQueueCmdPool != VK_NULL_HANDLE)
			{
				vkDestroyCommandPool(Vulkan.Device, Vulkan.GraphicsQueueCmdPool, nullptr);
				Vulkan.GraphicsQueueCmdBuffers.clear();
			}

			if (Vulkan.GraphicsPipeline != VK_NULL_HANDLE)
			{
				vkDestroyPipeline(Vulkan.Device, Vulkan.GraphicsPipeline, nullptr);
				Vulkan.GraphicsPipeline = VK_NULL_HANDLE;
			}

			if (Vulkan.RenderPass != VK_NULL_HANDLE)
			{
				vkDestroyRenderPass(Vulkan.Device, Vulkan.RenderPass, nullptr);
				Vulkan.RenderPass = VK_NULL_HANDLE;
			}

			for (size_t i = 0; i < Vulkan.FrameBuffers.size(); ++i)
			{
				if (Vulkan.FrameBuffers[i] != VK_NULL_HANDLE)
				{
					vkDestroyFramebuffer(Vulkan.Device, Vulkan.FrameBuffers[i], nullptr);
					Vulkan.FrameBuffers[i] = VK_NULL_HANDLE;
				}
			}
			Vulkan.FrameBuffers.clear();
		}
		//--

		if (Vulkan.Device != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(Vulkan.Device);

			if (Vulkan.ImageAvailableSemaphore != VK_NULL_HANDLE)
				vkDestroySemaphore(Vulkan.Device, Vulkan.ImageAvailableSemaphore, nullptr);
		
			if(Vulkan.RenderingFinishedSemaphore != VK_NULL_HANDLE)
				vkDestroySemaphore(Vulkan.Device, Vulkan.RenderingFinishedSemaphore, nullptr);

			for (size_t i = 0; i < Vulkan.SwapChain.Images.size(); ++i)
				if (Vulkan.SwapChain.Images[i].View != VK_NULL_HANDLE)
					vkDestroyImageView(Vulkan.Device, Vulkan.SwapChain.Images[i].View, nullptr);


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

// 	void StrikeRenderer::ChildClear()
// 	{
// 
// 	}
// 
// 	void StrikeRenderer::ChildOnWindowSizeChanged()
// 	{
// 
// 	}

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


		return true;
	}

	bool StrikeRenderer::OnWindowSizeChanged()
	{
		if (Vulkan.Device != VK_NULL_HANDLE)
			vkDeviceWaitIdle(Vulkan.Device);

		//--ChildClear
		if (Vulkan.Device != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(Vulkan.Device);

			if ((Vulkan.GraphicsQueueCmdBuffers.size() > 0) && (Vulkan.GraphicsQueueCmdBuffers[0] != VK_NULL_HANDLE))
			{
				vkFreeCommandBuffers(Vulkan.Device, Vulkan.GraphicsQueueCmdPool, static_cast<uint32_t>(Vulkan.GraphicsQueueCmdBuffers.size()), Vulkan.GraphicsQueueCmdBuffers.data());
				Vulkan.GraphicsQueueCmdBuffers.clear();
			}

			if (Vulkan.GraphicsQueueCmdPool != VK_NULL_HANDLE)
			{
				vkDestroyCommandPool(Vulkan.Device, Vulkan.GraphicsQueueCmdPool, nullptr);
				Vulkan.GraphicsQueueCmdBuffers.clear();
			}

			if (Vulkan.GraphicsPipeline != VK_NULL_HANDLE)
			{
				vkDestroyPipeline(Vulkan.Device, Vulkan.GraphicsPipeline, nullptr);
				Vulkan.GraphicsPipeline = VK_NULL_HANDLE;
			}

			if (Vulkan.RenderPass != VK_NULL_HANDLE)
			{
				vkDestroyRenderPass(Vulkan.Device, Vulkan.RenderPass, nullptr);
				Vulkan.RenderPass = VK_NULL_HANDLE;
			}

			for (size_t i = 0; i < Vulkan.FrameBuffers.size(); ++i)
			{
				if (Vulkan.FrameBuffers[i] != VK_NULL_HANDLE)
				{
					vkDestroyFramebuffer(Vulkan.Device, Vulkan.FrameBuffers[i], nullptr);
					Vulkan.FrameBuffers[i] = VK_NULL_HANDLE;
				}
			}
			Vulkan.FrameBuffers.clear();
		}
		//--

		if (CreateSwapChain())
		{
			if (m_CanRender)
			{
				//--ChildOnwindowSizeChanged
				if (!CreateRenderPass())
					return false;

				if (!CreateFrameBuffers())
					return false;

				if (!CreatePipeline())
					return false;

				if (!CreateCommandBuffers())
					return false;

				if (!RecordCommandBuffers())
					return false;
				//--
			}
			return true;
		}
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
	
	#include "VkFuncList.inl"
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
	
	#include "VkFuncList.inl"
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
	
	#include "VkFuncList.inl"
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
	
	#include "VkFuncList.inl"
		return true;
	}

	bool StrikeRenderer::CreateVkInstance()
	{
		uint32_t extensionsCount = 0;
		if ((vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr) != VK_SUCCESS) || extensionsCount == 0)
		{
			std::cout << "Error occured during instance extensions enumeration" << std::endl;
			return false;
		}

		std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
		if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, &availableExtensions[0]) != VK_SUCCESS)
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
			VK_API_VERSION
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
			&extensions[0]
		};

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
// 		uint32_t extensionsCount = 0;
// 		if ((vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr) != VK_SUCCESS) || (extensionsCount == 0))
// 		{
// 			std::cout << "Error occurred during physical device " << physicalDevice << " extensions enumeration" << std::endl;
// 			return false;
// 		}
// 
// 		std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
// 		if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, availableExtensions.data()) != VK_SUCCESS)
// 		{
// 			std::cout << "Error occurred during physical device " << physicalDevice << " extensions enumeration" << std::endl;
// 			return false;
// 		}
// 
// 		std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
// 
// 		for (size_t i = 0; i < deviceExtensions.size(); ++i)
// 		{
// 			if (!CheckExtensionAvailability(deviceExtensions[i], availableExtensions))
// 			{
// 				std::cout << "Physical device " << physicalDevice << " doesn't support extension named\"" << deviceExtensions[i] << "\"!" << std::endl;
// 				return false;
// 			}
// 		}
// 
// 		VkPhysicalDeviceProperties deviceProperties;
// 		VkPhysicalDeviceFeatures deviceFeatures;
// 
// 		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
// 		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
// 
// 		uint32_t majorVersion = VK_VERSION_MAJOR(deviceProperties.apiVersion);
// 		uint32_t minorVersion = VK_VERSION_MINOR(deviceProperties.apiVersion);
// 		uint32_t patchVersion = VK_VERSION_PATCH(deviceProperties.apiVersion);
// 
// 		if ((majorVersion < 1) || (deviceProperties.limits.maxImageDimension2D < 4096))
// 		{
// 			std::cout << "Physical device " << physicalDevice << " doesn't support required parameters" << std::endl;
// 			return false;
// 		}
// 
// 		uint32_t queueFamiliesCount = 0;
// 		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
// 		if (queueFamiliesCount == 0)
// 		{
// 			std::cout << "Physical device " << physicalDevice << " doesn't have any queue families" << std::endl;
// 			return false;
// 		}
// 
// 		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamiliesCount);
// 		std::vector<VkBool32> queuePresentSupport(queueFamiliesCount);
// 
// 		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamilyProperties.data());
// 
// 		uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
// 		uint32_t presentQueueFamilyIndex = UINT32_MAX;
// 
// 		for (uint32_t i = 0; i < queueFamiliesCount; ++i)
// 		{
// 			//vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, Vulkan.PresentationSurface, &queuePresentSupport[i]);
// 
// 			if ((queueFamilyProperties[i].queueCount > 0) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
// 			{
// 				//Select first queue that support graphics
// 				if (graphicsQueueFamilyIndex == UINT32_MAX)
// 					graphicsQueueFamilyIndex = i;
// 
// 				//If there is queue that supports both graphics and present, prefer it
// 				if (queuePresentSupport[i])
// 				{
// 					selectedGraphicsQueueFamilyIndex = i;
// 					selectedPresentQueueFamilyIndex = i;
// 					return true;
// 				}
// 			}
// 		}
// 
// 		//We don't have queue that supports both graphics and present so we have to use separate queues
// 		for (uint32_t i = 0; i < queueFamiliesCount; ++i)
// 		{
// 			if (queuePresentSupport[i])
// 			{
// 				presentQueueFamilyIndex = i;
// 				break;
// 			}
// 		}
// 
// 		//If this device doesn't support queues with graphics and present capabilites don't use it
// 		if ((graphicsQueueFamilyIndex == UINT32_MAX) || (presentQueueFamilyIndex == UINT32_MAX))
// 		{
// 			std::cout << "Could not find queue families with required properties on physical device " << physicalDevice << std::endl;
// 			return false;
// 		}
// 
// 		selectedGraphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
// 		selectedPresentQueueFamilyIndex = presentQueueFamilyIndex;
// 		return true;

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

	bool StrikeRenderer::GetDeviceQueue()
	{
		vkGetDeviceQueue(Vulkan.Device, Vulkan.GraphicsQueue.FamilyIndex, 0, &Vulkan.GraphicsQueue.Handle);
		vkGetDeviceQueue(Vulkan.Device, Vulkan.PresentQueue.FamilyIndex, 0, &Vulkan.PresentQueue.Handle);
		return true;
	}

	bool StrikeRenderer::Clear()
	{
		if (Vulkan.Device != VK_NULL_HANDLE)
			vkDeviceWaitIdle(Vulkan.Device);

		if ((Vulkan.PresentQueueCmdBuffers.size() > 0) && Vulkan.PresentQueueCmdBuffers[0] != VK_NULL_HANDLE)
		{
			vkFreeCommandBuffers(Vulkan.Device, Vulkan.PresentQueueCmdPool, static_cast<uint32_t>(Vulkan.PresentQueueCmdBuffers.size()), &Vulkan.PresentQueueCmdBuffers[0]);
			Vulkan.PresentQueueCmdBuffers.clear();
		}

		if (Vulkan.PresentQueueCmdPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(Vulkan.Device, Vulkan.PresentQueueCmdPool, nullptr);
			Vulkan.PresentQueueCmdPool = VK_NULL_HANDLE;
		}

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
	
	bool StrikeRenderer::CreateRenderPass()
	{
		VkAttachmentDescription attachmentDesc =
		{
			0,
			Vulkan.SwapChain.Format,
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference colorAttachmentReferences[] =
		{
			{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
		};

		VkSubpassDescription subpassDesc =
		{
			0,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			0,
			nullptr,
			1,
			colorAttachmentReferences,
			nullptr,
			nullptr,
			0,
			nullptr
		};

		VkRenderPassCreateInfo renderPassCreateInfo =
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			nullptr,
			0,
			1,
			&attachmentDesc,
			1,
			&subpassDesc,
			0,
			nullptr
		};

		if (vkCreateRenderPass(Vulkan.Device, &renderPassCreateInfo, nullptr, &Vulkan.RenderPass) != VK_SUCCESS)
		{
			std::cout << "Could not create render pass" << std::endl;
			return false;
		}

		return true;
		
	}

	bool StrikeRenderer::CreateFrameBuffers()
	{
		const std::vector<ImageParameters>& swapChainImages = Vulkan.SwapChain.Images;
		Vulkan.FrameBuffers.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); ++i)
		{
			VkFramebufferCreateInfo frameBufferCreateInfo =
			{
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				nullptr,
				0,
				Vulkan.RenderPass,
				1,
				&swapChainImages[i].View,
				300,
				300,
				1
			};

			if (vkCreateFramebuffer(Vulkan.Device, &frameBufferCreateInfo, nullptr, &Vulkan.FrameBuffers[i]) != VK_SUCCESS)
			{
				std::cout << "Could not create a framebuffer" << std::endl;
				return false;
			}
		}

		return true;
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

	Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout> StrikeRenderer::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo layoutCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			0,
			nullptr,
			0,
			nullptr
		};

		VkPipelineLayout pipelineLayout;
		if (vkCreatePipelineLayout(Vulkan.Device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			std::cout << "Could not create pipeline layout" << std::endl;
			return Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>();
		}

		return Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>(pipelineLayout, vkDestroyPipelineLayout, Vulkan.Device);
	}

	bool StrikeRenderer::CreatePipeline()
	{
		Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> vertexShaderModule = CreateShaderModule("../Data/vert.spv");
		Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> fragmentShaderModule = CreateShaderModule("../Data/frag.spv");

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


		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			0,
			nullptr,
			0,
			nullptr
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE
		};

		VkViewport viewport =
		{
			0.0f,
			0.0f,
			300.0f,
			300.0f,
			0.0f,
			1.0f,
		};

		VkRect2D scissor =
		{
			{ 0, 0 },
			{ 300, 300 }
		};

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			&viewport,
			1,
			&scissor
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
			{ 0.0, 0.0, 0.0, 0.0 }
		};
		
		Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout> pipelineLayout = CreatePipelineLayout();
		if (!pipelineLayout)
			return false;

		VkGraphicsPipelineCreateInfo pipelineCreateInfo =
		{
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32_t>(shaderStageCreateInfo.size()),
			&shaderStageCreateInfo[0],
			&vertexInputStateCreateInfo,
			&inputAssemblyStateCreateInfo,
			nullptr,
			&viewportStateCreateInfo,
			&rasterizationStateCreateInfo,
			&multisampleStageCreateInfo,
			nullptr,
			&colorBlendStateCreateInfo,
			nullptr,
			pipelineLayout.Get(),
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

	bool StrikeRenderer::CreateSemaphores()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo =
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			nullptr,
			0
		};

		if ((vkCreateSemaphore(Vulkan.Device, &semaphoreCreateInfo, nullptr, &Vulkan.ImageAvailableSemaphore) != VK_SUCCESS) ||
			(vkCreateSemaphore(Vulkan.Device, &semaphoreCreateInfo, nullptr, &Vulkan.RenderingFinishedSemaphore) != VK_SUCCESS))
		{
			std::cout << "Could not create semaphores" << std::endl;
			return false;
		}
		return true;
	}

	bool StrikeRenderer::CreateCommandBuffers()
	{
		if (!CreateCommandPool(Vulkan.GraphicsQueue.FamilyIndex, &Vulkan.GraphicsQueueCmdPool))
		{
			std::cout << "Could not create command pool" << std::endl;
			return false;
		}

		uint32_t imageCount = static_cast<uint32_t>(Vulkan.SwapChain.Images.size());
		Vulkan.GraphicsQueueCmdBuffers.resize(imageCount, VK_NULL_HANDLE);

		if (!AllocateCommandBuffers(Vulkan.GraphicsQueueCmdPool, imageCount, &Vulkan.GraphicsQueueCmdBuffers[0]))
		{
			std::cout << "Could not allocate command buffers" << std::endl;
			return false;
		}

		return true;
		/* OLD
		VkCommandPoolCreateInfo cmdPoolCreateInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			nullptr,
			0,
			Vulkan.PresentQueueFamilyIndex
		};

		if (vkCreateCommandPool(Vulkan.Device, &cmdPoolCreateInfo, nullptr, &Vulkan.PresentQueueCmdPool) != VK_SUCCESS)
		{
			std::cout << "Could not create a command pool" << std::endl;
			return false;
		}

		uint32_t imageCount = 0;
		if ((vkGetSwapchainImagesKHR(Vulkan.Device, Vulkan.SwapChain.Handle, &imageCount, nullptr) != VK_SUCCESS) || (imageCount == 0))
		{
			std::cout << "Could not get the number of swap chain images" << std::endl;
			return false;
		}

		Vulkan.PresentQueueCmdBuffers.resize(imageCount);

		VkCommandBufferAllocateInfo cmdBufferAllocateInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			Vulkan.PresentQueueCmdPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			imageCount
		};

		if (vkAllocateCommandBuffers(Vulkan.Device, &cmdBufferAllocateInfo, Vulkan.PresentQueueCmdBuffers.data()) != VK_SUCCESS)
		{
			std::cout << "Could not allocate command buffers" << std::endl;
			return false;
		}

		if (!RecordCommandBuffers())
		{
			std::cout << "Could not record command buffers" << std::endl;
			return false;
		}
		return true;
		*/
	}

	bool StrikeRenderer::CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPool* pool)
	{
		VkCommandPoolCreateInfo cmdPoolCreateInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			nullptr,
			0,
			queueFamilyIndex
		};

		if (vkCreateCommandPool(Vulkan.Device, &cmdPoolCreateInfo, nullptr, pool) != VK_SUCCESS)
			return false;

		return true;
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
			return false;

		return true;
	}

	bool StrikeRenderer::RecordCommandBuffers()
	{
		VkCommandBufferBeginInfo graphicsCmdBufferBeginInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			nullptr
		};

		VkImageSubresourceRange imgSubresourceRange =
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		};

		VkClearValue clearValue =
		{
			{ 1.0f, 0.8f, 0.4f, 0.0f },
		};

		const std::vector<ImageParameters>& swapChainImages = Vulkan.SwapChain.Images;

		for (size_t i = 0; i < Vulkan.GraphicsQueueCmdBuffers.size(); ++i)
		{
			vkBeginCommandBuffer(Vulkan.GraphicsQueueCmdBuffers[i], &graphicsCmdBufferBeginInfo);

			if (Vulkan.PresentQueue.Handle != Vulkan.GraphicsQueue.Handle)
			{
				VkImageMemoryBarrier barrierFromPresentToDraw =
				{
					VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
					nullptr,
					VK_ACCESS_MEMORY_READ_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					Vulkan.PresentQueue.FamilyIndex,
					Vulkan.GraphicsQueue.FamilyIndex,
					swapChainImages[i].Handle,
					imgSubresourceRange
				};
				vkCmdPipelineBarrier(Vulkan.GraphicsQueueCmdBuffers[i], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromPresentToDraw);

			}

			VkRenderPassBeginInfo renderPassBeginInfo =
			{
				VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				nullptr,
				Vulkan.RenderPass,
				Vulkan.FrameBuffers[i],
				{
					{ 0, 0 },
					{ 300, 300 }
				},
				1,
				&clearValue
			};

			vkCmdBeginRenderPass(Vulkan.GraphicsQueueCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(Vulkan.GraphicsQueueCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, Vulkan.GraphicsPipeline);

			vkCmdDraw(Vulkan.GraphicsQueueCmdBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(Vulkan.GraphicsQueueCmdBuffers[i]);

			if (Vulkan.GraphicsQueue.Handle != Vulkan.PresentQueue.Handle)
			{
				VkImageMemoryBarrier barrierFromDrawToPresent =
				{
					VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
					nullptr,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_MEMORY_READ_BIT,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					Vulkan.GraphicsQueue.FamilyIndex,
					Vulkan.PresentQueue.FamilyIndex,
					swapChainImages[i].Handle,
					imgSubresourceRange
				};
				vkCmdPipelineBarrier(Vulkan.GraphicsQueueCmdBuffers[i], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromDrawToPresent);

			}

			if (vkEndCommandBuffer(Vulkan.GraphicsQueueCmdBuffers[i]) != VK_SUCCESS)
			{
				std::cout << "Could not record command buffer" << std::endl;
				return false;
			}

		}
		return true;


		/* OLD
		uint32_t imageCount = static_cast<uint32_t>(Vulkan.PresentQueueCmdBuffers.size());

		std::vector<VkImage> swapChainImages(imageCount);
		if (vkGetSwapchainImagesKHR(Vulkan.Device, Vulkan.SwapChain.Handle, &imageCount, swapChainImages.data()) != VK_SUCCESS)
		{
			std::cout << "Could not get swap chain images" << std::endl;
			return false;
		}

		VkCommandBufferBeginInfo cmdBufferBeginInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			nullptr
		};

		VkClearColorValue clearColor =
		{
			{ 1.0f, 0.8f, 0.4f, 0.0f }
		};

		VkImageSubresourceRange imageSubresourceRange =
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		};

		for (uint32_t i = 0; i < imageCount; ++i)
		{
			VkImageMemoryBarrier barrierFromPresentToClear =
			{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				nullptr,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_QUEUE_FAMILY_IGNORED,
				VK_QUEUE_FAMILY_IGNORED,
				swapChainImages[i],
				imageSubresourceRange
			};

			VkImageMemoryBarrier barrierFromClearToPresent =
			{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				nullptr,
				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				VK_QUEUE_FAMILY_IGNORED,
				VK_QUEUE_FAMILY_IGNORED,
				swapChainImages[i],
				imageSubresourceRange
			};

			vkBeginCommandBuffer(Vulkan.PresentQueueCmdBuffers[i], &cmdBufferBeginInfo);
			vkCmdPipelineBarrier(Vulkan.PresentQueueCmdBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromPresentToClear);

			vkCmdClearColorImage(Vulkan.PresentQueueCmdBuffers[i], swapChainImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &imageSubresourceRange);

			vkCmdPipelineBarrier(Vulkan.PresentQueueCmdBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromClearToPresent);
			if (vkEndCommandBuffer(Vulkan.PresentQueueCmdBuffers[i]) != VK_SUCCESS)
			{
				std::cout << "Could not record command buffers" << std::endl;
				return false;
			}
		}
		return true;*/
	}

	bool StrikeRenderer::Draw()
	{
		VkSemaphore imageAvailableSemaphore = Vulkan.ImageAvailableSemaphore;
		VkSemaphore renderingFinishedSemaphore = Vulkan.RenderingFinishedSemaphore;
		VkSwapchainKHR swapchain = Vulkan.SwapChain.Handle;
		uint32_t imageIndex;

		VkResult res = vkAcquireNextImageKHR(Vulkan.Device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
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

		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
		VkSubmitInfo submitInfo =
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			1,
			&Vulkan.ImageAvailableSemaphore,
			&waitDstStageMask,
			1,
			&Vulkan.GraphicsQueueCmdBuffers[imageIndex],
			1,
			&Vulkan.RenderingFinishedSemaphore
		};

		if (vkQueueSubmit(Vulkan.GraphicsQueue.Handle, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
			return false;

		VkPresentInfoKHR presentInfo =
		{
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			nullptr,
			1,
			&renderingFinishedSemaphore,
			1,
			&swapchain,
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

		/* OLD
		uint32_t imageIndex;
		VkResult res = vkAcquireNextImageKHR(Vulkan.Device, Vulkan.SwapChain.Handle, UINT64_MAX, Vulkan.ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		switch (res)
		{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			return OnWindowSizeChanged();
		default:
			std::cout << "Problem occured during swap chain image acquisition" << std::endl;
			return false;
		}

		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
		VkSubmitInfo submitInfo =
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			1,
			&Vulkan.ImageAvailableSemaphore,
			&waitDstStageMask,
			1,
			&Vulkan.PresentQueueCmdBuffers[imageIndex],
			1,
			&Vulkan.RenderingFinishedSemaphore
		};

		if (vkQueueSubmit(Vulkan.PresentQueue.Handle, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
			return false;

		VkPresentInfoKHR presentInfo =
		{
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			nullptr,
			1,
			&Vulkan.RenderingFinishedSemaphore,
			1,
			&Vulkan.SwapChain.Handle,
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
		*/
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
	



}