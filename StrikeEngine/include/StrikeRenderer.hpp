#pragma once
#include "vulkan/vulkan.h"
//#include "../Externals/vulkan.h"
#include <vector>
#include "OS.hpp"
#include <string>
#include <iostream>
#include "Tools.hpp"
#include "Matrix4X4.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/hash.hpp"



namespace StrikeEngine
{

	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif


	//TMP
	const std::string MODEL_PATH = "../Data/viking_room.obj";
	const std::string TEXTURE_PATH = "../Data/viking_room.png";

	struct VertexData
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		bool operator==(const VertexData& other) const
		{
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

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

	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
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

	struct BufferParameters
	{
		VkBuffer Handle;
		VkDeviceMemory Memory;
		uint32_t Size;

		BufferParameters() :
			Handle(VK_NULL_HANDLE),
			Memory(VK_NULL_HANDLE),
			Size(0)
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

	struct RenderingResourceData
	{
		VkFramebuffer FrameBuffer;
		VkCommandBuffer CommandBuffer;
		VkSemaphore ImageAvailableSemaphore;
		VkSemaphore FinishedRenderingSemaphore;
		VkFence Fence;

		RenderingResourceData() :
			FrameBuffer(VK_NULL_HANDLE),
			CommandBuffer(VK_NULL_HANDLE),
			ImageAvailableSemaphore(VK_NULL_HANDLE),
			FinishedRenderingSemaphore(VK_NULL_HANDLE),
			Fence(VK_NULL_HANDLE)
		{}
	};

	//TMP
	struct ModelData
	{
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		BufferParameters VertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		BufferParameters indexBuffer;
		VkDeviceMemory indexBufferMemory;

		ModelData() :
			vertices(),
			indices(),
			VertexBuffer(),
			vertexBufferMemory(VK_NULL_HANDLE),
			indexBuffer(),
			indexBufferMemory(VK_NULL_HANDLE)
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
		uint32_t GraphicsQueueFamilyIndex;
		uint32_t PresentQueueFamilyIndex;
		std::vector<VkCommandBuffer> PresentQueueCmdBuffers;
		VkRenderPass RenderPass;
		std::vector<VkFramebuffer> FrameBuffers;
		VkPipeline GraphicsPipeline;
		std::vector<VkCommandBuffer> GraphicsQueueCmdBuffers;
		BufferParameters VertexBuffer;
		BufferParameters StagingBuffer;
		std::vector<RenderingResourceData> RenderingResources;
		VkCommandPool CommandPool;
		ImageParameters Image;
		ImageParameters DepthImage;
		DescriptorSetParameters DescriptorSet;
		VkPipelineLayout PipelineLayout;
		BufferParameters UniformBuffer;
		ModelData Model;
		void* UniformBufferMapped;

		static const size_t ResourcesCount = 3;

		VkParams() :
			Instance(VK_NULL_HANDLE),
			PhysicalDevice(VK_NULL_HANDLE),
			Device(VK_NULL_HANDLE),
			GraphicsQueue(),
			PresentQueue(),
			PresentationSurface(VK_NULL_HANDLE),
			PresentQueueCmdBuffers(0),
			GraphicsQueueFamilyIndex(0),
			PresentQueueFamilyIndex(0),
			SwapChain(),
			RenderPass(VK_NULL_HANDLE),
			FrameBuffers(),
			GraphicsPipeline(VK_NULL_HANDLE),
			GraphicsQueueCmdBuffers(),
			CommandPool(VK_NULL_HANDLE),
			RenderingResources(ResourcesCount),
			VertexBuffer(),
			StagingBuffer(),
			Image(),
			DepthImage(),
			PipelineLayout(),
			UniformBuffer(),
			Model(),
			UniformBufferMapped(nullptr)

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

		bool CreateSemaphore();
		bool CreateRenderPass();
		bool CreateFrameBuffers(VkFramebuffer& frameBuffer, VkImageView imageView);
		bool CreatePipeline();
		bool CreateVertexBuffer();
		bool CreateIndexBuffer();
		bool CreateStagingBuffer();
		bool CopyVertexData();
		bool CreateRenderingResources();
		bool CreateTexture();
		bool CreateDescriptorSetLayout();
		bool CreateDescriptorPool();
		bool AllocateDescriptorSet();
		bool UpdateDescriptorSet();
		bool CreatePipelineLayout();
		bool CreateUniformBuffer();
		bool CreateObject();
		bool CreateDepthResources();

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
	

		bool CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPool* pool);
		bool AllocateCommandBuffers(VkCommandPool pool, uint32_t count, VkCommandBuffer* commandBuffers);;
		bool AllocateBufferMemory(VkBuffer buffer, VkMemoryPropertyFlags property, VkDeviceMemory* memory);
		bool CreateFences();
		bool PrepareFrame(VkCommandBuffer cmdBuffer, const ImageParameters& imgParams, VkFramebuffer& frameBuffer);
	
		bool CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryProperty, BufferParameters& buffer);
		void DestroyBuffer(BufferParameters& buffer);
		const std::vector<float>& GetVertexData() const;

		

		bool CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image);
		bool AllocateImageMemory(VkImage image, VkMemoryPropertyFlagBits property, VkDeviceMemory* memory);
		bool CreateImageView(ImageParameters& imgParams, VkFormat format, VkImageAspectFlags aspectFlags);
		bool CreateSampler(VkSampler* sampler);
		bool CopyTextureData(char* textureData, uint32_t dataSize, uint32_t width, uint32_t height);

		bool CopyUniformBufferData();
		const std::array<float, 16> GetUniformBufferData() const;

		bool UpdateUniformBuffer();
		bool LoadModel(ModelData& modelData);

		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat FindDepthFormat();
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		bool HasStencilComponent(VkFormat format);

		VkCommandBuffer BeginSingleTimeCommands();
		bool EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		bool CheckValidationLayerSupport();
		bool TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		bool CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	};


}

