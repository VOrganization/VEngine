#include "Engine.h"

typedef struct {
	vec4 pos;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 distance;
} _light_point_data;

typedef struct {
	vec4 pos;
	vec4 dir;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 distance;
	vec4 spot;
} _light_spot_data;

typedef struct {
	vec4 pos;
	vec4 dir;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 intensity;
} _light_dir_data;

typedef struct {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 shininess;
} _material_data;

// -------------------------------------------------------------------------------------------- //

void Engine::CalcObject(Object * obj){

	if (obj->parent != nullptr) {
		obj->tmp_transform->position = obj->transform->position + obj->parent->tmp_transform->position;
		obj->tmp_transform->rotation = obj->transform->rotation + obj->parent->tmp_transform->rotation;
		obj->tmp_transform->scale    = obj->transform->scale * obj->parent->tmp_transform->scale;
	}
	else {
		obj->tmp_transform->position = obj->transform->position + this->scene->WorldTransform->position;
		obj->tmp_transform->rotation = obj->transform->rotation + this->scene->WorldTransform->rotation;
		obj->tmp_transform->scale = obj->transform->scale * + this->scene->WorldTransform->scale;
	}

	//Obliczanie Animacji z systemu animacji

	switch (obj->type){

	case OT_MESH: {
		if (this->obj_index_tmp_0 < this->RendererMeshes->size()) {
			this->RendererMeshes->at(this->obj_index_tmp_0) = (ObjectMesh*)obj;
		}
		else {
			this->RendererMeshes->push_back((ObjectMesh*)obj);
		}

		/*if (((ObjectMesh*)obj)->armature != nullptr) {
			EnterCriticalSection(&cs);
			((ObjectMesh*)obj)->armature->CalcData();
			LeaveCriticalSection(&cs);
		}*/
		this->obj_index_tmp_0 += 1;
		break;
	}

	case OT_LIGHT_POINT: {
		if (this->obj_index_tmp_1 < this->RendererLight0->size())
			this->RendererLight0->at(this->obj_index_tmp_1) = (ObjectLightPoint*)obj;
		else
			this->RendererLight0->push_back((ObjectLightPoint*)obj);
		this->obj_index_tmp_1 += 1;
		break;
	}

	case OT_LIGHT_SPOT: {
		if (this->obj_index_tmp_2 < this->RendererLight1->size())
			this->RendererLight1->at(this->obj_index_tmp_2) = (ObjectLightSpot*)obj;
		else
			this->RendererLight1->push_back((ObjectLightSpot*)obj);
		this->obj_index_tmp_2 += 1;
		break;
	}

	case OT_LIGHT_DIR: {
		if (this->obj_index_tmp_3 < this->RendererLight2->size())
			this->RendererLight2->at(this->obj_index_tmp_3) = (ObjectLightDir*)obj;
		else
			this->RendererLight2->push_back((ObjectLightDir*)obj);
		this->obj_index_tmp_3 += 1;
		break;
	}

	case OT_CAMERA: {
		if (((ObjectCamera*)obj)->Default) {
			this->camPos = obj->tmp_transform->position;
			if (((ObjectCamera*)obj)->UseDirection)
				this->matrixUBOData[1] = lookAt(this->camPos, this->camPos + ((ObjectCamera*)obj)->Direction, ((ObjectCamera*)obj)->Up);
			else {
				vec3 rot = obj->transform->rotation;

				float Ax = radians(rot.x);
				float Ay = radians(rot.y);
				float Az = radians(rot.z);

				vec3 dir = vec3(cos(Ay) * sin(Ax), sin(Ay), cos(Ay) * cos(Ax));

				this->matrixUBOData[1] = lookAt(this->camPos, this->camPos + dir, vec3(sin(Az), 1, cos(Az)));
			}
			if (((ObjectCamera*)obj)->Perpective)
				this->matrixUBOData[0] = perspective(radians(((ObjectCamera*)obj)->Fov), (float)this->win_width / (float)this->win_height, ((ObjectCamera*)obj)->Near, ((ObjectCamera*)obj)->Far);
			else
				this->matrixUBOData[0] = ortho(0, this->win_width, this->win_height, 0, -5, 5);
		}
		break;
	}

	default:
		break;
	}
	

	for (size_t i = 0; i < obj->children->size(); i++)
		this->CalcObject(obj->children->at(i));
}

// -------------------------------------------------------------------------------------------- //

