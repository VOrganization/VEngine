#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

#include "GFContext.h"

using namespace glm;

#include "Object.h"

typedef struct{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	/*ivec4 boneIndex;
	vec4 boneWeight;*/
} Vertex;

class Mesh{
public:

	GFContext* context = nullptr;

	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

public:
	Mesh();
	virtual ~Mesh();

	string id;

	bool IsLoad();

	vector<Vertex> vertices;
	vector<uint16_t> index;

	void Init(GFContext* c);

	void Draw(VkCommandBuffer commandBuffer);

	void Read(ifstream* f);
	void Write(ofstream* f);
};

