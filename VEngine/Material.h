#pragma once

#include "Object.h"
#include "Shader.h"

#include "Texture_2D.h"

class Material{

	bool load = false;

public:
	Material();
	Material(File* file);
	Material(string path);
	Material(ifstream* f);
	virtual ~Material();

	bool IsLoad();

	unsigned int type;

	string name;
	string id;
	uint32_t engine_id;

	float opacity;

	vec4 ambient;

	vec4 diffuse;
	Texture_2D* diffuseMap;

	vec4 specular;
	float shininess;
	Texture_2D* specularMap;

	vec4 emission;
	Texture_2D* emissionMap;

	float metalic;
	Texture_2D* metalicMap;

	float roughness;
	Texture_2D* roughnessMap;

	Texture_2D* bumpMap;
	float bumpScale;

	Texture_2D* normalMap;

	Texture_2D* aoMap;
	float aoIntensity;

	float reflection;
	float refraction;
	bool dynamicCube;

	vector<Texture_2D*> othersMap;
	vector<vec4> othersColor;
	vector<float> othersValues;

	Shader* shader = nullptr;

	void Save(string path, bool all_inside);
	void Read(ifstream* f);
	void Write(ofstream* f);
};

