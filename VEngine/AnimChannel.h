#pragma once
#include "KeyFrame.h"

class AnimChannel{

public:
	AnimChannel();
	AnimChannel(ifstream* f);
	~AnimChannel();

	string name;
	vector<KeyFrame*> KeyFrames;


	void Read(ifstream* f);
	void Write(ofstream* f);
};

