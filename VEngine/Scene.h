#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "GFContext.h"

using namespace glm;
using namespace std;

#include "File.h"
#include "Object.h"
#include "Model.h"

#include "ObjectLightPoint.h"
#include "ObjectLightSpot.h"
#include "ObjectLightDir.h"
#include "ObjectCamera.h"

#include "Script.h"

class Scene{

	void Init();

	void InitShader(Shader* s);
	void InitMaterial(Material* m);
	void LoadFile(File* file);

	void ObjWrite(Object* obj, ofstream* f);
	Object* ObjRead(ifstream* f, Object* p);

public:
	Scene();
	Scene(string path);
	virtual ~Scene();

	void Load(string path);
	void Save(string path);
	bool load = false;

	vec3 clearColor;

	Shader* default_shader;
	Material* default_material;

	Transform* WorldTransform;

	ObjectCamera* defaultCamera = nullptr;

	vector<File*>* Files;
	vector<Model*>* Models;
	vector<Mesh*>* Meshes;
	vector<Animation*>* Animations;
	vector<Armature*>* Armatures;
	vector<Shader*>* Shaders;
	vector<Texture_2D*>* Textures_2D;
	vector<Material*>* Materials;
	vector<Object*>* Objects;
	vector<Script*>* Scripts;

};

