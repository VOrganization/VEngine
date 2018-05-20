#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Transform.h"

class KeyFrame{

	Transform* transform;
	double time;

public:
	KeyFrame();
	KeyFrame(ifstream* f);
	KeyFrame(double t, Transform* tran);
	~KeyFrame();

	double GetTime();
	void SetTime(double t);

	Transform* GetTransform();
	void SetTransform(Transform* tran);

	void Read(ifstream* f);
	void Write(ofstream* f);
};

