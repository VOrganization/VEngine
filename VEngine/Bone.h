#pragma once

#include <vector>
#include "Transform.h"

class Bone{

public:
	Bone();
	Bone(string n, Transform tran);
	~Bone();

	Transform transform;
	string name;

	mat4 xfrom;
	mat4 comb;

	unsigned int index;

	vector<Bone*> Bones;

	void Read(ifstream* f);
	void Write(ofstream* f);
};

