#include "ObjectCamera.h"



ObjectCamera::ObjectCamera(){
	this->type = OT_CAMERA;
	this->Default = false;
	this->UseDirection = false;
	this->Up = vec3(0.0f, 1.0f, 0.0f);
	this->Direction = vec3(0, 0, -1);

	this->Fov = 70.0f;
	this->Far = 1000.0f;
	this->Near = 0.1f;

	this->Perpective = true;
}

// -------------------------------------------------------------------------------------------- //

ObjectCamera::~ObjectCamera(){ }

// -------------------------------------------------------------------------------------------- //

void ObjectCamera::Read(ifstream * f) {
	unsigned int name_size;
	f->read((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = new char[name_size];
	f->read(tmp_name, name_size);
	tmp_name[name_size] = '\0';
	this->name = tmp_name;

	f->read((char*)&this->display_type, 4);
	f->read((char*)&this->display_priority, 4);

	this->transform->Read(f);

	f->read((char*)&this->Default, 1);
	f->read((char*)&this->UseDirection, 1);
	f->read((char*)&this->Perpective, 1);
	f->read((char*)&this->Direction, sizeof(vec3));
	f->read((char*)&this->Up, sizeof(vec3));
	f->read((char*)&this->Fov, sizeof(float));
	f->read((char*)&this->Far, sizeof(float));
	f->read((char*)&this->Near, sizeof(float));
}

// -------------------------------------------------------------------------------------------- //

void ObjectCamera::Write(ofstream * f) {
	unsigned int name_size = this->name.size();
	f->write((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = _strdup(this->name.c_str());
	f->write(tmp_name, name_size);
	this->transform->Write(f);

	f->write((char*)&this->Default, 1);
	f->write((char*)&this->UseDirection, 1);
	f->write((char*)&this->Perpective, 1);
	f->write((char*)&this->Direction, sizeof(vec3));
	f->write((char*)&this->Up, sizeof(vec3));
	f->write((char*)&this->Fov, sizeof(float));
	f->write((char*)&this->Far, sizeof(float));
	f->write((char*)&this->Near, sizeof(float));
}
