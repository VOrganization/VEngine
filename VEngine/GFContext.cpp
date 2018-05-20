#include "GFContext.h"

#include <sstream>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif // defined(WIN32)

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

#ifdef _DEBUG

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallBack(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT obj_type,
	uint64_t src_obj,
	size_t location,
	int32_t msg_code,
	const char* layer_prefix,
	const char* msg,
	void* user_data
) {
	std::stringstream stream;
	stream << "VK ";
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
		stream << "INFO ";
	}
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
		stream << "WARNING ";
	}
	if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
		stream << "PERFORMANCE WARNING ";
	}
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		stream << "ERROR ";
	}
	if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
		stream << "DEBUG ";
	}
	stream << "@[" << layer_prefix << "] " << msg << "\n";
	std::cout << stream.str();

#ifdef _WIN32
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		MessageBox(NULL, stream.str().c_str(), "Vulkan Error", 0);
		std::exit(-1);
	}
#endif // _WIN32
	return false;
}

void GFContext::InitDebug() {
	this->VkCreateDebugCallBack = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(this->instance, "vkCreateDebugReportCallbackEXT");
	this->VkDestroyDebugCallBack = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(this->instance, "vkDestroyDebugReportCallbackEXT");
	if (this->VkCreateDebugCallBack == VK_NULL_HANDLE || this->VkDestroyDebugCallBack == VK_NULL_HANDLE) {
		VkDisplayError("Can't to create debug function");
	}

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.pfnCallback = VulkanDebugCallBack;
	createInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
		VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_ERROR_BIT_EXT |
		VK_DEBUG_REPORT_DEBUG_BIT_EXT | 0;

	VkCheck(this->VkCreateDebugCallBack(this->instance, &createInfo, VK_ALLOCK, &this->debugRaport));
}

void GFContext::DeInitDebug() {
	this->VkDestroyDebugCallBack(this->instance, this->debugRaport, VK_ALLOCK);
}

