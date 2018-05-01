#pragma once
//#include <lua.hpp>

#include <iostream>
#include <fstream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

#include "File.h"


typedef struct {
	string name;
	unsigned int code;
	void* data;
} VAR;


template <typename T>
inline VAR CreateVar(string name, T d) {
	VAR var;
	var.name = name;
	var.code = typeid(d).hash_code();
	var.data = (void*)d;
	return var;
}

class Script{

	//typedef void(*script_fun)(Script*, lua_State*);

	/*script_fun lua_init_fun = nullptr;
	script_fun lua_update_fun = nullptr;

	lua_State* state = nullptr;*/

	void CheckLoad(int r);
	void CallFunction(string name);

public:
	Script();
	//Script(script_fun fun0, script_fun fun1);
	~Script();

	vector<VAR*> LocalVars;

	char* context = nullptr;
	unsigned int context_size = 0;

	//lua_State* GetState();

	void Load();
	void Load(File* file);
	void Load(string path);

	void Start();
	void Update();

	void Read(ifstream* f);
	void Write(ofstream* f);
};

