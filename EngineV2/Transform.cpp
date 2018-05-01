#include "Transform.h"


Transform::Transform(vec3 pos, vec3 rot, vec3 sca) {
	this->position = pos;
	this->rotation = rot;
	this->scale = sca;
}

// -------------------------------------------------------------------------------------------- //

Transform::Transform(ifstream * f){
	this->Read(f);
}

// -------------------------------------------------------------------------------------------- //

Transform::Transform() {
	this->position = vec3(0);
	this->rotation = vec3(0);
	this->scale = vec3(1);
}

// -------------------------------------------------------------------------------------------- //

Transform::~Transform() { }

// -------------------------------------------------------------------------------------------- //

mat4 Transform::GetMatrix() {
	mat4 translateMatrix = translate(position);

	mat4 rotX = rotate(glm::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	mat4 rotY = rotate(glm::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	mat4 rotZ = rotate(glm::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
	mat4 rotationMatrix = rotX * rotY * rotZ;

	mat4 scaleMatrix = glm::scale(scale);

	return translateMatrix * rotationMatrix * scaleMatrix;
}

// -------------------------------------------------------------------------------------------- //

void Transform::Read(ifstream * f){
	f->read((char*)&this->position, sizeof(vec3));
	f->read((char*)&this->rotation, sizeof(vec3));
	f->read((char*)&this->scale, sizeof(vec3));
}

// -------------------------------------------------------------------------------------------- //

void Transform::Write(ofstream * f){
	f->write((char*)&this->position, sizeof(vec3));
	f->write((char*)&this->rotation, sizeof(vec3));
	f->write((char*)&this->scale, sizeof(vec3));
}
