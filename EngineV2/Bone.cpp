#include "Bone.h"



Bone::Bone()
{
}

Bone::Bone(string n, Transform tran){
	this->name = n;
	this->transform = tran;
}


Bone::~Bone()
{
}

void Bone::Read(ifstream * f){
	unsigned int name_size;
	f->read((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = new char[name_size];
	f->read(tmp_name, name_size);
	tmp_name[name_size] = '\0';
	this->name = tmp_name;
	this->transform.Read(f);

	f->read((char*)&this->index, 4);

	unsigned int bone_size = 0;
	f->read((char*)(&bone_size), 4);
	for (unsigned int i = 0; i < bone_size; i++) {
		Bone* b = new Bone();
		b->Read(f);
		this->Bones.push_back(b);
	}

}

void Bone::Write(ofstream * f){
	unsigned int name_size = this->name.size();
	f->write((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = _strdup(this->name.c_str());
	f->write(tmp_name, name_size);
	this->transform.Write(f);

	f->write((char*)&this->index, 4);

	unsigned int bone_size = this->Bones.size();
	f->write((char*)(&bone_size), 4);
	for (unsigned int i = 0; i < bone_size; i++)
		this->Bones[i]->Write(f);

}
