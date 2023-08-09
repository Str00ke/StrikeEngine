#pragma once
#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"
//#include "../Externals/vulkan.h"
#include <vector>
#include "OS.hpp"
#include <iostream>

namespace StrikeEngine
{

	struct QueueParameters
	{
		VkQueue Handle;
		uint32_t FamilyIndex;

		QueueParameters() :
			Handle(VK_NULL_HANDLE),
			FamilyIndex(0)
		{}
	};

	struct VkParams
	{
		VkInstance Instance;
		VkPhysicalDevice PhysicalDevice;
		VkDevice Device;
		QueueParameters GraphicsQueue;
		QueueParameters PresentQueue;
		VkSurfaceKHR PresentationSurface;

		VkParams() :
			Instance(VK_NULL_HANDLE),
			PhysicalDevice(VK_NULL_HANDLE),
			Device(VK_NULL_HANDLE),
			PresentationSurface(VK_NULL_HANDLE)
		{}
	};

	class StrikeRenderer : public OS::Window
	{
	public:
		StrikeRenderer();
		StrikeRenderer(const StrikeRenderer&) = delete;
		StrikeRenderer(StrikeRenderer&&) = delete;
		~StrikeRenderer();

		bool InitVulkan(/*OS::WindowParameters params*/);

		bool OnWindowSizeChanged() override;
		bool Draw() override;


	private:
		OS::LibraryHandle VulkanLib;
		OS::WindowParameters Window;
		VkParams Vulkan;

		bool LoadVulkan();
		bool LoadExportedEntryPoints();
		bool LoadGlobalLevelEntryPoints();
		bool LoadInstanceLevelEntryPoints();
		bool LoadDeviceLevelEntryPoints();
		bool CreateVkInstance();
		bool CreateDevice();
		//bool CheckPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex);
		bool CheckPhysicalDeviceProperties(VkPhysicalDevice physical_device, uint32_t& queue_family_index);
		bool CheckExtensionAvailability(const char* extensionName, const std::vector<VkExtensionProperties>& availableExtensions);
		bool GetDeviceQueue();
	};


}
