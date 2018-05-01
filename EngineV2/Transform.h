#pragma once
#include <fstream>
using namespace std;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace glm;


class Transform {

public:
	Transform();
	Transform(vec3 pos, vec3 rot, vec3 sca);
	Transform(ifstream* f);
	virtual ~Transform();

	vec3 position;
	vec3 rotation;
	vec3 scale;

	mat4 GetMatrix();

	void Read(ifstream* f);
	void Write(ofstream* f);
};

