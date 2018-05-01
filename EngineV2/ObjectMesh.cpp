#include "ObjectMesh.h"



ObjectMesh::ObjectMesh(){
	this->type = OT_MESH;
}

// -------------------------------------------------------------------------------------------- //

ObjectMesh::ObjectMesh(Mesh * m){
	this->type = OT_MESH;
	this->mesh = m;
}

// -------------------------------------------------------------------------------------------- //

ObjectMesh::ObjectMesh(ifstream * f){
	this->type = OT_MESH;
	this->Read(f);
}

// -------------------------------------------------------------------------------------------- //

ObjectMesh::~ObjectMesh(){ }

// -------------------------------------------------------------------------------------------- //

void ObjectMesh::Draw() {
	/*if (this->mesh != nullptr)
		this->mesh->Draw();*/
}

// -------------------------------------------------------------------------------------------- //

void ObjectMesh::Read(ifstream * f){
	unsigned int name_size;
	f->read((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = new char[name_size];
	f->read(tmp_name, name_size);
	tmp_name[name_size] = '\0';
	this->name = tmp_name;

	f->read((char*)&this->display_type, 4);
	f->read((char*)&this->display_priority, 4);

	this->transform->Read(f);

	char control;

	f->read(&control, 1);
	if (control == 'O') {
		this->armature = new Armature(f);
	}

	f->read(&control, 1);
	if (control == 'O') {
		this->mesh = new Mesh();
		this->mesh->Read(f);
	}

	f->read(&control, 1);
	if (control == 'O') {
		this->material = new Material(f);
	}

}

// -------------------------------------------------------------------------------------------- //

void ObjectMesh::Write(ofstream * f){
	//f->write((char*)&this->type, sizeof(unsigned int));
	unsigned int name_size = this->name.size();
	f->write((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = _strdup(this->name.c_str());
	f->write(tmp_name, name_size);
	this->transform->Write(f);

	f->write((this->armature != nullptr ? "O" : "N"), 1);
	if (this->armature != nullptr)
	this->armature->Write(f);

	f->write((this->mesh != nullptr ? "O" : "N"), 1);
	if (this->mesh != nullptr)
		this->mesh->Write(f);

	f->write((this->material != nullptr ? "O" : "N"), 1);
	if (this->material != nullptr)
		this->material->Write(f);
}