void Engine::descriptorUpdate(){
	if (this->descriptorSetLayout == VK_NULL_HANDLE) {
		vector<VkDescriptorSetLayoutBinding> bindings;
		for (uint32_t i = 0; i < this->descriptorItems->size(); i++) {
			bindings.push_back(this->descriptorItems->at(i).binding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		VkCheck(vkCreateDescriptorSetLayout(this->context->GetDevice(), &layoutInfo, VK_ALLOCK, &this->descriptorSetLayout));
		this->context->descriptorSetLayouts.push_back(this->descriptorSetLayout);
	}

	if (this->descriptorSet == VK_NULL_HANDLE || this->descriptorItemsAlloc != this->descriptorItems->size()) {

		if (this->descriptorItemsAlloc != this->descriptorItems->size() && this->descriptorSet != VK_NULL_HANDLE) {
			vkFreeDescriptorSets(this->context->GetDevice(), this->descriptorPool, 1, &this->descriptorSet);
			vkDestroyDescriptorPool(this->context->GetDevice(), this->descriptorPool, VK_ALLOCK);
		}

		this->descriptorItemsAlloc = this->descriptorItems->size();

		vector<VkDescriptorPoolSize> poolSizes;
		for (uint32_t i = 0; i < this->descriptorItems->size(); i++) {
			poolSizes.push_back(this->descriptorItems->at(i).pool);
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;
		VkCheck(vkCreateDescriptorPool(this->context->GetDevice(), &poolInfo, VK_ALLOCK, &this->descriptorPool));

		VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.pSetLayouts = layouts;
		VkCheck(vkAllocateDescriptorSets(this->context->GetDevice(), &descriptorSetAllocateInfo, &this->descriptorSet));

	}

	VkDescriptorBufferInfo bufferInfo0 = this->matrixUBOBufferInfo;
	this->descriptorItems->at(0).writer.pBufferInfo = &bufferInfo0;

	VkDescriptorBufferInfo bufferInfo1 = this->matrixDynamicUBOBufferInfo;
	this->descriptorItems->at(1).writer.pBufferInfo = &bufferInfo1;

	VkDescriptorBufferInfo bufferInfo2 = this->engineUBOBufferInfo;
	this->descriptorItems->at(2).writer.pBufferInfo = &bufferInfo2;

	VkDescriptorBufferInfo bufferInfo3 = this->materialBufferInfo;
	this->descriptorItems->at(3).writer.pBufferInfo = &bufferInfo3;

	VkDescriptorBufferInfo bufferInfo4 = this->lightPointUBOBufferInfo;
	this->descriptorItems->at(4).writer.pBufferInfo = &bufferInfo4;

	VkDescriptorBufferInfo bufferInfo5 = this->lightSpotUBOBufferInfo;
	this->descriptorItems->at(5).writer.pBufferInfo = &bufferInfo5;

	VkDescriptorBufferInfo bufferInfo6 = this->lightDirUBOBufferInfo;
	this->descriptorItems->at(6).writer.pBufferInfo = &bufferInfo6;


	vector<VkWriteDescriptorSet> descriptorWrites;
	for (uint32_t i = 0; i < this->descriptorItems->size(); i++) {
		this->descriptorItems->at(i).writer.dstSet = this->descriptorSet;
		descriptorWrites.push_back(this->descriptorItems->at(i).writer);
	}
	vkUpdateDescriptorSets(this->context->GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

// -------------------------------------------------------------------------------------------- //

void Engine::Init(){

	this->RendererMeshes = new vector<ObjectMesh*>();
	this->RendererLight0 = new vector<ObjectLightPoint*>();
	this->RendererLight1 = new vector<ObjectLightSpot*>();
	this->RendererLight2 = new vector<ObjectLightDir*>();
	this->descriptorItems = new vector<_descriptor_item_info>();

	//Initialize FreeImage
	FreeImage_Initialise();

	//Init Context
	this->context = new GFContext();
	this->context->swapchainImageCount = 3;
	this->context->appName = this->win_title;
	this->context->engineName = "EngineV2";
	this->context->engineVersion = VK_MAKE_VERSION(2, 4, 0);
	// this->context->window = glfwCreateWindow(this->win_width, this->win_height, this->win_title.c_str(), glfwGetPrimaryMonitor(), NULL);
	this->context->window = glfwCreateWindow(this->win_width, this->win_height, this->win_title.c_str(), NULL, NULL);
	this->context->InitContext();
	this->window = this->context->window;

	this->context->vertexInputBindingDescription.push_back({
		0,
		sizeof(Vertex),
		VK_VERTEX_INPUT_RATE_VERTEX
	});

	this->context->vertexInputAttributeDescription.push_back({
		0,
		0,
		VK_FORMAT_R32G32B32_SFLOAT,
		offsetof(Vertex, pos)
	});

	this->context->vertexInputAttributeDescription.push_back({
		1,
		0,
		VK_FORMAT_R32G32_SFLOAT,
		offsetof(Vertex, uv)
	});

	this->context->vertexInputAttributeDescription.push_back({
		2,
		0,
		VK_FORMAT_R32G32B32_SFLOAT,
		offsetof(Vertex, normal)
	});

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	this->context->colorBlendAttachments.push_back(colorBlendAttachment);

	this->VkInit();

	cout << "\n\n\n";
}

// -------------------------------------------------------------------------------------------- //

void Engine::VkInit(){

	//global matrix ubo
	{
		VkDeviceSize bufferSize = sizeof(mat4) * 3;
		this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->matrixUBOBuffer, this->matrixUBOBufferMemory);

		this->matrixUBOBufferInfo = {
			this->matrixUBOBuffer,
			0,
			bufferSize
		};

		void* data;
		vkMapMemory(this->context->GetDevice(), this->matrixUBOBufferMemory, 0, this->matrixUBODataSize, 0, &data);
		memcpy(data, this->matrixUBOData, this->matrixUBODataSize);
		vkUnmapMemory(this->context->GetDevice(), this->matrixUBOBufferMemory);

	}

	//object instance ubo
	{

		size_t minUboAlignment = this->context->GetGpuProperties().limits.minUniformBufferOffsetAlignment;
		this->matrixDynamicAlignment = sizeof(mat4) + sizeof(uint32_t);
		if (minUboAlignment > 0) {
			this->matrixDynamicAlignment = (this->matrixDynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
		}

		VkDeviceSize bufferSize = this->matrixDynamicAlignment * this->matrixDynamicAllocate;
		this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->matrixDynamicUBOBuffer, this->matrixDynamicUBOBufferMemory);

		this->matrixDynamicUBOBufferInfo = {
			this->matrixDynamicUBOBuffer,
			0,
			this->matrixDynamicAlignment
		};
		
		this->matrixDynamicUBOData = (mat4*)malloc(this->matrixDynamicAllocate * this->matrixDynamicAlignment);
		for (size_t i = 0; i < this->matrixDynamicAllocate; i++){
			mat4* modelMat = (mat4*)(((uint64_t)this->matrixDynamicUBOData + (i * this->matrixDynamicAlignment)));
			*modelMat = this->tmp_tran.GetMatrix();
		}

		void* data;
		vkMapMemory(this->context->GetDevice(), this->matrixDynamicUBOBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->matrixDynamicUBOData, bufferSize);
		vkUnmapMemory(this->context->GetDevice(), this->matrixDynamicUBOBufferMemory);

		VkMappedMemoryRange memoryRange = {
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			this->matrixDynamicUBOBufferMemory,
			0,
			VK_WHOLE_SIZE
		};
		vkFlushMappedMemoryRanges(this->context->GetDevice(), 1, &memoryRange);
	}

	//engine data ubo
	{
		VkDeviceSize bufferSize = sizeof(_engine_data_ubo);
		this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->engineUBOBuffer, this->engineUBOBufferMemory);

		this->engineUBOBufferInfo = {
			this->engineUBOBuffer,
			0,
			bufferSize
		};

		void* data;
		vkMapMemory(this->context->GetDevice(), this->engineUBOBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, &this->engineUBOData, bufferSize);
		vkUnmapMemory(this->context->GetDevice(), this->engineUBOBufferMemory);
	}

	//texture

	//material
	{
		VkDeviceSize bufferSize = sizeof(_material_data) * this->materialAllocate + sizeof(vec4);
		this->materialData = malloc(bufferSize);
		this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->materialBuffer, this->materialBufferMemory);

		this->materialBufferInfo = {
			this->materialBuffer,
			0,
			bufferSize
		};

		void* data;
		vkMapMemory(this->context->GetDevice(), this->materialBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->materialData, bufferSize);
		vkUnmapMemory(this->context->GetDevice(), this->materialBufferMemory);
	}

	//light point
	{
		VkDeviceSize bufferSize = sizeof(_light_point_data) * this->lightPointUBOAllocate + sizeof(vec4);
		this->lightPointUBOData = malloc(bufferSize);
		this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->lightPointUBOBuffer, this->lightPointUBOBufferMemory);

		this->lightPointUBOBufferInfo = {
			this->lightPointUBOBuffer,
			0,
			bufferSize
		};

		void* data;
		vkMapMemory(this->context->GetDevice(), this->lightPointUBOBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->lightPointUBOData, bufferSize);
		vkUnmapMemory(this->context->GetDevice(), this->lightPointUBOBufferMemory);
	}

	//light spot
	{
		VkDeviceSize bufferSize = sizeof(_light_spot_data) * this->lightSpotUBOAllocate + sizeof(vec4);
		this->lightSpotUBOData = malloc(bufferSize);
		this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->lightSpotUBOBuffer, this->lightSpotUBOBufferMemory);

		this->lightSpotUBOBufferInfo = {
			this->lightSpotUBOBuffer,
			0,
			bufferSize
		};

		void* data;
		vkMapMemory(this->context->GetDevice(), this->lightSpotUBOBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->lightSpotUBOData, bufferSize);
		vkUnmapMemory(this->context->GetDevice(), this->lightSpotUBOBufferMemory);
	}

	//light dir
	{
		VkDeviceSize bufferSize = sizeof(_light_dir_data) * this->lightDirUBOAllocate + sizeof(vec4);
		this->lightDirUBOData = malloc(bufferSize);
		this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->lightDirUBOBuffer, this->lightDirUBOBufferMemory);

		this->lightDirUBOBufferInfo = {
			this->lightDirUBOBuffer,
			0,
			bufferSize
		};

		void* data;
		vkMapMemory(this->context->GetDevice(), this->lightDirUBOBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->lightDirUBOData, bufferSize);
		vkUnmapMemory(this->context->GetDevice(), this->lightDirUBOBufferMemory);
	}

	//descriptor
	{
		VkDescriptorBufferInfo bufferInfo0 = this->matrixUBOBufferInfo;
		VkDescriptorBufferInfo bufferInfo1 = this->matrixDynamicUBOBufferInfo;
		VkDescriptorBufferInfo bufferInfo2 = this->engineUBOBufferInfo;
		VkDescriptorBufferInfo bufferInfo3 = this->materialBufferInfo;
		VkDescriptorBufferInfo bufferInfo4 = this->lightPointUBOBufferInfo;
		VkDescriptorBufferInfo bufferInfo5 = this->lightSpotUBOBufferInfo;
		VkDescriptorBufferInfo bufferInfo6 = this->lightDirUBOBufferInfo;

		//global matrix
		this->descriptorItems->push_back({
			{
				0,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT,
				nullptr
			},
			{
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				0,
				0,
				0,
				1,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				nullptr,
				&bufferInfo0,
				nullptr,
			}
		});

		//object matrix
		this->descriptorItems->push_back({
			{
				1,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
				1,
				VK_SHADER_STAGE_VERTEX_BIT,
				nullptr
			},
			{
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
				1
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				0,
				1,
				0,
				1,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
				nullptr,
				&bufferInfo1,
				nullptr,
			}
		});

		//engine
		this->descriptorItems->push_back({
			{
				2,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				0,
				2,
				0,
				1,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				nullptr,
				&bufferInfo2,
				nullptr,
			}
		});

		//material
		this->descriptorItems->push_back({
			{
				3,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_VERTEX_BIT,
				nullptr
			},
			{
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				0,
				3,
				0,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				nullptr,
				&bufferInfo3,
				nullptr,
			}
			});

		//light point
		this->descriptorItems->push_back({
			{
				4,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				0,
				4,
				0,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				nullptr,
				&bufferInfo4,
				nullptr,
			}
		});

		//light spot
		this->descriptorItems->push_back({
			{
				5,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				0,
				5,
				0,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				nullptr,
				&bufferInfo5,
				nullptr,
			}
		});

		//light dir
		this->descriptorItems->push_back({
			{
				6,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr
			},
			{
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				0,
				6,
				0,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				nullptr,
				&bufferInfo6,
				nullptr,
			}
		});

	}

	this->descriptorUpdate();

	//Init VK Stuff

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->context->GetCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	VkCheck(vkAllocateCommandBuffers(this->context->GetDevice(), &allocInfo, &this->commandBuffer));

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkCheck(vkCreateSemaphore(this->context->GetDevice(), &semaphoreInfo, nullptr, &this->semaphore));
}

