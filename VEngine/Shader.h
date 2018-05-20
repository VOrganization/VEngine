#pragma once
#include "GFContext.h"
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

#include "File.h"

typedef struct {
	uint8_t type;
	uint32_t size;
	vector<char> context;
} ShaderContextInfo;

class Shader {

	GFContext* gf_context;
	
	vector<VkShaderModule> modules;
	vector<VkPipelineShaderStageCreateInfo> shaderStages;
	vector<ShaderContextInfo> shaders;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

public:
	Shader();
	Shader(string path);
	Shader(File* file);
	virtual ~Shader();

	string id;
	string name;

	void Load(string file);
	void Load(File* file);
	void Load(char* d, unsigned int s);

	void Init(GFContext* context);

	VkPipeline GetPipeline();
	VkPipelineLayout GetPipelineLayout();

	void Read(ifstream* f);
	void Write(ofstream* f);
};

