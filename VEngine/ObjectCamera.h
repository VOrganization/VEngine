#pragma once

#include "Object.h"

class ObjectCamera : public Object{

public:
	ObjectCamera();
	~ObjectCamera();

	bool Default;

	vec3 Up;

	bool UseDirection;
	vec3 Direction;

	float Fov;
	float Near;
	float Far;
	float Zoom;

	bool Perpective;

	void Read(ifstream* f);
	void Write(ofstream* f);
};

