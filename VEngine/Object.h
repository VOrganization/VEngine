#pragma once
#include <string>
#include <vector>
#include <fstream>
using namespace std;

#include "Animation.h"

#include "Transform.h"
#include "md5.h"

enum ObjectType{
	OT_NULL,
	OT_MESH,
	OT_LIGHT_POINT,
	OT_LIGHT_SPOT,
	OT_LIGHT_DIR,
	OT_CAMERA,
	OT_SIZE
};

class Object{

	void Init();

public:
	Object();
	Object(ifstream* f);
	virtual ~Object();
	
	unsigned int type = OT_NULL;

	string name;
	Transform* transform;
	Transform* tmp_transform;
	
	unsigned int display_type;
	unsigned int display_priority;

	Object* parent;
	vector<Object*>* children;

	void Draw();

	void Read(ifstream* f);
	void Write(ofstream* f);
};

