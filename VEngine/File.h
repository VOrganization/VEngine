#pragma once
#include <string>
#include <fstream>
#include <iostream> //dla testow

using namespace std;


#include "md5.h"

enum FileType{
	FT_UNKNOWN,
	FT_ANIMATION,
	FT_IMAGE,
	FT_MODEL,
	FT_MATERIAL,
	FT_SHADER,
	FT_SCENE,
	FT_SCRIPT,
	FT_AUDIO
};

unsigned int CalcTypeFile(string path);

string BaseName(string path);

class File{

	string Name;

	char* Data;
	unsigned int DataSize;

	unsigned int Type;

	bool Inside;

	char* Checksum;

	bool Autoload;

	bool IsLoad;
public:
	File();
	File(string path);
	File(ifstream* f);
	~File();

	void Create(string path, bool inside);

	void SetAutoLoad(bool d);
	bool GetAutoLoad();

	void SetType(unsigned int type);
	unsigned int GetType();

	void SetInside(bool in);
	bool GetInside();

	void SetName(string name);
	string GetName();

	char* GetChecksum();

	void Load();

	bool Check();

	void GetData(char** data, unsigned int* size);


	void Read(ifstream* f);
	void Write(ofstream* f);

};

