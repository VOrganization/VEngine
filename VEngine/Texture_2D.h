#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <FreeImage.h>
#include "GFContext.h"

using namespace std;

#include "File.h"

void loadImageFI(string path, int& width, int& height, BYTE** bits);
void loadImageFI(File* file, int& width, int& height, BYTE** bits);

class Texture_2D {

	//GLuint texture = 0;
	File* tex_file = nullptr;

	void Load();

public:
	Texture_2D();
	Texture_2D(string path);
	Texture_2D(File* file);
	Texture_2D(ifstream* f);
	~Texture_2D();

	string id;

	unsigned int GetIndex();
	unsigned int Use();


	void Read(ifstream* f);
	void Write(ofstream* f, bool all_inside);
};

