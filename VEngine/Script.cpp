#include "Script.h"



void Script::CheckLoad(int r){
	/*switch (r)
	{
	case 0:
		if (lua_pcall(this->state, 0, 0, 0) != 0) {
			cout << lua_tostring(this->state, -1) << endl;
			lua_pop(this->state, 1);
			this->state = 0;
		}
		break;
	case LUA_ERRFILE:
		cout << "Cannot find / open lua script\nSkipping Lua init.\n";
		this->state = 0;
		break;
	case LUA_ERRSYNTAX:
		cout << "Syntax error during pre-compilation of script\nSkipping Lua init.\n";
		if (lua_pcall(this->state, 0, 0, 0) != 0) {
			cout << lua_tostring(this->state, -1) << endl;
			lua_pop(this->state, 1);
			this->state = 0;
		}
		break;
	case LUA_ERRMEM:
		cout << "Fatal memory allocation error during processing of script\n";
		this->state = 0;
		break;
	}*/
}

// -------------------------------------------------------------------------------------------- //

void Script::CallFunction(string name){
	/*if (this->state) {
		lua_getglobal(this->state, name.c_str());
		if (lua_pcall(this->state, 0, 0, 0) != 0) {
			cout << lua_tostring(this->state, -1) << endl;
			lua_pop(this->state, 1);
			this->state = 0;
		}
		if (this->lua_update_fun != nullptr)
			this->lua_update_fun(this, this->state);
	}*/
}

// -------------------------------------------------------------------------------------------- //

Script::Script(){
	/*this->state = luaL_newstate();
	if (this->state)
		luaL_openlibs(this->state);*/
}

//Script::Script(script_fun fun0, script_fun fun1){
//	this->lua_init_fun = fun0;
//	this->lua_update_fun = fun1;
//	this->state = luaL_newstate();
//	if (this->state)
//		luaL_openlibs(this->state);
//}

// -------------------------------------------------------------------------------------------- //

Script::~Script(){
	/*if (this->state)
		lua_close(this->state);*/
}

// -------------------------------------------------------------------------------------------- //

//lua_State * Script::GetState(){
//	return this->state;
//}

void Script::Load(){
	//if (this->lua_init_fun != nullptr && this->state != 0)
	//	this->lua_init_fun(this, this->state);

	/*int result = luaL_loadstring(this->state, (const char*)this->context);
	this->CheckLoad(result);*/
}

// -------------------------------------------------------------------------------------------- //

void Script::Load(File * file){
	/*if (this->lua_init_fun != nullptr && this->state != 0)
		this->lua_init_fun(this, this->state);

	file->GetData(&this->context, &this->context_size);
	int result = luaL_loadstring(this->state, this->context);
	this->CheckLoad(result);*/
}

// -------------------------------------------------------------------------------------------- //

void Script::Load(string path){
	/*if (this->lua_init_fun != nullptr && this->state != 0)
		this->lua_init_fun(this, this->state);

	int result = luaL_loadfile(this->state, path.c_str());
	this->CheckLoad(result);*/
}

// -------------------------------------------------------------------------------------------- //

void Script::Start(){
	this->CallFunction("Start");
}

// -------------------------------------------------------------------------------------------- //

void Script::Update(){
	this->CallFunction("Update");
}

// -------------------------------------------------------------------------------------------- //

void Script::Read(ifstream * f){

}

// -------------------------------------------------------------------------------------------- //

void Script::Write(ofstream * f){

}
