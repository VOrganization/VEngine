#include "File.h"


File::File(){
	this->DataSize = 0;
	this->Data = nullptr;
	this->Checksum = new char[32];
	this->Type = 0;
	this->Inside = false;
	this->Autoload = false;
	this->Inside = false;
}

// -------------------------------------------------------------------------------------------- //

File::File(string path){
	this->DataSize = 0;
	this->Data = nullptr;
	this->Checksum = new char[32];
	this->Type = 0;
	this->Inside = false;
	this->Autoload = true;
	this->Inside = false;

	this->Create(path, false);
	this->Load();

}

// -------------------------------------------------------------------------------------------- //

File::File(ifstream * f){
	this->Read(f);
}

// -------------------------------------------------------------------------------------------- //

File::~File(){
	if (this->IsLoad && this->Data != nullptr) {
		//delete[] this->Data;
		//delete this->Checksum;
	}
}

// -------------------------------------------------------------------------------------------- //

void File::Create(string path, bool inside){
	this->Type = CalcTypeFile(path);
	this->Name = path;
	this->Inside = inside;
	this->Type = CalcTypeFile(path);
	ifstream f(path.c_str(), ios::binary);
	if (f.good()) {
			
		streampos fsize = 0;
		fsize = f.tellg();
		f.seekg(0, ios::end);
		fsize = f.tellg() - fsize;
		f.seekg(0, ios::beg);

		unsigned int tmp_size = fsize;
		char* tmp_data = new char[tmp_size];
		f.read(tmp_data, tmp_size);
		tmp_data[tmp_size] = '\0';

		MD5 md5;
		string tmp = md5.digestMemory((BYTE*)tmp_data, tmp_size);
		this->Checksum = _strdup(tmp.c_str());

		if (this->Inside) {
			this->Data = tmp_data;
			this->DataSize = tmp_size;
			this->IsLoad = true;
		}
		//else
//			delete[] tmp_data;

		
	}
	else {
		if(this->Inside)
			this->Inside = false;
	}
	f.close();
}

// -------------------------------------------------------------------------------------------- //

void File::SetAutoLoad(bool d){
	this->Autoload = d;
}

// -------------------------------------------------------------------------------------------- //

bool File::GetAutoLoad(){
	return this->Autoload;
}

// -------------------------------------------------------------------------------------------- //

void File::SetType(unsigned int type){
	this->Type = type;
}

// -------------------------------------------------------------------------------------------- //

unsigned int File::GetType(){
	return this->Type;
}

// -------------------------------------------------------------------------------------------- //

void File::SetInside(bool in){
	this->Inside = in;
}

// -------------------------------------------------------------------------------------------- //

bool File::GetInside(){
	return this->Inside;
}

// -------------------------------------------------------------------------------------------- //

void File::SetName(string name){
	this->Name;
}

// -------------------------------------------------------------------------------------------- //

string File::GetName(){
	return this->Name;
}

// -------------------------------------------------------------------------------------------- //

char * File::GetChecksum(){
	return this->Checksum;
}

// -------------------------------------------------------------------------------------------- //

void File::Load(){
	if (this->IsLoad == true)
		return;

	ifstream f(this->Name.c_str(), ios::binary);
	if (!f.good())
		return;

	streampos fsize = 0;
	fsize = f.tellg();
	f.seekg(0, ios::end);
	fsize = f.tellg() - fsize;
	f.seekg(0, ios::beg);

	this->DataSize = fsize;
	this->Data = new char[this->DataSize];
	f.read(this->Data, this->DataSize);
	this->Data[this->DataSize] = '\0';

	f.close();

	this->IsLoad = true;
}

// -------------------------------------------------------------------------------------------- //

bool File::Check(){
	MD5 md5;
	string tmp = md5.digestMemory((BYTE*)this->Data, this->DataSize);

	if (tmp == string(this->Checksum))
		return true;

	return false;
}

// -------------------------------------------------------------------------------------------- //

void File::GetData(char ** data, unsigned int * size){
	(*data) = this->Data;
	(*size) = this->DataSize;
}

// -------------------------------------------------------------------------------------------- //

void File::Read(ifstream * f){
	//
	unsigned int name_szie;
	f->read((char*)&name_szie, sizeof(unsigned int));
	char* name = new char[name_szie];
	f->read(name, name_szie);
	name[name_szie] = '\0';
	this->Name = name;
	//
	f->read((char*)&this->Type, sizeof(unsigned int));
	f->read((char*)&this->Autoload, sizeof(bool));
	f->read((char*)&this->Inside, sizeof(bool));
	//
	if (this->Inside && this->Autoload) {
		f->read((char*)&this->DataSize, sizeof(unsigned int));
		this->Data = new char[this->DataSize];
		f->read(this->Data, this->DataSize);
	}
	//
	this->Checksum = new char[32];
	f->read(this->Checksum, 32);
	this->Checksum[32] = '\0';
}

// -------------------------------------------------------------------------------------------- //

void File::Write(ofstream * f){
	unsigned int name_size = this->Name.size();
	f->write((char*)&name_size, sizeof(unsigned int));
	f->write(this->Name.c_str(), name_size);
	f->write((char*)&this->Type, sizeof(unsigned int));
	f->write((char*)&this->Autoload, sizeof(bool));
	f->write((char*)&this->Inside, sizeof(bool));
	if (this->Inside) {
		f->write((char*)&this->DataSize, sizeof(unsigned int));
		f->write(this->Data, this->DataSize);
	}
	f->write(this->Checksum, 32);
}

// -------------------------------------------------------------------------------------------- //

#define CTF_FIND(s) (tmp_path.find(s) != string::npos)

unsigned int CalcTypeFile(string path){
	//wszystkie litery na mle
	string tmp_path = path;
	for (unsigned int i = 0; i < tmp_path.size(); i++)
		tmp_path[i] = tolower(tmp_path[i]);

	//sprawdzanie obslugiwanych typow

	//animation
	if (CTF_FIND(".anim"))
		return FT_ANIMATION;

	//image
	if (CTF_FIND(".bmp") || CTF_FIND(".png") || CTF_FIND(".jpg") || CTF_FIND(".tga") || CTF_FIND(".dds") || CTF_FIND(".psd") || CTF_FIND(".hdr") || CTF_FIND(".webp"))
		return FT_IMAGE;

	//model
	if (CTF_FIND(".obj") || CTF_FIND(".fbx") || CTF_FIND(".blend") || CTF_FIND(".dae") || CTF_FIND(".abc") || CTF_FIND(".3ds") || CTF_FIND(".ply") || CTF_FIND(".x3d") || CTF_FIND(".stl"))
		return FT_MODEL;

	//material
	if (CTF_FIND(".mat"))
		return FT_MATERIAL;

	//shader
	if (CTF_FIND(".glsl"))
		return FT_SHADER;

	//scene
	if (CTF_FIND(".scene"))
		return FT_SCENE;

	//script
	if (CTF_FIND(".lua"))
		return FT_SCRIPT;

	return FT_UNKNOWN;
}

string BaseName(string path){
	unsigned int pos = -1;
	if (path.find("\\") != string::npos) {
		pos = path.find_last_of("\\");
	}
	else if (path.find("/") != string::npos) {
		pos = path.find_last_of("/");
	}
	else {
		return path;
	}

	if (pos != -1) {
		string tmp = path;
		tmp = tmp.substr(0, pos);
		return tmp;
	}

	return path;
}
