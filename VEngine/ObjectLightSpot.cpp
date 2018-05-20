#include "ObjectLightSpot.h"



ObjectLightSpot::ObjectLightSpot(){
	this->type = OT_LIGHT_SPOT;
}

// -------------------------------------------------------------------------------------------- //

ObjectLightSpot::~ObjectLightSpot(){ }

// -------------------------------------------------------------------------------------------- //

void ObjectLightSpot::Read(ifstream * f){
	((Object*)this)->Read(f);

	f->read((char*)&this->ambient, sizeof(vec3));
	f->read((char*)&this->diffuse, sizeof(vec3));
	f->read((char*)&this->specular, sizeof(vec3));

	f->read((char*)&this->intensity, sizeof(float));

	f->read((char*)&this->constant, sizeof(float));
	f->read((char*)&this->linear, sizeof(float));
	f->read((char*)&this->quadratic, sizeof(float));

	f->read((char*)&this->cutOff, sizeof(float));
	f->read((char*)&this->outerCutOff, sizeof(float));
}

// -------------------------------------------------------------------------------------------- //

void ObjectLightSpot::Write(ofstream * f){
	unsigned int name_size = this->name.size();
	f->write((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = _strdup(this->name.c_str());
	f->write(tmp_name, name_size);
	this->transform->Write(f);

	f->write((char*)&this->ambient, sizeof(vec3));
	f->write((char*)&this->diffuse, sizeof(vec3));
	f->write((char*)&this->specular, sizeof(vec3));

	f->write((char*)&this->constant, sizeof(float));
	f->write((char*)&this->linear, sizeof(float));
	f->write((char*)&this->quadratic, sizeof(float));

	f->write((char*)&this->cutOff, sizeof(float));
	f->write((char*)&this->outerCutOff, sizeof(float));
}
