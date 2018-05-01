#pragma once

#include "Object.h"

class ObjectLightPoint : public Object{

public:
	ObjectLightPoint();
	~ObjectLightPoint();
	

	vec3 diffuse;
	vec3 ambient;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	void Read(ifstream* f);
	void Write(ofstream* f);
};

