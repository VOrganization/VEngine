#pragma once
//#include "Mesh.h"
#include "AnimChannel.h"
#include "GFContext.h"

class Animation{
public:

	double duration = 0;
	double tps = 0;
	double start_time = -1.0;
	bool start = false;
	bool auto_start = false;
	bool loop = false;
	bool add_transform = false;

	double speed = 1;

public:
	Animation();
	Animation(ifstream* f);
	Animation(double d, double tps, vector<AnimChannel*>* chn);
	~Animation();
	
	string name;

	vector<AnimChannel*> Channels;

	double GetDuration();

	double GetSpeed();
	void SetSpeed(double sp);

	void SetLoop(bool l);
	bool GetLoop();

	void SetAutoStart(bool as);
	bool GetAutoStart();

	bool IsPlay();
	void Start();
	void Pause();
	void Stop();
	void Update(string name, Transform* tran);
	void OnStart(string name, Transform* tran);


	void Read(ifstream* f);
	void Write(ofstream* f);
};

