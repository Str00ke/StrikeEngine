#pragma once
//#include "vulkan/vulkan.h"
#include "../Externals/vulkan.h"
#include <vector>
#include "OS.hpp"
#include <iostream>
#include "Tools.hpp"

namespace StrikeEngine
{

	struct ImageParameters
	{
		VkImage Handle;
		VkImageView View;
		VkSampler Sampler;
		VkDeviceMemory Memory;

		ImageParameters() :
			Handle(VK_NULL_HANDLE),
			View(VK_NULL_HANDLE),
			Sampler(VK_NULL_HANDLE),
			Memory(VK_NULL_HANDLE)
		{}
	};

	struct QueueParameters
	{
		VkQueue Handle;
		uint32_t FamilyIndex;

		QueueParameters() :
			Handle(VK_NULL_HANDLE),
			FamilyIndex(0)
		{}
	};

	struct SwapChainParameters
	{
		VkSwapchainKHR Handle;
		VkFormat Format;
		std::vector<ImageParameters> Images;
		VkExtent2D Extent;

		SwapChainParameters() :
			Handle(VK_NULL_HANDLE),
			Format(VK_FORMAT_UNDEFINED),
			Images(),
			Extent()
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
		SwapChainParameters SwapChain;
//============================================================
		VkSemaphore ImageAvailableSemaphore;
		VkSemaphore RenderingFinishedSemaphore;
		uint32_t GraphicsQueueFamilyIndex;
		uint32_t PresentQueueFamilyIndex;
		std::vector<VkCommandBuffer> PresentQueueCmdBuffers;
		VkCommandPool PresentQueueCmdPool;
		VkRenderPass RenderPass;
		std::vector<VkFramebuffer> FrameBuffers;
		VkPipeline GraphicsPipeline;
		VkCommandPool GraphicsQueueCmdPool;
		std::vector<VkCommandBuffer> GraphicsQueueCmdBuffers;

		VkParams() :
			Instance(VK_NULL_HANDLE),
			PhysicalDevice(VK_NULL_HANDLE),
			Device(VK_NULL_HANDLE),
			GraphicsQueue(),
			PresentQueue(),
			PresentationSurface(VK_NULL_HANDLE),
			ImageAvailableSemaphore(VK_NULL_HANDLE),
			RenderingFinishedSemaphore(VK_NULL_HANDLE),
			PresentQueueCmdBuffers(0),
			GraphicsQueueFamilyIndex(0),
			PresentQueueFamilyIndex(0),
			PresentQueueCmdPool(VK_NULL_HANDLE),
			SwapChain(),
			RenderPass(VK_NULL_HANDLE),
			FrameBuffers(),
			GraphicsPipeline(VK_NULL_HANDLE),
			GraphicsQueueCmdPool(VK_NULL_HANDLE),
			GraphicsQueueCmdBuffers()


		{}
	};

	class StrikeRenderer : public OS::Window
	{
	public:
		StrikeRenderer(StrikeWindow*);
		StrikeRenderer(const StrikeRenderer&) = delete;
		StrikeRenderer(StrikeRenderer&&) = delete;
		~StrikeRenderer();

		bool InitVulkan();

		bool OnWindowSizeChanged() override;
//============================================================================================================================

		bool Draw() override;

		bool CreateSwapChain();
		bool CreateCommandBuffers();

		bool CreateSemaphores();
		bool RecordCommandBuffers();
		bool CreateRenderPass();
		bool CreateFrameBuffers();
		bool CreatePipeline();

	private:
		StrikeWindow* m_strikeWin;

		OS::LibraryHandle VulkanLib;
		VkParams Vulkan;

		//virtual void ChildClear() = 0;
		//virtual void ChildOnWindowSizeChanged() = 0;
		
		bool LoadVulkan();
		bool LoadExportedEntryPoints();
		bool LoadGlobalLevelEntryPoints();
		bool LoadInstanceLevelEntryPoints();
		bool LoadDeviceLevelEntryPoints();
		bool CreateVkInstance();
		bool CreateDevice();
		bool CheckPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex);
		bool GetDeviceQueue();
		bool CreatePresentationSurface();
		bool CreateSwapChainImageViews();


		bool CheckExtensionAvailability(const char* extensionName, const std::vector<VkExtensionProperties>& availableExtensions);
		uint32_t GetSwapChainNumImages(VkSurfaceCapabilitiesKHR& surfaceCapabilities);
		VkSurfaceFormatKHR GetSwapChainFormat(std::vector<VkSurfaceFormatKHR>& surfaceFormats);
		VkExtent2D GetSwapChainExtent(VkSurfaceCapabilitiesKHR& surfaceCapabilities);
		VkImageUsageFlags GetSwapChainUsageFlags(VkSurfaceCapabilitiesKHR& surfaceCapabilities);
		VkSurfaceTransformFlagBitsKHR GetSwapChainTransform(VkSurfaceCapabilitiesKHR& surfaceCapabilities);
		VkPresentModeKHR GetSwapChainPresentMode(std::vector<VkPresentModeKHR>& presentModes);
//============================================================================================================================
		Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> CreateShaderModule(const char* filename);
		Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout> CreatePipelineLayout();
		
		bool Clear();

		bool CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPool* pool);
		bool AllocateCommandBuffers(VkCommandPool pool, uint32_t count, VkCommandBuffer* commandBuffers);;
		
	};


}
