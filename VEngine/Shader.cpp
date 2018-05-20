#include "Shader.h"


// ------------------------------------------------------------------------------------------------------------ //

Shader::Shader() {
	
}

// ------------------------------------------------------------------------------------------------------------ //

Shader::Shader(string path){
	this->Load(path);
}

// ------------------------------------------------------------------------------------------------------------ //

Shader::Shader(File * file){
	this->Load(file);
}

// ------------------------------------------------------------------------------------------------------------ //

Shader::~Shader() {
	if (this->pipelineLayout != VK_NULL_HANDLE && this->graphicsPipeline != VK_NULL_HANDLE && gf_context != nullptr) {
		vkDestroyPipelineLayout(this->gf_context->GetDevice(), this->pipelineLayout, VK_ALLOCK);
		this->pipelineLayout = VK_NULL_HANDLE;
		vkDestroyPipeline(this->gf_context->GetDevice(), this->graphicsPipeline, VK_ALLOCK);
		this->graphicsPipeline = VK_NULL_HANDLE;
	}

	for (size_t i = 0; i < this->modules.size(); i++) {
		vkDestroyShaderModule(this->gf_context->GetDevice(), this->modules[i], VK_ALLOCK);
	}
}

// ------------------------------------------------------------------------------------------------------------ //

void Shader::Load(string file){
	ifstream f(file.c_str(), ios::binary);
	this->name = file.erase(0, file.find_first_of("/")+1);
	this->name = file.erase(0, file.find_first_of("\\")+1);
	if (f.good()) {
		streampos fsize = 0;
		fsize = f.tellg();
		f.seekg(0, ios::end);
		fsize = f.tellg() - fsize;
		f.seekg(0, ios::beg);

		uint8_t type;
		f.read((char*)&type, 1);

		uint8_t num;
		f.read((char*)&num, 1);

		for (uint8_t i = 0; i < num; i++){
			uint8_t t;
			f.read((char*)&t, 1);

			uint32_t len;
			f.read((char*)&len, 4);

			vector<char> buffer(len);
			f.read(buffer.data(), len);

			this->shaders.push_back({
				t,
				len,
				buffer
			});
		}

	}
	f.close();
}

// ------------------------------------------------------------------------------------------------------------ //

void Shader::Load(File * file){
	/*file->GetData(&this->shader_context, &this->shader_context_size);
	this->shader_context[this->shader_context_size] = '\0';

	string filename = file->GetName();

	this->name = filename.erase(0, filename.find_first_of("/") + 1);
	this->name = filename.erase(0, filename.find_first_of("\\") + 1);

	string mdid = this->shader_context + name;
	MD5 md5;
	this->id = md5.digestString(_strdup(mdid.c_str()));

	this->InitShader();*/
}

// ------------------------------------------------------------------------------------------------------------ //

void Shader::Load(char * d, unsigned int s){
	/*this->shader_context = d;
	this->shader_context_size = s;

	string mdid = this->shader_context;
	MD5 md5;
	this->id = md5.digestString(_strdup(mdid.c_str()));

	this->InitShader();*/
}

// ------------------------------------------------------------------------------------------------------------ //

void Shader::Init(GFContext * c){
	this->gf_context = c;

	this->modules.resize(this->shaders.size());
	this->shaderStages.resize(this->shaders.size());
	for (size_t i = 0; i < this->shaders.size(); i++){
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = this->shaders[i].size;
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(this->shaders[i].context.data());
		VkCheck(vkCreateShaderModule(c->GetDevice(), &shaderModuleCreateInfo, VK_ALLOCK, &this->modules[i]));

		VkShaderStageFlagBits stageFlagBits;

		switch (this->shaders[i].type){

			case 0: {
				stageFlagBits = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			}

			case 1: {
				stageFlagBits = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
				break;
			}

			case 2: {
				stageFlagBits = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
				break;
			}

			case 3: {
				stageFlagBits = VK_SHADER_STAGE_GEOMETRY_BIT;
				break;
			}

			case 4: {
				stageFlagBits = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			}

			case 5: {
				stageFlagBits = VK_SHADER_STAGE_COMPUTE_BIT;
				break;
			}

			default: {
				VkDisplayError("Invalid shader type");
				break;
			}
		}

		this->shaderStages[i] = {
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			stageFlagBits,
			this->modules[i],
			"main",
			nullptr
		};
	}


	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		c->vertexInputBindingDescription.size(),
		c->vertexInputBindingDescription.data(),
		c->vertexInputAttributeDescription.size(),
		c->vertexInputAttributeDescription.data(),
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkViewport viewport = {};
	VkRect2D scissor = {};


	VkPipelineViewportStateCreateInfo viewportState = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		&viewport,
		1,
		&scissor
	};


	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
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

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		c->descriptorSetLayouts.size(),
		c->descriptorSetLayouts.data(),
		0,
		nullptr
	};
	VkCheck(vkCreatePipelineLayout(c->GetDevice(), &pipelineLayoutInfo, VK_ALLOCK, &this->pipelineLayout));

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
	pipelineInfo.stageCount = this->shaderStages.size();
	pipelineInfo.pStages = this->shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = this->pipelineLayout;
	pipelineInfo.renderPass = c->GetRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VkCheck(vkCreateGraphicsPipelines(c->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, VK_ALLOCK, &this->graphicsPipeline));
}

// ------------------------------------------------------------------------------------------------------------ //

VkPipeline Shader::GetPipeline(){
	return this->graphicsPipeline;
}

// ------------------------------------------------------------------------------------------------------------ //

VkPipelineLayout Shader::GetPipelineLayout(){
	return this->pipelineLayout;
}

// ------------------------------------------------------------------------------------------------------------ //

void Shader::Read(ifstream * f){


	/*unsigned int name_size;
	f->read((char*)&name_size, 4);
	char* tmp_name = new char[name_size];
	f->read(tmp_name, name_size);
	tmp_name[name_size] = '\0';

	f->read(this->id, 32);
	this->id[32] = '\0';

	f->read((char*)&this->shader_context_size, 4);
	this->shader_context = new char[this->shader_context_size];
	f->read(this->shader_context, this->shader_context_size);
	this->shader_context[this->shader_context_size] = '\0';

	this->InitShader();*/
}

// ------------------------------------------------------------------------------------------------------------ //

void Shader::Write(ofstream * f){
	/*unsigned int name_size = this->name.size();
	char* name_tmp = _strdup(this->name.c_str());
	f->write((char*)&name_size, 4);
	f->write(name_tmp, name_size);

	f->write(this->id, 32);

	f->write((char*)&this->shader_context_size, 4);
	f->write(this->shader_context, this->shader_context_size);*/
}
