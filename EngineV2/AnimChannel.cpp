#include "AnimChannel.h"



AnimChannel::AnimChannel(){ }

// -------------------------------------------------------------------------------------------- //

AnimChannel::AnimChannel(ifstream * f){
	this->Read(f);
}

// -------------------------------------------------------------------------------------------- //

AnimChannel::~AnimChannel(){

}

// -------------------------------------------------------------------------------------------- //

void AnimChannel::Read(ifstream * f){
	unsigned int n_s;
	f->read((char*)&n_s, 4);
	char* n = new char[n_s];
	f->read(n, n_s);
	n[n_s] = '\0';
	this->name = n;

	unsigned int key_s;
	f->read((char*)&key_s, 4);
	for (unsigned int i = 0; i < key_s; i++)
		this->KeyFrames.push_back(new KeyFrame(f));

}

// -------------------------------------------------------------------------------------------- //

void AnimChannel::Write(ofstream * f){
	unsigned int n_s = this->name.size();
	char* n = _strdup(this->name.c_str());
	f->write((char*)&n_s, 4);
	f->write(n, n_s);

	unsigned int key_s = this->KeyFrames.size();
	f->write((char*)&key_s, 4);
	for (unsigned int i = 0; i < key_s; i++)
		this->KeyFrames[i]->Write(f);

}