// -------------------------------------------------------------------------------------------- //

Engine::Engine(){
	this->Init();
}

// -------------------------------------------------------------------------------------------- //

Engine::Engine(int argc, char ** argv){
	this->Init();
}

// -------------------------------------------------------------------------------------------- //

Engine::~Engine(){
	VkCheck(vkQueueWaitIdle(this->context->GetQueue()));
	VkCheck(vkDeviceWaitIdle(this->context->GetDevice()));

	if (this->scene != nullptr) {
		delete this->scene;
		this->scene = nullptr;
	}

	vkDestroyDescriptorPool(this->context->GetDevice(), this->descriptorPool, VK_ALLOCK);
	vkDestroyDescriptorSetLayout(this->context->GetDevice(), this->descriptorSetLayout, VK_ALLOCK);

	vkDestroyBuffer(this->context->GetDevice(), this->matrixUBOBuffer, VK_ALLOCK);
	vkFreeMemory(this->context->GetDevice(), this->matrixUBOBufferMemory, VK_ALLOCK);

	vkDestroyBuffer(this->context->GetDevice(), this->matrixDynamicUBOBuffer, VK_ALLOCK);
	vkFreeMemory(this->context->GetDevice(), this->matrixDynamicUBOBufferMemory, VK_ALLOCK);

	vkDestroyBuffer(this->context->GetDevice(), this->materialBuffer, VK_ALLOCK);
	vkFreeMemory(this->context->GetDevice(), this->materialBufferMemory, VK_ALLOCK);
	delete this->materialData;
	this->materialData = nullptr;

	vkDestroyBuffer(this->context->GetDevice(), this->engineUBOBuffer, VK_ALLOCK);
	vkFreeMemory(this->context->GetDevice(), this->engineUBOBufferMemory, VK_ALLOCK);

	vkDestroyBuffer(this->context->GetDevice(), this->lightPointUBOBuffer, VK_ALLOCK);
	vkFreeMemory(this->context->GetDevice(), this->lightPointUBOBufferMemory, VK_ALLOCK);
	delete this->lightPointUBOData;

	vkDestroyBuffer(this->context->GetDevice(), this->lightSpotUBOBuffer, VK_ALLOCK);
	vkFreeMemory(this->context->GetDevice(), this->lightSpotUBOBufferMemory, VK_ALLOCK);
	delete this->lightSpotUBOData;

	vkDestroyBuffer(this->context->GetDevice(), this->lightDirUBOBuffer, VK_ALLOCK);
	vkFreeMemory(this->context->GetDevice(), this->lightDirUBOBufferMemory, VK_ALLOCK);
	delete this->lightDirUBOData;

	vkDestroySemaphore(this->context->GetDevice(), this->semaphore, VK_ALLOCK);
	vkFreeCommandBuffers(this->context->GetDevice(), this->context->GetCommandPool(), 1, &this->commandBuffer);

	this->context->~GFContext();

	delete this->RendererMeshes;
	this->RendererMeshes = nullptr;
	delete this->RendererLight0;
	this->RendererLight0 = nullptr;
	delete this->RendererLight1;
	this->RendererLight1 = nullptr;
	delete this->RendererLight2;
	this->RendererLight2 = nullptr;
}

