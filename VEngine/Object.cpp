#include "Object.h"



void Object::Init(){
	this->children = new vector<Object*>();
	this->transform = new Transform();
	this->tmp_transform = new Transform();
}

// -------------------------------------------------------------------------------------------- //

Object::Object(){
	this->Init();
}

// -------------------------------------------------------------------------------------------- //

Object::Object(ifstream * f){
	this->Init();
	this->Read(f);
}

// -------------------------------------------------------------------------------------------- //

Object::~Object(){
	for (unsigned int i = 0; i < this->children->size(); i++) {
		delete this->children->at(i);
		this->children->at(i) = NULL;
	}
	delete this->children;
	this->children = NULL;
	delete this->transform;
	delete this->tmp_transform;
}

// -------------------------------------------------------------------------------------------- //

void Object::Draw(){ }

// -------------------------------------------------------------------------------------------- //

void Object::Read(ifstream * f){
	unsigned int name_size;
	f->read((char*)&name_size, 4);
	char* tmp_name = new char[name_size];
	f->read(tmp_name, name_size);
	tmp_name[name_size] = '\0';
	this->name = tmp_name;

	f->read((char*)&this->display_type, 4);
	f->read((char*)&this->display_priority, 4);

	this->transform->Read(f);

	/*unsigned int a_s;
	f->read((char*)&a_s, 4);
	for (unsigned int i = 0; i < a_s; i++) {
		this->animations.push_back(new Animation(f));
	}*/
}

// -------------------------------------------------------------------------------------------- //

void Object::Write(ofstream * f){
	unsigned int name_size = this->name.size();
	f->write((char*)&name_size, 4);
	char* tmp_name = _strdup(this->name.c_str());
	f->write(tmp_name, name_size);
	
	this->transform->Write(f);

	/*unsigned int a_s = this->animations.size();
	f->write((char*)&a_s, 4);
	for (unsigned int i = 0; i < a_s; i++)
		this->animations[i]->Write(f);*/

}
