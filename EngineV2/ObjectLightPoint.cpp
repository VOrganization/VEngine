#include "ObjectLightPoint.h"




ObjectLightPoint::ObjectLightPoint(){
	this->type = OT_LIGHT_POINT;
}

// -------------------------------------------------------------------------------------------- //

ObjectLightPoint::~ObjectLightPoint(){ }

// -------------------------------------------------------------------------------------------- //

void ObjectLightPoint::Read(ifstream * f) {
	unsigned int name_size;
	f->read((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = new char[name_size];
	f->read(tmp_name, name_size);
	tmp_name[name_size] = '\0';
	this->name = tmp_name;

	f->read((char*)&this->display_type, 4);
	f->read((char*)&this->display_priority, 4);

	this->transform->Read(f);

	f->read((char*)&this->ambient,  sizeof(vec3));
	f->read((char*)&this->diffuse,  sizeof(vec3));
	f->read((char*)&this->specular, sizeof(vec3));

	f->read((char*)&this->constant,  sizeof(float));
	f->read((char*)&this->linear,    sizeof(float));
	f->read((char*)&this->quadratic, sizeof(float));
}

// -------------------------------------------------------------------------------------------- //

void ObjectLightPoint::Write(ofstream * f) {
	unsigned int name_size = this->name.size();
	f->write((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = _strdup(this->name.c_str());
	f->write(tmp_name, name_size);
	this->transform->Write(f);

	f->write((char*)&this->ambient,  sizeof(vec3));
	f->write((char*)&this->diffuse,  sizeof(vec3));
	f->write((char*)&this->specular, sizeof(vec3));

	f->write((char*)&this->constant,  sizeof(float));
	f->write((char*)&this->linear,    sizeof(float));
	f->write((char*)&this->quadratic, sizeof(float));
}