// -------------------------------------------------------------------------------------------- //

void Engine::InitScene(){
	if (this->scene == nullptr) {
		return;
	}

	this->last_time = glfwGetTime();
	if (this->scene->defaultCamera == nullptr) {
		this->scene->defaultCamera = new ObjectCamera();
		this->scene->defaultCamera->Default = true;
		this->scene->Objects->push_back(this->scene->defaultCamera);
	}

	this->scene->default_shader->Init(this->context);
	for (unsigned int i = 0; i < this->scene->Shaders->size(); i++){
		this->scene->Shaders->at(i)->Init(this->context);
	}

	for (unsigned int i = 0; i < this->scene->Meshes->size(); i++) {
		this->scene->Meshes->at(i)->Init(this->context);
	}
}

// -------------------------------------------------------------------------------------------- //

void Engine::Update(){

	double currentTime = glfwGetTime();
	this->windows_fps_tmp += 1.0;
	if (currentTime - this->last_time >= 1.0) {
		this->window_fps = this->windows_fps_tmp;
		this->windows_fps_tmp = 0;
		this->last_time += 1.0;
	}

	if (this->scene == nullptr || !this->scene->load) {
		glfwPollEvents();
		return;
	}

	//Calc object data
	{
		this->obj_index_tmp_0 = 0;
		this->obj_index_tmp_1 = 0;
		this->obj_index_tmp_2 = 0;
		this->obj_index_tmp_3 = 0;

		for (size_t i = 0; i < this->scene->Objects->size(); i++) {
			this->CalcObject(this->scene->Objects->at(i));
		}
	}

	//Calc objects matrix
	{
		if (this->matrixDynamicAllocate < this->RendererMeshes->size()) {
			this->matrixDynamicAllocate = this->RendererMeshes->size();

			VkDeviceSize bufferSize = this->matrixDynamicAlignment * this->matrixDynamicAllocate;

			delete this->matrixDynamicUBOData;
			this->matrixDynamicUBOData = (mat4*)malloc(bufferSize);

			vkDestroyBuffer(this->context->GetDevice(), this->matrixDynamicUBOBuffer, VK_ALLOCK);
			vkFreeMemory(this->context->GetDevice(), this->matrixDynamicUBOBufferMemory, VK_ALLOCK);
			this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->matrixDynamicUBOBuffer, this->matrixDynamicUBOBufferMemory);

			this->matrixDynamicUBOBufferInfo = {
				this->matrixDynamicUBOBuffer,
				0,
				this->matrixDynamicAlignment
			};
			
			this->descriptorUpdate();
		}

		for (size_t i = 0; i < this->RendererMeshes->size(); i++) {
			mat4* matrix = (mat4*)(((uint64_t)this->matrixDynamicUBOData + (i * this->matrixDynamicAlignment)));
			*matrix = this->RendererMeshes->at(i)->tmp_transform->GetMatrix();

			uint32_t* material = (uint32_t*)(((uint64_t)this->matrixDynamicUBOData + (i * this->matrixDynamicAlignment) + sizeof(mat4)));
			*material = this->RendererMeshes->at(i)->material->engine_id;
		}

		{
			VkDeviceSize bufferSize = this->matrixDynamicAlignment * this->RendererMeshes->size();

			void* data;
			vkMapMemory(this->context->GetDevice(), this->matrixDynamicUBOBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, this->matrixDynamicUBOData, bufferSize);
			vkUnmapMemory(this->context->GetDevice(), this->matrixDynamicUBOBufferMemory);

			VkMappedMemoryRange memoryRange = {
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				nullptr,
				this->matrixDynamicUBOBufferMemory,
				0,
				VK_WHOLE_SIZE
			};
			vkFlushMappedMemoryRanges(this->context->GetDevice(), 1, &memoryRange);
		}
	}

	//Calc material data
	{
		if (this->materialAllocate < this->scene->Materials->size()) {
			this->materialAllocate = this->scene->Materials->size();

			VkDeviceSize bufferSize = this->materialAllocate * sizeof(_material_data) + sizeof(vec4);

			delete this->materialData;
			this->materialData = malloc(bufferSize);

			vkDestroyBuffer(this->context->GetDevice(), this->materialBuffer, VK_ALLOCK);
			vkFreeMemory(this->context->GetDevice(), this->materialBufferMemory, VK_ALLOCK);
			this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->materialBuffer, this->materialBufferMemory);

			this->materialBufferInfo = {
				this->materialBuffer,
				0,
				bufferSize
			};

			this->descriptorUpdate();
		}

		{
			VkDeviceSize bufferSize = this->materialAllocate * sizeof(_material_data) + sizeof(vec4);

			uint32_t* size = (uint32_t*)(((uint64_t)this->materialData + (0)));
			*size = this->materialAllocate;
			for (size_t i = 0; i < this->scene->Materials->size(); i++) {
				_material_data* m = (_material_data*)(((uint64_t)this->materialData + (i * sizeof(_material_data) + sizeof(vec4))));
				
				m->ambient = this->scene->Materials->at(i)->ambient;
				m->diffuse = this->scene->Materials->at(i)->diffuse;
				m->specular = this->scene->Materials->at(i)->specular;
				m->shininess.x = 32.0f;
			}

			void* data;
			vkMapMemory(this->context->GetDevice(), this->materialBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, this->materialData, bufferSize);
			vkUnmapMemory(this->context->GetDevice(), this->materialBufferMemory);
		}
	}

	//Calc light point data
	{
		if (this->lightPointUBOAllocate < this->RendererLight0->size()) {
			this->lightPointUBOAllocate = this->RendererLight0->size();
			VkDeviceSize bufferSize = this->lightPointUBOAllocate * sizeof(_light_point_data) + sizeof(vec4);

			delete this->lightPointUBOData;
			this->lightPointUBOData = malloc(bufferSize);

			vkDestroyBuffer(this->context->GetDevice(), this->lightPointUBOBuffer, VK_ALLOCK);
			vkFreeMemory(this->context->GetDevice(), this->lightPointUBOBufferMemory, VK_ALLOCK);
			this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->lightPointUBOBuffer, this->lightPointUBOBufferMemory);

			this->lightPointUBOBufferInfo = {
				this->lightPointUBOBuffer,
				0,
				bufferSize
			};

			this->descriptorUpdate();
		}

		{
			VkDeviceSize bufferSize = this->lightPointUBOAllocate * sizeof(_light_point_data) + sizeof(vec4);

			uint32_t* size = (uint32_t*)(((uint64_t)this->lightPointUBOData + (0)));
			*size = this->lightPointUBOAllocate;
			for (size_t i = 0; i < this->RendererLight0->size(); i++){
				_light_point_data* l = (_light_point_data*)(((uint64_t)this->lightPointUBOData + (i * sizeof(_light_point_data) + sizeof(vec4))));
				(*l).pos = vec4(this->RendererLight0->at(i)->tmp_transform->position, 0);
				(*l).ambient = vec4(this->RendererLight0->at(i)->ambient, 0);
				(*l).diffuse = vec4(this->RendererLight0->at(i)->diffuse, 0);
				(*l).specular = vec4(this->RendererLight0->at(i)->specular, 0);
				(*l).distance = vec4(this->RendererLight0->at(i)->constant, this->RendererLight0->at(i)->linear, this->RendererLight0->at(i)->quadratic, this->RendererLight0->at(i)->intensity);
			}

			void* data;
			vkMapMemory(this->context->GetDevice(), this->lightPointUBOBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, this->lightPointUBOData, bufferSize);
			vkUnmapMemory(this->context->GetDevice(), this->lightPointUBOBufferMemory);
		}

	}

	//Calc light spot data
	{
		if (this->lightSpotUBOAllocate < this->RendererLight1->size()) {
			this->lightSpotUBOAllocate = this->RendererLight1->size();
			VkDeviceSize bufferSize = this->lightSpotUBOAllocate * sizeof(_light_spot_data) + sizeof(vec4);

			delete this->lightSpotUBOData;
			this->lightSpotUBOData = malloc(bufferSize);

			vkDestroyBuffer(this->context->GetDevice(), this->lightSpotUBOBuffer, VK_ALLOCK);
			vkFreeMemory(this->context->GetDevice(), this->lightSpotUBOBufferMemory, VK_ALLOCK);
			this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->lightSpotUBOBuffer, this->lightSpotUBOBufferMemory);

			this->lightSpotUBOBufferInfo = {
				this->lightSpotUBOBuffer,
				0,
				bufferSize
			};

			this->descriptorUpdate();
		}

		{
			uint32_t* size = (uint32_t*)(((uint64_t)this->lightSpotUBOData + (0)));
			*size = this->lightSpotUBOAllocate;
			for (size_t i = 0; i < this->RendererLight1->size(); i++) {
				_light_spot_data* l = (_light_spot_data*)(((uint64_t)this->lightSpotUBOData + (i * sizeof(_light_spot_data) + sizeof(vec4))));
				(*l).pos = vec4(this->RendererLight1->at(i)->tmp_transform->position, 0);
				(*l).dir = vec4(this->RendererLight2->at(i)->tmp_transform->GetDirection(vec3(0, 1, 0)), 0);
				(*l).ambient = vec4(this->RendererLight1->at(i)->ambient, 0);
				(*l).diffuse = vec4(this->RendererLight1->at(i)->diffuse, 0);
				(*l).specular = vec4(this->RendererLight1->at(i)->specular, 0);
				(*l).distance = vec4(this->RendererLight1->at(i)->constant, this->RendererLight0->at(i)->linear, this->RendererLight0->at(i)->quadratic, this->RendererLight0->at(i)->intensity);
				(*l).spot = vec4(this->RendererLight1->at(i)->cutOff, this->RendererLight1->at(i)->outerCutOff, 0, 0);
			}

			VkDeviceSize bufferSize = this->lightSpotUBOAllocate * sizeof(_light_spot_data) + sizeof(vec4);
			void* data;
			vkMapMemory(this->context->GetDevice(), this->lightSpotUBOBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, this->lightSpotUBOData, bufferSize);
			vkUnmapMemory(this->context->GetDevice(), this->lightSpotUBOBufferMemory);
		}
	}

	//Calc light dir data
	{
		if (this->lightDirUBOAllocate < this->RendererLight2->size()) {
			this->lightDirUBOAllocate = this->RendererLight2->size();
			VkDeviceSize bufferSize = this->lightDirUBOAllocate * sizeof(_light_dir_data) + sizeof(vec4);

			delete this->lightDirUBOData;
			this->lightDirUBOData = malloc(bufferSize);

			vkDestroyBuffer(this->context->GetDevice(), this->lightDirUBOBuffer, VK_ALLOCK);
			vkFreeMemory(this->context->GetDevice(), this->lightDirUBOBufferMemory, VK_ALLOCK);
			this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->lightDirUBOBuffer, this->lightDirUBOBufferMemory);

			this->lightDirUBOBufferInfo = {
				this->lightDirUBOBuffer,
				0,
				bufferSize
			};

			this->descriptorUpdate();
		}

		{
			*(uint32_t*)(((uint64_t)this->lightDirUBOData + (0))) = this->RendererLight2->size();
			for (size_t i = 0; i < this->RendererLight2->size(); i++) {
				_light_dir_data* l = (_light_dir_data*)(((uint64_t)this->lightDirUBOData + (i * sizeof(_light_dir_data) + sizeof(vec4))));
				(*l).pos = vec4(this->RendererLight2->at(i)->tmp_transform->position, 0);
				(*l).dir = vec4(this->RendererLight2->at(i)->tmp_transform->GetDirection(vec3(0, -1, 0)), 0);
				(*l).ambient = vec4(this->RendererLight2->at(i)->ambient, 0);
				(*l).diffuse = vec4(this->RendererLight2->at(i)->diffuse, 0);
				(*l).specular = vec4(this->RendererLight2->at(i)->specular, 0);
				(*l).intensity.x = this->RendererLight2->at(i)->intensity;
			}

			VkDeviceSize bufferSize = this->lightDirUBOAllocate * sizeof(_light_dir_data) + sizeof(vec4);
			void* data;
			vkMapMemory(this->context->GetDevice(), this->lightDirUBOBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, this->lightDirUBOData, bufferSize);
			vkUnmapMemory(this->context->GetDevice(), this->lightDirUBOBufferMemory);
		}
	}

	//send main matrix
	{
		void* data;
		vkMapMemory(this->context->GetDevice(), this->matrixUBOBufferMemory, 0, this->matrixUBODataSize, 0, &data);
		memcpy(data, this->matrixUBOData, this->matrixUBODataSize);
		vkUnmapMemory(this->context->GetDevice(), this->matrixUBOBufferMemory);
	}

	//send engine info
	{
		this->engineUBOData.camPos = this->camPos;
		void* data;
		vkMapMemory(this->context->GetDevice(), this->engineUBOBufferMemory, 0, sizeof(_engine_data_ubo), 0, &data);
		memcpy(data, &this->engineUBOData, sizeof(_engine_data_ubo));
		vkUnmapMemory(this->context->GetDevice(), this->engineUBOBufferMemory);
	}

	//Rendering main scene
	{

		this->context->BeginRender();
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VkCheck(vkBeginCommandBuffer(this->commandBuffer, &commandBufferBeginInfo));
		{
			VkViewport viewport = {
				0.0f,
				0.0f,
				(float)this->win_width,
				(float)this->win_height,
				0.0f,
				1.0f
			};
			vkCmdSetViewport(this->commandBuffer, 0, 1, &viewport);

			VkRect2D scissor = {
				{ 0, 0 },
				{ this->win_width, this->win_height }
			};
			vkCmdSetScissor(this->commandBuffer, 0, 1, &scissor);

			VkRect2D render_area{};
			render_area.offset.x = 0;
			render_area.offset.y = 0;
			render_area.extent = this->context->GetSurfaceExtent();

			VkClearValue clearValues[2];
			clearValues[0].color = { 0.3f, 0.3f, 0.3f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };

			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = this->context->GetRenderPass();
			renderPassBeginInfo.framebuffer = this->context->GetActiveFrameBuffer();
			renderPassBeginInfo.renderArea = render_area;
			renderPassBeginInfo.clearValueCount = 2;
			renderPassBeginInfo.pClearValues = clearValues;
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->scene->default_shader->GetPipeline());

			for (size_t i = 0; i < this->RendererMeshes->size(); i++) {
				uint32_t dynamicOffset = i * static_cast<uint32_t>(this->matrixDynamicAlignment);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->scene->default_shader->GetPipelineLayout(), 0, 1, &this->descriptorSet, 1, &dynamicOffset);
				this->RendererMeshes->at(i)->mesh->Draw(this->commandBuffer);
			}


			vkCmdEndRenderPass(this->commandBuffer);
		}
		VkCheck(vkEndCommandBuffer(this->commandBuffer));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &this->commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &this->semaphore;
		VkCheck(vkQueueSubmit(this->context->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE));

		this->context->EndRender({ this->semaphore });

	}

	this_thread::sleep_for(10ms);

	glfwPollEvents();
}

// -------------------------------------------------------------------------------------------- //

bool Engine::IsOpen(){
	return !glfwWindowShouldClose(this->window);
}
