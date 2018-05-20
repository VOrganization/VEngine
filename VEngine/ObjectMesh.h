#pragma once

#include "Object.h"
#include "Mesh.h"
#include "Material.h"
#include "Armature.h"

#include <iostream>
using namespace std;

class ObjectMesh : public Object {

public:
	ObjectMesh();
	ObjectMesh(Mesh* m);
	ObjectMesh(ifstream* f);
	~ObjectMesh();

	Mesh* mesh;
	Material* material;
	Armature* armature;

	void Draw();


	void Read(ifstream* f);
	void Write(ofstream* f);
};