#endif // _DEBUG

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::InitInstance() {
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = this->appName.c_str();
	appInfo.applicationVersion = this->appVersion;
	appInfo.pEngineName = this->engineName.c_str();
	appInfo.engineVersion = this->engineVersion;
	appInfo.apiVersion = this->apiVersion;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = this->instanceLayer.size();
	createInfo.ppEnabledLayerNames = this->instanceLayer.data();
	createInfo.enabledExtensionCount = this->instanceExtension.size();
	createInfo.ppEnabledExtensionNames = this->instanceExtension.data();

	VkCheck(vkCreateInstance(&createInfo, VK_ALLOCK, &instance));
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::DeInitInstance() {
	vkDestroyInstance(this->instance, VK_ALLOCK);
	this->instance = VK_NULL_HANDLE;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::InitDevice() {

	{
		uint32_t gpu_count = 0;
		vkEnumeratePhysicalDevices(this->instance, &gpu_count, nullptr);
		vector<VkPhysicalDevice> gpus(gpu_count);
		vkEnumeratePhysicalDevices(this->instance, &gpu_count, gpus.data());
		this->gpu = gpus[0];
		vkGetPhysicalDeviceProperties(this->gpu, &this->gpuProperties);
		vkGetPhysicalDeviceMemoryProperties(this->gpu, &this->gpuMemoryProperties);
	}

	{
		uint32_t family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(this->gpu, &family_count, nullptr);
		vector<VkQueueFamilyProperties> family_list(family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(this->gpu, &family_count, family_list.data());

		bool found = false;
		for (uint32_t i = 0; i < family_count; i++) {
			if (family_list[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				found = true;
				this->queueFamilyIndex = i;
				break;
			}
		}
		if (!found) {
			VkDisplayError("Not found queue family index");
		}
	}

	float queuePriorities[] = { 1.0f };
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.queueFamilyIndex = this->queueFamilyIndex;
	queueCreateInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.enabledLayerCount = this->deviceLayer.size();
	createInfo.ppEnabledLayerNames = this->deviceLayer.data();
	createInfo.enabledExtensionCount = this->deviceExtension.size();
	createInfo.ppEnabledExtensionNames = this->deviceExtension.data();

	VkCheck(vkCreateDevice(this->gpu, &createInfo, VK_ALLOCK, &this->device));

	vkGetDeviceQueue(this->device, this->queueFamilyIndex, 0, &this->queue);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::DeInitDevice() {
	vkDestroyDevice(this->device, VK_ALLOCK);
	this->device = VK_NULL_HANDLE;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::InitCommandPool() {
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = this->queueFamilyIndex;
	VkCheck(vkCreateCommandPool(this->device, &poolInfo, VK_ALLOCK, &this->commandPool));
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::DeInitCommandPool() {
	vkDestroyCommandPool(this->device, this->commandPool, VK_ALLOCK);
	this->commandPool = VK_NULL_HANDLE;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::InitWindow() {
	if (window == nullptr) {
		return;
	}

	glfwSetWindowUserPointer(this->window, this);
	glfwSetWindowSizeCallback(this->window, GFContext::onWindowResize);

	VkCheck(glfwCreateWindowSurface(this->instance, this->window, VK_ALLOCK, &this->surface));

	VkBool32 support;
	VkCheck(vkGetPhysicalDeviceSurfaceSupportKHR(this->gpu, this->queueFamilyIndex, this->surface, &support));
	if (!support) {
		VkDisplayError("WSI not supported");
	}

	int w, h;
	glfwGetWindowSize(this->window, &w, &h);
	this->surfaceExtent.width = w;
	this->surfaceExtent.height = h;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->gpu, this->surface, &this->surfaceCapabilities);
	{
		uint32_t format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(this->gpu, this->surface, &format_count, nullptr);
		if (format_count == 0) {
			VkDisplayError("Surface formats missing");
		}
		vector<VkSurfaceFormatKHR> formats(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(this->gpu, this->surface, &format_count, formats.data());
		if (formats[0].format == VK_FORMAT_UNDEFINED) {
			this->surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
			this->surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		}
		else {
			this->surfaceFormat = formats[0];
		}
	}

	{
		uint32_t present_mode_count = 0;
		VkCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, nullptr));
		vector<VkPresentModeKHR> present_mode_list(present_mode_count);
		VkCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, present_mode_list.data()));
		for (auto m : present_mode_list) {
			if (m == VK_PRESENT_MODE_MAILBOX_KHR) presentMode = m;
		}
	}


	this->InitSwapChain();
	this->InitRenderPass();
	this->InitDepthStencil();
	this->InitFrameBuffer();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::DeInitWindow() {
	if (window == nullptr) {
		return;
	}

	this->DeInitFrameBuffer();
	this->DeInitDepthStencil();
	this->DeInitRenderPass();
	this->DeInitSwapChain();

	vkDestroySurfaceKHR(this->instance, this->surface, VK_ALLOCK);
	this->surface = VK_NULL_HANDLE;
	glfwDestroyWindow(this->window);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::InitSwapChain() {
	this->swapChainImageFormat = surfaceFormat.format;

	VkSwapchainCreateInfoKHR SwapChainCreateInfo = {};
	SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapChainCreateInfo.surface = surface;
	SwapChainCreateInfo.minImageCount = this->swapchainImageCount;
	SwapChainCreateInfo.imageFormat = this->surfaceFormat.format;
	SwapChainCreateInfo.imageColorSpace = this->surfaceFormat.colorSpace;
	SwapChainCreateInfo.imageExtent = this->surfaceExtent;
	SwapChainCreateInfo.imageArrayLayers = 1;
	SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	SwapChainCreateInfo.queueFamilyIndexCount = 1;
	SwapChainCreateInfo.pQueueFamilyIndices = &this->queueFamilyIndex;
	SwapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	SwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapChainCreateInfo.presentMode = presentMode;
	SwapChainCreateInfo.clipped = VK_TRUE;
	SwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	VkCheck(vkCreateSwapchainKHR(this->device, &SwapChainCreateInfo, VK_ALLOCK, &this->swapchain));
	VkCheck(vkGetSwapchainImagesKHR(this->device, this->swapchain, &swapchainImageCount, nullptr));

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	VkCheck(vkCreateFence(this->device, &fenceCreateInfo, VK_ALLOCK, &this->swapchainImageAvailable));


	uint32_t imageCount;
	vkGetSwapchainImagesKHR(this->device, this->swapchain, &imageCount, nullptr);
	this->swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(this->device, this->swapchain, &imageCount, this->swapChainImages.data());
	this->swapChainImageViews.resize(this->swapChainImages.size());

	for (size_t i = 0; i < this->swapChainImages.size(); i++) {
		this->swapChainImageViews[i] = this->createImageView(this->swapChainImages[i], this->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::DeInitSwapChain() {
	for (size_t i = 0; i < this->swapChainImages.size(); i++) {
		//vkDestroyImage(this->device, this->swapChainImages[i], VK_ALLOCK);
		vkDestroyImageView(this->device, this->swapChainImageViews[i], VK_ALLOCK);
		this->swapChainImageViews[i] = VK_NULL_HANDLE;
		this->swapChainImages[i] = VK_NULL_HANDLE;
	}

	vkDestroyFence(this->device, this->swapchainImageAvailable, VK_ALLOCK);
	this->swapchainImageAvailable = VK_NULL_HANDLE;

	vkDestroySwapchainKHR(this->device, this->swapchain, VK_ALLOCK);
	this->swapchain = VK_NULL_HANDLE;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::InitRenderPass() {
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = this->findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkCheck(vkCreateRenderPass(this->device, &renderPassInfo, VK_ALLOCK, &this->renderPass));
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::DeInitRenderPass() {
	vkDestroyRenderPass(this->device, this->renderPass, VK_ALLOCK);
	this->renderPass = VK_NULL_HANDLE;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::InitDepthStencil() {
	VkFormat depthFormat = findDepthFormat();

	this->createImage(this->surfaceExtent.width, this->surfaceExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->depthImage, this->depthImageMemory);
	this->depthImageView = this->createImageView(this->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	transitionImageLayout(this->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::DeInitDepthStencil() {
	vkDestroyImageView(this->device, this->depthImageView, VK_ALLOCK);
	this->depthImageView = VK_NULL_HANDLE;
	vkDestroyImage(this->device, this->depthImage, VK_ALLOCK);
	this->depthImage = VK_NULL_HANDLE;
	vkFreeMemory(this->device, this->depthImageMemory, VK_ALLOCK);
	this->depthImageMemory = VK_NULL_HANDLE;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::InitFrameBuffer() {
	this->swapChainFramebuffers.resize(this->swapChainImageViews.size());

	for (size_t i = 0; i < this->swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			this->swapChainImageViews[i],
			this->depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = this->renderPass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = this->surfaceExtent.width;
		framebufferInfo.height = this->surfaceExtent.height;
		framebufferInfo.layers = 1;

		VkCheck(vkCreateFramebuffer(device, &framebufferInfo, VK_ALLOCK, &this->swapChainFramebuffers[i]));
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::DeInitFrameBuffer() {
	for (auto f : this->swapChainFramebuffers) {
		vkDestroyFramebuffer(this->device, f, VK_ALLOCK);
		f = VK_NULL_HANDLE;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

GFContext::GFContext() {

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	this->instanceLayer.push_back("VK_LAYER_LUNARG_standard_validation");
	this->instanceExtension.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	this->instanceExtension.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	this->deviceExtension.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	this->deviceLayer.push_back("VK_LAYER_LUNARG_standard_validation");

	uint32_t instance_extension_count = 0;
	const char ** instance_extensions_buffer = glfwGetRequiredInstanceExtensions(&instance_extension_count);
	for (uint32_t i = 0; i < instance_extension_count; i++) {
		this->instanceExtension.push_back(instance_extensions_buffer[i]);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

GFContext::~GFContext() {
	VkCheck(vkQueueWaitIdle(this->queue));

	this->DeInitWindow();
	this->DeInitCommandPool();
	this->DeInitDevice();
#ifdef _DEBUG
	this->DeInitDebug();
#endif
	this->DeInitInstance();

	glfwTerminate();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::onWindowResize(GLFWwindow * window, int width, int height){
	if (width == 0) {
		width = 1;
	}

	if (height == 0) {
		height = 1;
	}

	GFContext* context = (GFContext*)(glfwGetWindowUserPointer(window));
	context->ResizeWindow();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::ResizeWindow(){
	int w, h;
	glfwGetWindowSize(this->window, &w, &h);
	this->surfaceExtent.width = w;
	this->surfaceExtent.height = h;

	
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::InitContext() {
	if (this->instance != VK_NULL_HANDLE) {
		return;
	}

	this->InitInstance();
#ifdef _DEBUG
	this->InitDebug();
#endif
	this->InitDevice();
	this->InitCommandPool();
	this->InitWindow();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::BeginRender() {
	VkCheck(vkAcquireNextImageKHR(this->device, this->swapchain, UINT64_MAX, VK_NULL_HANDLE, this->swapchainImageAvailable, &this->swapchainImageID));
	VkCheck(vkWaitForFences(this->device, 1, &this->swapchainImageAvailable, VK_TRUE, UINT64_MAX));
	VkCheck(vkResetFences(this->device, 1, &this->swapchainImageAvailable));
	VkCheck(vkQueueWaitIdle(this->queue));
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::EndRender(std::vector<VkSemaphore> waitSemaphores) {
	VkResult present_result = VkResult::VK_RESULT_MAX_ENUM;

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = waitSemaphores.size();
	presentInfo.pWaitSemaphores = waitSemaphores.data();
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &this->swapchain;
	presentInfo.pImageIndices = &this->swapchainImageID;
	presentInfo.pResults = &present_result;

	VkCheck(vkQueuePresentKHR(this->queue, &presentInfo));
	VkCheck(present_result);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

VkFramebuffer GFContext::GetActiveFrameBuffer() {
	return this->swapChainFramebuffers[this->swapchainImageID];
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkDevice GFContext::GetDevice() {
	return this->device;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkPhysicalDevice GFContext::GetGpu() {
	return this->gpu;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkPhysicalDeviceProperties GFContext::GetGpuProperties() {
	return this->gpuProperties;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkPhysicalDeviceMemoryProperties GFContext::GetGpuMemoryProperties() {
	return this->gpuMemoryProperties;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const uint32_t GFContext::GetQueueFamilyIndex() {
	return this->queueFamilyIndex;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkQueue GFContext::GetQueue() {
	return this->queue;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkCommandPool GFContext::GetCommandPool() {
	return this->commandPool;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkRenderPass GFContext::GetRenderPass() {
	return this->renderPass;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkSurfaceKHR GFContext::GetSurface() {
	return this->surface;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkSurfaceFormatKHR GFContext::sGetSurfaceFormat() {
	return this->surfaceFormat;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkExtent2D GFContext::GetSurfaceExtent() {
	return this->surfaceExtent;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkSurfaceCapabilitiesKHR GFContext::GetSurfaceCapabilities() {
	return this->surfaceCapabilities;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkPresentModeKHR GFContext::GetPresentMode() {
	return this->presentMode;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkSwapchainKHR GFContext::GetSwapchain() {
	return this->swapchain;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

const VkFormat GFContext::GetSwapChainImageFormat() {
	return this->swapChainImageFormat;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

uint32_t GFContext::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(this->gpu, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

VkCommandBuffer GFContext::beginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = this->commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	VkCheck(vkAllocateCommandBuffers(this->device, &allocInfo, &commandBuffer));

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VkCheck(vkBeginCommandBuffer(commandBuffer, &beginInfo));

	return commandBuffer;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
	VkCheck(vkEndCommandBuffer(commandBuffer));

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkCheck(vkQueueSubmit(this->queue, 1, &submitInfo, VK_NULL_HANDLE));
	VkCheck(vkQueueWaitIdle(this->queue));

	vkFreeCommandBuffers(this->device, commandPool, 1, &commandBuffer);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer cmd = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		VkCheck(VK_ERROR_VALIDATION_FAILED_EXT);
	}

	vkCmdPipelineBarrier(cmd, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	endSingleTimeCommands(cmd);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory) {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkCheck(vkCreateImage(this->device, &imageInfo, VK_ALLOCK, &image));

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	VkCheck(vkAllocateMemory(this->device, &allocInfo, VK_ALLOCK, &imageMemory));

	vkBindImageMemory(this->device, image, imageMemory, 0);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

VkImageView GFContext::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
	VkImageView imageView;
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	VkCheck(vkCreateImageView(this->device, &viewInfo, VK_ALLOCK, &imageView));

	return imageView;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBuffer cmd = beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	endSingleTimeCommands(cmd);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkCheck(vkCreateBuffer(this->device, &bufferInfo, VK_ALLOCK, &buffer));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(this->device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	VkCheck(vkAllocateMemory(this->device, &allocInfo, VK_ALLOCK, &bufferMemory));

	vkBindBufferMemory(this->device, buffer, bufferMemory, 0);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

void GFContext::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = this->beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	this->endSingleTimeCommands(commandBuffer);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

VkFormat GFContext::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(this->gpu, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}
	return VkFormat();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

VkFormat GFContext::findDepthFormat() {
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

VkGraphicsPipelineCreateInfo GFContext::CreatePipelineInfo(){

	/*VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(this->vertexInputBindingDescription.size());
	vertexInputInfo.pVertexBindingDescriptions = this->vertexInputBindingDescription.data();
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(this->vertexInputAttributeDescription.size());
	vertexInputInfo.pVertexAttributeDescriptions = this->vertexInputAttributeDescription.data();*/

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
		0,
		nullptr,
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayout pipelineLayout;
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	/*pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;*/
	VkCheck(vkCreatePipelineLayout(device, &pipelineLayoutInfo, VK_ALLOCK, &pipelineLayout));

	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicState = {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0,
		2,
		dynamicStates
	};


	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	//pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = this->renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	return pipelineInfo;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------- //

#ifdef _DEBUG

void _VkCheckError(VkResult r, const char* filename, const char* function, uint32_t line) {
	std::stringstream stream;
	std::string title;
	if (r < 0) {
		switch (r) {
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			title = "VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			title = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			title = "VK_ERROR_INITIALIZATION_FAILED";
			break;
		case VK_ERROR_DEVICE_LOST:
			title = "VK_ERROR_DEVICE_LOST";
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			title = "VK_ERROR_MEMORY_MAP_FAILED";
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			title = "VK_ERROR_LAYER_NOT_PRESENT";
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			title = "VK_ERROR_EXTENSION_NOT_PRESENT";
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			title = "VK_ERROR_FEATURE_NOT_PRESENT";
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			title = "VK_ERROR_INCOMPATIBLE_DRIVER";
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			title = "VK_ERROR_TOO_MANY_OBJECTS";
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			title = "VK_ERROR_FORMAT_NOT_SUPPORTED";
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			title = "VK_ERROR_SURFACE_LOST_KHR";
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			title = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
			break;
		case VK_SUBOPTIMAL_KHR:
			title = "VK_SUBOPTIMAL_KHR";
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			title = "VK_ERROR_OUT_OF_DATE_KHR";
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			title = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			title = "VK_ERROR_VALIDATION_FAILED_EXT";
			break;
		default:
			break;
		}
		stream << title << ":\n";
		stream << filename << ":" << line << "   " << function << "\n";
		std::cout << stream.str();
#if defined(_WIN32) || defined(_WIN64)
		MessageBox(NULL, stream.str().c_str(), "Vulkan Error", 0);
#endif
		exit(-1);
	}
}

void _VkDisplayError(std::string msg, const char* filename, const char* function, uint32_t line) {
	std::cout << filename << ":" << line << "  VkError: " << msg << "\n";
#if defined(_WIN32) || defined(_WIN64)
	MessageBox(NULL, msg.c_str(), "Vulkan Error", 0);
#endif
	std::exit(-1);
}

#else

void _VkCheckError(VkResult r, const char* function) {
	if (r != VK_SUCCESS) {
		std::cout << "VKError: " << function << "\n";
#if defined(_WIN32) || defined(_WIN64)
		MessageBox(NULL, function, "Vulkan Error", 0);
#endif
		std::exit(-1);
	}
}

void _VkDisplayError(std::string msg) {
	std::cout << "VkError: " << msg << "\n";
#if defined(_WIN32) || defined(_WIN64)
	MessageBox(NULL, msg.c_str(), "Vulkan Error", 0);
#endif
	std::exit(-1);
}

#endif // _DEBUG
