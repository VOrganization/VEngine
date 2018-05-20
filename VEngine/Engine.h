#pragma once
#include <chrono>
#include <thread>

using namespace std::chrono;

#include "Scene.h"

typedef struct{
	VkDescriptorSetLayoutBinding binding;
	VkDescriptorPoolSize pool;
	VkWriteDescriptorSet writer;
} _descriptor_item_info;

typedef struct {
	vec3 camPos;
} _engine_data_ubo;

class Engine{

	double last_time = 0;
	double windows_fps_tmp = 0;

	bool is_error = false;
	unsigned int error = 0;

	unsigned int render_loop_size = 0;

	unsigned int obj_index_tmp_0 = 0;
	unsigned int obj_index_tmp_1 = 0;
	unsigned int obj_index_tmp_2 = 0;
	unsigned int obj_index_tmp_3 = 0;
	void CalcObject(Object* obj);

	vector<ObjectMesh*>* RendererMeshes = nullptr;
	vector<ObjectLightPoint*>* RendererLight0 = nullptr;
	vector<ObjectLightSpot*>* RendererLight1 = nullptr;
	vector<ObjectLightDir*>* RendererLight2 = nullptr;

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	VkSemaphore semaphore = VK_NULL_HANDLE;

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	vector<_descriptor_item_info>* descriptorItems = nullptr;
	uint32_t descriptorItemsAlloc = 0;
	void descriptorUpdate();

	VkBuffer matrixUBOBuffer = VK_NULL_HANDLE;
	VkDeviceMemory matrixUBOBufferMemory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo matrixUBOBufferInfo;
	uint32_t matrixUBODataSize = sizeof(mat4) * 3;
	mat4* matrixUBOData = new mat4[3];
	
	VkBuffer matrixDynamicUBOBuffer = VK_NULL_HANDLE;
	VkDeviceMemory matrixDynamicUBOBufferMemory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo matrixDynamicUBOBufferInfo;
	uint32_t matrixDynamicAlignment = 0;
	uint32_t matrixDynamicAllocate = 1;
	mat4* matrixDynamicUBOData = nullptr;

	VkBuffer engineUBOBuffer = VK_NULL_HANDLE;
	VkDeviceMemory engineUBOBufferMemory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo engineUBOBufferInfo;
	_engine_data_ubo engineUBOData;

	VkBuffer lightPointUBOBuffer = VK_NULL_HANDLE;
	VkDeviceMemory lightPointUBOBufferMemory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo lightPointUBOBufferInfo;
	uint32_t lightPointUBOAllocate = 10;
	void* lightPointUBOData;

	VkBuffer lightSpotUBOBuffer = VK_NULL_HANDLE;
	VkDeviceMemory lightSpotUBOBufferMemory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo lightSpotUBOBufferInfo;
	uint32_t lightSpotUBOAllocate = 10;
	void* lightSpotUBOData;

	VkBuffer lightDirUBOBuffer = VK_NULL_HANDLE;
	VkDeviceMemory lightDirUBOBufferMemory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo lightDirUBOBufferInfo;
	uint32_t lightDirUBOAllocate = 10;
	void* lightDirUBOData;

	VkBuffer materialBuffer = VK_NULL_HANDLE;
	VkDeviceMemory materialBufferMemory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo materialBufferInfo;
	uint32_t materialAllocate = 10;
	void* materialData = nullptr;

	vec3 camPos;
	Transform tmp_tran;

	void Init();
	void VkInit();

public:
	Engine(); 
	Engine(int argc, char** argv);
	virtual ~Engine();

	Scene* scene;

	GFContext* context = nullptr;

	GLFWwindow* window = nullptr;

	int win_width = 800;
	int win_height = 600;
	string win_title = "EngineV2";
	double window_fps = 60;

	void InitScene();
	void Update();
	bool IsOpen();

	Mesh* tmp_m;

};

