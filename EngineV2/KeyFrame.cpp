#include "KeyFrame.h"



KeyFrame::KeyFrame(){
	this->transform = new Transform();
	this->time = 0;
}

// -------------------------------------------------------------------------------------------- //

KeyFrame::KeyFrame(ifstream * f){
	this->transform = new Transform();
	this->time = 0;
	this->Read(f);
}

// -------------------------------------------------------------------------------------------- //

KeyFrame::KeyFrame(double t, Transform * tran){
	this->time = t;
	this->transform = tran;
}

// -------------------------------------------------------------------------------------------- //

KeyFrame::~KeyFrame(){
	this->transform->~Transform();
	delete this->transform;
	this->transform = 0;
}

// -------------------------------------------------------------------------------------------- //

double KeyFrame::GetTime(){
	return this->time;
}

// -------------------------------------------------------------------------------------------- //

void KeyFrame::SetTime(double t){
	this->time = time;
}

// -------------------------------------------------------------------------------------------- //

Transform * KeyFrame::GetTransform(){
	return this->transform;
}

// -------------------------------------------------------------------------------------------- //

void KeyFrame::SetTransform(Transform * tran){
	this->transform = tran;
}

// -------------------------------------------------------------------------------------------- //

void KeyFrame::Read(ifstream * f){
	f->read((char*)&this->time, 8);
	this->transform->Read(f);
}

// -------------------------------------------------------------------------------------------- //

void KeyFrame::Write(ofstream * f){
	f->write((char*)&this->time, 8);
	this->transform->Write(f);
}
