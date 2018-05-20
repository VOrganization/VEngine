#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <string>
#include <vector>

#define VK_ALLOCK nullptr

#ifdef _DEBUG
void _VkCheckError(VkResult r, const char* filename, const char* function, uint32_t line);
void _VkDisplayError(std::string msg, const char* filename, const char* function, uint32_t line);
#define VkCheck(r) (_VkCheckError(r, __FILE__, __FUNCTION__, __LINE__))
#define VkDisplayError(r) (_VkDisplayError(r, __FILE__, __FUNCTION__, __LINE__))
#else
void _VkCheckError(VkResult r, const char* function);
void _VkDisplayError(std::string msg);
#define VkCheck(r) (_VkCheckError(r, __FUNCTION__))
#define VkDisplayError(r) (_VkDisplayError(r))
#endif // _DEBUG

class GFContext {

	VkInstance instance = VK_NULL_HANDLE;
	std::vector<const char*> instanceLayer;
	std::vector<const char*> instanceExtension;

	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDevice gpu = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties gpuProperties = {};
	VkPhysicalDeviceMemoryProperties gpuMemoryProperties = {};
	std::vector<const char*> deviceLayer;
	std::vector<const char*> deviceExtension;

	uint32_t queueFamilyIndex = 0;
	VkQueue queue = VK_NULL_HANDLE;

	VkCommandPool commandPool = VK_NULL_HANDLE;

	VkRenderPass renderPass = VK_NULL_HANDLE;

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkSurfaceFormatKHR surfaceFormat = {};
	VkExtent2D surfaceExtent = {};
	VkSurfaceCapabilitiesKHR surfaceCapabilities = {};

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkFormat swapChainImageFormat = {};
	VkFence swapchainImageAvailable = VK_NULL_HANDLE;
	uint32_t swapchainImageID = 0;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkImage depthImage = VK_NULL_HANDLE;
	VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
	VkImageView depthImageView = VK_NULL_HANDLE;



#ifdef _DEBUG
	PFN_vkCreateDebugReportCallbackEXT VkCreateDebugCallBack;
	PFN_vkDestroyDebugReportCallbackEXT VkDestroyDebugCallBack;
	VkDebugReportCallbackEXT debugRaport = VK_NULL_HANDLE;

	void InitDebug();
	void DeInitDebug();
#endif

	void InitInstance();
	void DeInitInstance();

	void InitDevice();
	void DeInitDevice();

	void InitCommandPool();
	void DeInitCommandPool();

	void InitWindow();
	void DeInitWindow();

	void InitSwapChain();
	void DeInitSwapChain();

	void InitRenderPass();
	void DeInitRenderPass();

	void InitDepthStencil();
	void DeInitDepthStencil();

	void InitFrameBuffer();
	void DeInitFrameBuffer();

public:
	GFContext();
	virtual ~GFContext();

	uint32_t apiVersion = VK_MAKE_VERSION(1, 0, 3);

	std::string appName = "";
	uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);

	std::string engineName = "";
	uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);

	GLFWwindow* window = nullptr;
	uint32_t swapchainImageCount = 2;

	static void onWindowResize(GLFWwindow* window, int width, int height);
	void ResizeWindow();

	std::vector<VkCommandBuffer> commandBuffers;

	void InitContext();
	void BeginRender();
	void EndRender(std::vector<VkSemaphore> waitSemaphores);

	VkFramebuffer GetActiveFrameBuffer();

	const VkDevice GetDevice();
	const VkPhysicalDevice GetGpu();
	const VkPhysicalDeviceProperties GetGpuProperties();
	const VkPhysicalDeviceMemoryProperties GetGpuMemoryProperties();

	const uint32_t GetQueueFamilyIndex();
	const VkQueue GetQueue();

	const VkCommandPool GetCommandPool();

	const VkRenderPass GetRenderPass();

	const VkSurfaceKHR GetSurface();
	const VkSurfaceFormatKHR sGetSurfaceFormat();
	const VkExtent2D GetSurfaceExtent();
	const VkSurfaceCapabilitiesKHR GetSurfaceCapabilities();

	const VkPresentModeKHR GetPresentMode();

	const VkSwapchainKHR GetSwapchain();
	const VkFormat GetSwapChainImageFormat();


	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription;

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
	VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineMultisampleStateCreateInfo multisampling = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FALSE,
		0,
		nullptr,
		VK_FALSE,
		VK_FALSE
	};

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();

	VkGraphicsPipelineCreateInfo CreatePipelineInfo();

};

