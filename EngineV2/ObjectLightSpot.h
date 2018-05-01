#pragma once
#include "Object.h"
class ObjectLightSpot : public Object {
public:
	ObjectLightSpot();
	~ObjectLightSpot();

	vec3 diffuse;
	vec3 ambient;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;

	void Read(ifstream* f);
	void Write(ofstream* f);
};

