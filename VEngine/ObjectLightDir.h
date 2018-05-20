#pragma once
#include "Object.h"
class ObjectLightDir : public Object{
public:
	ObjectLightDir();
	~ObjectLightDir();

	vec3 diffuse;
	vec3 ambient;
	vec3 specular;

	float intensity;

	void Read(ifstream* f);
	void Write(ofstream* f);
};

