#include "Animation.h"



static vec3 calc_vec3_time(vec3 ks, vec3 kk, double p) {
	vec3 o;
	o = ks + ((float)p * (kk - ks));
	return o;
}

// -------------------------------------------------------------------------------------------- //

Animation::Animation(){}

// -------------------------------------------------------------------------------------------- //

Animation::Animation(ifstream * f){
	this->Read(f);
}

// -------------------------------------------------------------------------------------------- //

Animation::Animation(double d, double tps, vector<AnimChannel*>* chn){
	this->duration = d;
	this->tps = this->tps;
	this->Channels = (*chn);
}

// -------------------------------------------------------------------------------------------- //

Animation::~Animation()
{
}

// -------------------------------------------------------------------------------------------- //

double Animation::GetDuration(){
	return this->duration;
}

// -------------------------------------------------------------------------------------------- //

double Animation::GetSpeed(){
	return this->speed;
}

// -------------------------------------------------------------------------------------------- //

void Animation::SetSpeed(double sp){
	this->speed = sp;
}

// -------------------------------------------------------------------------------------------- //

void Animation::SetLoop(bool l){
	this->loop = l;
}

// -------------------------------------------------------------------------------------------- //

bool Animation::GetLoop(){
	return this->loop;
}

// -------------------------------------------------------------------------------------------- //

void Animation::SetAutoStart(bool as){
	this->auto_start = as;
}

// -------------------------------------------------------------------------------------------- //

bool Animation::GetAutoStart(){
	return this->auto_start;
}

// -------------------------------------------------------------------------------------------- //

bool Animation::IsPlay(){
	return this->start;
}

// -------------------------------------------------------------------------------------------- //

void Animation::Start(){
	this->start = true;
	this->start_time = glfwGetTime();
}

// -------------------------------------------------------------------------------------------- //

void Animation::Stop(){
	this->start = false;
}

// -------------------------------------------------------------------------------------------- //

void Animation::Update(string name, Transform* tran){
	if (!start)
		return;

	double time = glfwGetTime() * this->speed;
	double t = this->duration - ((this->start_time + this->duration) - time);

	//cout << t << "/" << this->duration << "  Anim: " << name << "\n";

	if (this->loop) {
		if ((this->start_time + this->duration) < time)
			this->start_time = time;
	}
	else {
		if ((this->start_time + this->duration) < time)
			this->start = false;
	}

	for (size_t i = 0; i < this->Channels.size(); i++){
		if (this->Channels[i]->name == name) {


			KeyFrame* prev = this->Channels[i]->KeyFrames[0];
			KeyFrame* key;
			for (size_t j = 0; j < this->Channels[i]->KeyFrames.size(); j++){
				key = this->Channels[i]->KeyFrames[j];

				if (j > 0)
					prev = this->Channels[i]->KeyFrames[j - 1];

				if (t >= prev->GetTime() && key->GetTime() >= t) {
					double p = (t - prev->GetTime()) / (key->GetTime() - prev->GetTime());

					tran->position = calc_vec3_time(prev->GetTransform()->position, key->GetTransform()->position, p);
					tran->rotation = calc_vec3_time(prev->GetTransform()->rotation, key->GetTransform()->rotation, p);
					tran->scale = calc_vec3_time(prev->GetTransform()->scale, key->GetTransform()->scale, p);

					break;
				}

			}

			break;
		}
	}

	
}

// -------------------------------------------------------------------------------------------- //

void Animation::OnStart(string name, Transform * tran){
	for (size_t i = 0; i < this->Channels.size(); i++) {
		if (this->Channels[i]->name == name) {
			memcpy(tran, this->Channels[i]->KeyFrames[0]->GetTransform(), sizeof(Transform));
			break;
		}
	}
}

// -------------------------------------------------------------------------------------------- //

void Animation::Read(ifstream * f){
	char* cs = new char[10];
	f->read(cs, 4);

	unsigned int n_s;
	f->read((char*)&n_s, 4);
	char* n = new char[n_s];
	f->read(n, n_s);
	n[n_s] = '\0';
	this->name = n;

	f->read((char*)&this->add_transform, 1);
	f->read((char*)&this->duration, 8);
	f->read((char*)&this->loop, 1);
	f->read((char*)&this->speed, 4);
	f->read((char*)&this->start, 1);
	f->read((char*)&this->auto_start, 1);
	f->read((char*)&this->tps, 8);

	if (auto_start)
		this->Start();

	unsigned int ch_s;
	f->read((char*)&ch_s, 4);
	for (unsigned int i = 0; i < ch_s; i++)
		this->Channels.push_back(new AnimChannel(f));
}

// -------------------------------------------------------------------------------------------- //

void Animation::Write(ofstream * f){
	f->write("ANIM", 4);

	unsigned int n_s = this->name.size();
	char* n = _strdup(this->name.c_str());
	f->write((char*)&n_s, 4);
	f->write(n, n_s);

	f->write((char*)&this->add_transform, 1);
	f->write((char*)&this->duration, 8);
	f->write((char*)&this->loop, 1);
	f->write((char*)&this->speed, 4);
	f->write((char*)&this->start, 1);
	f->write((char*)&this->auto_start, 1);
	f->write((char*)&this->tps, 8);

	unsigned int ch_s = this->Channels.size();
	f->write((char*)&ch_s, 4);
	for (unsigned int i = 0; i < ch_s; i++)
		this->Channels[i]->Write(f);

}
