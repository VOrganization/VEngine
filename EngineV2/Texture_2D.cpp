#include "Texture_2D.h"



Texture_2D::Texture_2D() { }

// -------------------------------------------------------------------------------------------- //

Texture_2D::Texture_2D(string path) {
	/*int width, height;
	BYTE* bits = 0;
	loadImageFI(path, width, height, &bits);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)bits);
	delete[]bits;

	glGenerateMipmap(GL_TEXTURE_2D);*/
}

// -------------------------------------------------------------------------------------------- //

Texture_2D::Texture_2D(File* file) {
	/*this->tex_file = file;

	int width, height;
	BYTE* bits = 0;
	loadImageFI(file, width, height, &bits);

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)bits);
	delete[]bits;

	glGenerateMipmap(GL_TEXTURE_2D);

	MD5 md5;
	string tmp_id = BaseName(file->GetName()) + to_string(width) + to_string(height);
	this->id = md5.digestString(_strdup(tmp_id.c_str()));*/
}

// -------------------------------------------------------------------------------------------- //

Texture_2D::Texture_2D(ifstream * f){
	this->Read(f);
}

// -------------------------------------------------------------------------------------------- //

Texture_2D::~Texture_2D() {
	// glDeleteTextures(1, &this->texture);
}

// -------------------------------------------------------------------------------------------- //

unsigned int Texture_2D::GetIndex() {
	// return this->texture;
	return 0;
}

// -------------------------------------------------------------------------------------------- //

unsigned int Texture_2D::Use() {
	/*glActiveTexture(GL_TEXTURE0 + this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	return this->texture;*/
	return 0;
}

// -------------------------------------------------------------------------------------------- //

void Texture_2D::Read(ifstream * f){
	bool isid;
	f->read((char*)&isid, 1);

	if (isid) {
		char* _id = new char[32];
		f->read(_id, 32);
		_id[32] = '\0';
		this->id = _id;
	}
	else {

	}

	/*if (control == 'O') {
		this->tex_file = new File();
		this->tex_file->Read(f);
	}
	else {
		f->read(this->id, 32);
		this->id[32] = '\0';
	}*/

}

// -------------------------------------------------------------------------------------------- //

void Texture_2D::Write(ofstream * f, bool all_inside){
	if (this->tex_file != nullptr) {
		f->write("O", 1);
		if (all_inside) {
			bool in = this->tex_file->GetInside();
			bool al = this->tex_file->GetAutoLoad();
			this->tex_file->SetInside(true);
			this->tex_file->SetAutoLoad(true);
			this->tex_file->Write(f);
			this->tex_file->SetInside(in);
			this->tex_file->SetAutoLoad(al);
		}
		else
			this->tex_file->Write(f);
	}
	else {
		f->write("N", 1);
		f->write(_strdup(this->id.c_str()), 32);
	}
}

// -------------------------------------------------------------------------------------------- //

void loadImageFI(string path, int & width, int & height, BYTE ** bits) {
	ifstream f(path.c_str());
	if (!f.good())
		return;

	FREE_IMAGE_FORMAT image_format = FIF_UNKNOWN;
	FIBITMAP* image_ptr = 0;

	image_format = FreeImage_GetFileType(path.c_str(), 0);
	if (image_format == FIF_UNKNOWN)
		image_format = FreeImage_GetFIFFromFilename(path.c_str());

	if (FreeImage_FIFSupportsReading(image_format))
		image_ptr = FreeImage_Load(image_format, path.c_str());

	FIBITMAP* bitmap = FreeImage_ConvertTo32Bits(image_ptr);
	width = FreeImage_GetWidth(bitmap);
	height = FreeImage_GetHeight(bitmap);

	(*bits) = new BYTE[4 * width*height];
	char* pixeles = (char*)FreeImage_GetBits(bitmap);

	for (int j = 0; j<width*height; j++) {
		(*bits)[j * 4 + 0] = pixeles[j * 4 + 2];
		(*bits)[j * 4 + 1] = pixeles[j * 4 + 1];
		(*bits)[j * 4 + 2] = pixeles[j * 4 + 0];
		(*bits)[j * 4 + 3] = pixeles[j * 4 + 3];
	}

	FreeImage_Unload(bitmap);
	FreeImage_Unload(image_ptr);
}

// -------------------------------------------------------------------------------------------- //

void loadImageFI(File* file, int & width, int & height, BYTE ** bits) {
	FREE_IMAGE_FORMAT image_format = FIF_UNKNOWN;
	FIBITMAP* image_ptr = 0;

	char* data;
	unsigned int size;
	file->GetData(&data, &size);

	FIMEMORY* mem = FreeImage_OpenMemory((BYTE*)data, size);
	image_format = FreeImage_GetFileTypeFromMemory(mem);

	if (image_format == FIF_UNKNOWN)
		image_format = FreeImage_GetFIFFromFilename(file->GetName().c_str());

	if (FreeImage_FIFSupportsReading(image_format))
		image_ptr = FreeImage_LoadFromMemory(image_format, mem);

	FIBITMAP* bitmap = FreeImage_ConvertTo32Bits(image_ptr);
	width = FreeImage_GetWidth(bitmap);
	height = FreeImage_GetHeight(bitmap);

	(*bits) = new BYTE[4 * width*height];
	char* pixeles = (char*)FreeImage_GetBits(bitmap);

	for (int j = 0; j<width*height; j++) {
		(*bits)[j * 4 + 0] = pixeles[j * 4 + 2];
		(*bits)[j * 4 + 1] = pixeles[j * 4 + 1];
		(*bits)[j * 4 + 2] = pixeles[j * 4 + 0];
		(*bits)[j * 4 + 3] = pixeles[j * 4 + 3];
	}

	FreeImage_Unload(bitmap);
	FreeImage_Unload(image_ptr);
}
