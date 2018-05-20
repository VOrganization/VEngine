#include "Material.h"



Material::Material(){
	this->emission = vec4(0);
	this->ambient = vec4(0.3, 0.3, 0.3, 1);
	this->diffuse = vec4(0.8, 0.8, 0.8, 1);
	this->specular = vec4(1.0, 1.0, 1.0, 1);
	this->shininess = 32.0f;
}

// ------------------------------------------------------------------------------------------------------------ //

Material::Material(string path){ }

// ------------------------------------------------------------------------------------------------------------ //

Material::Material(ifstream * f){
	this->Read(f);
}

// ------------------------------------------------------------------------------------------------------------ //

Material::~Material(){ }

// ------------------------------------------------------------------------------------------------------------ //

bool Material::IsLoad(){
	return this->load;
}

// ------------------------------------------------------------------------------------------------------------ //

void Material::Save(string path, bool all_inside){ }

// ------------------------------------------------------------------------------------------------------------ //

void Material::Read(ifstream * f){
	bool isid;
	f->read((char*)&isid, 1);
	if (isid) {
		char* _id = new char[32];
		f->read(_id, 32);
		_id[32] = '\0';
		this->id = _id;
		this->load = false;
	}
	else {
		f->read((char*)&this->type, 4);

		char cc;
		unsigned int name_size;
		f->read((char*)&name_size, 4);
		char* tmp_name = new char[name_size];
		f->read(tmp_name, name_size);
		tmp_name[name_size] = '\0';
		this->name = tmp_name;

		char* _id = new char[32];
		f->read(_id, 32);
		_id[32] = '\0';
		this->id = _id;

		f->read((char*)&this->opacity, 4);

		f->read((char*)&this->diffuse, sizeof(vec4));
		f->read(&cc, 1);
		if (cc == 'O') {
			this->diffuseMap = new Texture_2D(f);
		}

		if (this->type == 0) {
			f->read((char*)&this->specular, sizeof(vec4));
			f->read((char*)&this->shininess, 4);
			this->shininess = 32.0f;
			f->read(&cc, 1);
			if (cc == 'O') {
				this->specularMap = new Texture_2D(f);
			}
		}

		f->read((char*)&this->emission, sizeof(vec4));
		f->read(&cc, 1);
		if (cc == 'O') {
			this->emissionMap = new Texture_2D(f);
		}

		if (this->type == 1) {
			f->read((char*)&this->roughness, 4);
			f->read(&cc, 1);
			if (cc == 'O') {
				this->roughnessMap = new Texture_2D(f);
			}
			f->read((char*)&this->metalic, 4);
			f->read(&cc, 1);
			if (cc == 'O') {
				this->metalicMap = new Texture_2D(f);
			}
		}

		f->read(&cc, 1);
		if (cc == 'O') {
			this->bumpMap = new Texture_2D(f);
		}
		f->read((char*)&this->shininess, 4);

		f->read(&cc, 1);
		if (cc == 'O') {
			this->normalMap = new Texture_2D(f);
		}

		f->read(&cc, 1);
		if (cc == 'O') {
			this->aoMap = new Texture_2D(f);
		}
		f->read((char*)&this->aoIntensity, 4);

		f->read((char*)&this->reflection, 4);
		f->read((char*)&this->refraction, 4);
		f->read((char*)&this->dynamicCube, 1);

		unsigned int ss;
		f->read((char*)&ss, 4);
		f->read((char*)&ss, 4);
		f->read((char*)&ss, 4);

		this->load = false;
	}
}

// ------------------------------------------------------------------------------------------------------------ //

void Material::Write(ofstream * f){
	unsigned int name_size = this->name.size();
	char* name_tmp = _strdup(this->name.c_str());
	f->write((char*)&name_size, sizeof(unsigned int));
	f->write(name_tmp, name_size);

	f->write((char*)&this->ambient,  sizeof(vec4));
	f->write((char*)&this->diffuse,  sizeof(vec4));
	f->write((char*)&this->specular, sizeof(vec4));
	f->write((char*)&this->emission, sizeof(vec4));
	f->write((char*)&this->shininess, sizeof(float));

	if (shader != nullptr && this->shader->name != "default_engine_shader") {
		f->write("O", 1);
		this->shader->Write(f);
	}
	else
		f->write("N", 1);

	/*unsigned int text2d_size = this->textures.size();
	f->write((char*)&text2d_size, 4);
	for (size_t i = 0; i < text2d_size; i++){
		f->write(this->textures[i]->id, 32);
	}*/
}
