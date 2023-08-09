#include "StrikeRenderer.hpp"
#include <vector>
#include "VkFunctions.hpp"

namespace StrikeEngine
{

	StrikeRenderer::StrikeRenderer() :
		VulkanLib(),
		Window(),
		Vulkan()
	{}

	StrikeRenderer::~StrikeRenderer()
	{
		if (Vulkan.Device != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(Vulkan.Device);
			vkDestroyDevice(Vulkan.Device, nullptr);
		}

		if (Vulkan.Instance != VK_NULL_HANDLE)
			vkDestroyInstance(Vulkan.Instance, nullptr);

		if (VulkanLib)
		{
			FreeLibrary(VulkanLib);
		}
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

	bool StrikeRenderer::InitVulkan(/*OS::WindowParameters params*/)
	{
		//Window = params;

		if (!LoadVulkan()) return false;

		if (!LoadExportedEntryPoints()) return false;

		if (!LoadGlobalLevelEntryPoints()) return false;

		if (!CreateVkInstance()) return false;

		if (!LoadInstanceLevelEntryPoints()) return false;

		if (!CreateDevice()) return false;

		if (!LoadDeviceLevelEntryPoints()) return false;

		return true;
	}

	bool StrikeRenderer::OnWindowSizeChanged()
	{
		return true;
	}

	bool StrikeRenderer::Draw()
	{
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
		VkApplicationInfo appInfo =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			"Strike Engine Vulkan Renderer",
			VK_MAKE_VERSION(1, 0, 0),
			"Strike Engine",
			VK_MAKE_VERSION(1, 0, 0),
			VK_API_VERSION_1_3
		};

		VkInstanceCreateInfo instanceCreateInfo =
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0,
			&appInfo,
			0,
			nullptr,
			0,
			nullptr
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
			if (CheckPhysicalDeviceProperties(physDevices[i], selectedQueueFamilyIndex))
			//if (CheckPhysicalDeviceProperties(physDevices[i], selectedGraphicsQueueFamilyIndex, selectedPresentQueueFamilyIndex))
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


	//bool StrikeRenderer::CheckPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, uint32_t& selectedGraphicsQueueFamilyIndex, uint32_t& selectedPresentQueueFamilyIndex)
	//{
	//	uint32_t extensionsCount = 0;
	//	if ((vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr) != VK_SUCCESS) || (extensionsCount == 0))
	//	{
	//		std::cout << "Error occurred during physical device " << physicalDevice << " extensions enumeration" << std::endl;
	//		return false;
	//	}

	//	std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
	//	if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, availableExtensions.data()) != VK_SUCCESS)
	//	{
	//		std::cout << "Error occurred during physical device " << physicalDevice << " extensions enumeration" << std::endl;
	//		return false;
	//	}

	//	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	//	for (size_t i = 0; i < deviceExtensions.size(); ++i)
	//	{
	//		if (!CheckExtensionAvailability(deviceExtensions[i], availableExtensions))
	//		{
	//			std::cout << "Physical device " << physicalDevice << " doesn't support extension named\"" << deviceExtensions[i] << "\"!" << std::endl;
	//			return false;
	//		}
	//	}

	//	VkPhysicalDeviceProperties deviceProperties;
	//	VkPhysicalDeviceFeatures deviceFeatures;

	//	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	//	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

	//	uint32_t majorVersion = VK_VERSION_MAJOR(deviceProperties.apiVersion);
	//	uint32_t minorVersion = VK_VERSION_MINOR(deviceProperties.apiVersion);
	//	uint32_t patchVersion = VK_VERSION_PATCH(deviceProperties.apiVersion);

	//	if ((majorVersion < 1) || (deviceProperties.limits.maxImageDimension2D < 4096))
	//	{
	//		std::cout << "Physical device " << physicalDevice << " doesn't support required parameters" << std::endl;
	//		return false;
	//	}

	//	uint32_t queueFamiliesCount = 0;
	//	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
	//	if (queueFamiliesCount == 0)
	//	{
	//		std::cout << "Physical device " << physicalDevice << " doesn't have any queue families" << std::endl;
	//		return false;
	//	}

	//	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamiliesCount);
	//	std::vector<VkBool32> queuePresentSupport(queueFamiliesCount);

	//	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamilyProperties.data());

	//	uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
	//	uint32_t presentQueueFamilyIndex = UINT32_MAX;

	//	for (uint32_t i = 0; i < queueFamiliesCount; ++i)
	//	{
	//		//vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, Vulkan.PresentationSurface, &queuePresentSupport[i]);

	//		if ((queueFamilyProperties[i].queueCount > 0) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
	//		{
	//			//Select first queue that support graphics
	//			if (graphicsQueueFamilyIndex == UINT32_MAX)
	//				graphicsQueueFamilyIndex = i;

	//			//If there is queue that supports both graphics and present, prefer it
	//			if (queuePresentSupport[i])
	//			{
	//				selectedGraphicsQueueFamilyIndex = i;
	//				selectedPresentQueueFamilyIndex = i;
	//				return true;
	//			}
	//		}
	//	}

	//	//We don't have queue that supports both graphics and present so we have to use separate queues
	//	for (uint32_t i = 0; i < queueFamiliesCount; ++i)
	//	{
	//		if (queuePresentSupport[i])
	//		{
	//			presentQueueFamilyIndex = i;
	//			break;
	//		}
	//	}

	//	//If this device doesn't support queues with graphics and present capabilites don't use it
	//	if ((graphicsQueueFamilyIndex == UINT32_MAX) || (presentQueueFamilyIndex == UINT32_MAX))
	//	{
	//		std::cout << "Could not find queue families with required properties on physical device " << physicalDevice << std::endl;
	//		return false;
	//	}

	//	selectedGraphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
	//	selectedPresentQueueFamilyIndex = presentQueueFamilyIndex;
	//	return true;

	//	
	//}



	bool StrikeRenderer::CheckPhysicalDeviceProperties(VkPhysicalDevice physical_device, uint32_t& queue_family_index) {
		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures   device_features;

		vkGetPhysicalDeviceProperties(physical_device, &device_properties);
		vkGetPhysicalDeviceFeatures(physical_device, &device_features);

		uint32_t major_version = VK_VERSION_MAJOR(device_properties.apiVersion);
		uint32_t minor_version = VK_VERSION_MINOR(device_properties.apiVersion);
		uint32_t patch_version = VK_VERSION_PATCH(device_properties.apiVersion);

		if ((major_version < 1) ||
			(device_properties.limits.maxImageDimension2D < 4096)) {
			std::cout << "Physical device " << physical_device << " doesn't support required parameters!" << std::endl;
			return false;
		}

		uint32_t queue_families_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
		if (queue_families_count == 0) {
			std::cout << "Physical device " << physical_device << " doesn't have any queue families!" << std::endl;
			return false;
		}

		std::vector<VkQueueFamilyProperties> queue_family_properties(queue_families_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_family_properties.data());
		for (uint32_t i = 0; i < queue_families_count; ++i) {
			if ((queue_family_properties[i].queueCount > 0) &&
				(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				queue_family_index = i;
				std::cout << "Selected device: " << device_properties.deviceName << std::endl;
				return true;
			}
		}

		std::cout << "Could not find queue family with required properties on physical device " << physical_device << "!" << std::endl;
		return false;
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

	bool StrikeRenderer::GetDeviceQueue()
	{
		vkGetDeviceQueue(Vulkan.Device, Vulkan.GraphicsQueue.FamilyIndex, 0, &Vulkan.GraphicsQueue.Handle);
		vkGetDeviceQueue(Vulkan.Device, Vulkan.PresentQueue.FamilyIndex, 0, &Vulkan.PresentQueue.Handle);
		return true;
	}


}
