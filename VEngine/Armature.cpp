#include "Armature.h"

#define PI 3.14159265359f

static quat CalcQuat(vec3 rot) {
	quat q;

	float h = rot.y * PI / 360.0f;
	float a = rot.z * PI / 360.0f;
	float b = rot.x * PI / 360.0f;

	float c1 = cos(h);
	float c2 = cos(a);
	float c3 = cos(b);
	float s1 = sin(h);
	float s2 = sin(a);
	float s3 = sin(b);

	q.w = round((c1*c2*c3 - s1*s2*s3) * 100000.0f) / 100000.0f;
	q.x = round((s1*s2*c3 + c1*c2*s3) * 100000.0f) / 100000.0f;
	q.y = round((s1*c2*c3 + c1*s2*s3) * 100000.0f) / 100000.0f;
	q.z = round((c1*s2*c3 - s1*c2*s3) * 100000.0f) / 100000.0f;

	return q;
}

// -------------------------------------------------------------------------------------------- //

void Armature::showBone(Bone * bone){
	vec3 pos = bone->transform.position;
	vec3 rot = bone->transform.rotation;
	vec3 sca = bone->transform.scale;

	vec3 pos1 = vec3(0);
	pos1.x = pos.x + cos(rot.x) * sca.x;
	pos1.y = pos.y + sin(rot.y) * sca.y;
	pos1.z = pos.z + cos(rot.z) * sca.z;

	/*glBegin(GL_LINE);
	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(pos1.x, pos1.y, pos1.z);
	glEnd();*/

	for (unsigned int i = 0; i < bone->Bones.size(); i++)
		this->showBone(bone->Bones[i]);
}

// -------------------------------------------------------------------------------------------- //

unsigned int Armature::counterBone(Bone * bone){
	unsigned int count = 1;
	for (unsigned int i = 0; i < bone->Bones.size(); i++)
		count += counterBone(bone->Bones[i]);
	return count;
}

// -------------------------------------------------------------------------------------------- //

Bone * Armature::findBone(Bone* bone, string name){
	if (bone->name == name)
		return bone;
	for (unsigned int i = 0; i < bone->Bones.size(); i++){
		Bone* b = findBone(bone->Bones[i], name);
		if (b != nullptr)
			return b;
	}
	return nullptr;
}

// -------------------------------------------------------------------------------------------- //

void Armature::calcBone(Bone * a_bone, Bone * p_bone){
	mat4 S = scale(mat4(1), a_bone->transform.scale);
	
	mat4 R = toMat4(CalcQuat(a_bone->transform.rotation));

	mat4 T = translate(mat4(1), a_bone->transform.position);
	a_bone->xfrom = T * R * S;

	//cout << a_bone->name << ":  " << a_bone->transform.GetRotation().x << "  " << a_bone->transform.GetRotation().y << "  " << a_bone->transform.GetRotation().z << "\n";

	if (p_bone == nullptr)
		a_bone->comb = a_bone->xfrom;
	else
		a_bone->comb = p_bone->comb * a_bone->xfrom;

	this->matrix_data[a_bone->index] = a_bone->comb * this->inv_matrix_data[a_bone->index];

	for (unsigned int i = 0; i < a_bone->Bones.size(); i++)
		calcBone(a_bone->Bones[i], a_bone);
}

// -------------------------------------------------------------------------------------------- //

void Armature::calcBoneInv(Bone * a_bone, Bone * p_bone){
	mat4 S = scale(mat4(1), a_bone->transform.scale);
	
	mat4 R = toMat4(CalcQuat(a_bone->transform.rotation));
	
	mat4 T = translate(mat4(1), a_bone->transform.position);
	a_bone->xfrom = T * R * S;

	if (p_bone == nullptr)
		a_bone->comb = a_bone->xfrom;
	else
		a_bone->comb = p_bone->comb * a_bone->xfrom;

	this->bind_matrix_data[a_bone->index] = a_bone->comb;
	this->inv_matrix_data[a_bone->index] = inverse(a_bone->comb);

	for (unsigned int i = 0; i < a_bone->Bones.size(); i++)
		calcBoneInv(a_bone->Bones[i], a_bone);
}

// -------------------------------------------------------------------------------------------- //

void Armature::animationBone(Bone * bone, Animation * animation){
	animation->Update(bone->name, &bone->transform);

	for (unsigned int i = 0; i < bone->Bones.size(); i++)
		this->animationBone(bone->Bones[i], animation);
}

// -------------------------------------------------------------------------------------------- //

void Armature::startAnimationBone(Bone * bone, Animation * animation){
	animation->OnStart(bone->name, &bone->transform);
	for (unsigned int i = 0; i < bone->Bones.size(); i++)
		this->startAnimationBone(bone->Bones[i], animation);
}

// -------------------------------------------------------------------------------------------- //

void Armature::testBone(Bone * bone){
	
}

// -------------------------------------------------------------------------------------------- //

Armature::Armature(){ }

// -------------------------------------------------------------------------------------------- //

Armature::Armature(ifstream * f){
	this->Read(f);
}

// -------------------------------------------------------------------------------------------- //

Armature::~Armature(){
	this->matrix_size = 0;
	delete[] this->matrix_data;
	this->matrix_data = 0;
	delete[] this->inv_matrix_data;
	this->inv_matrix_data = 0;
}

// -------------------------------------------------------------------------------------------- //

unsigned int Armature::NumOfBones(){
	unsigned int num = 0;
	for (unsigned int i = 0; i < this->Bones.size(); i++)
		num += this->counterBone(this->Bones[i]);
	return num;
}

// -------------------------------------------------------------------------------------------- //

Bone * Armature::FindBone(string name){
	for (unsigned int i = 0; i < this->Bones.size(); i++){
		Bone* b = this->findBone(this->Bones[i], name);
		if (b != nullptr)
			return b;
	}
	return nullptr;
}

// -------------------------------------------------------------------------------------------- //

void Armature::Display(){
	for (unsigned int i = 0; i < this->Bones.size(); i++)
		showBone(this->Bones[i]);
}

// -------------------------------------------------------------------------------------------- //

void Armature::CalcData(){
	unsigned int data_size = this->NumOfBones();
	if (data_size != this->matrix_size) {
		delete[] this->matrix_data;
		delete[] this->bind_matrix_data;
		delete[] this->inv_matrix_data;
		this->matrix_size = data_size;
		this->matrix_data = new mat4[data_size];
		this->bind_matrix_data = new mat4[data_size];
		this->inv_matrix_data = new mat4[data_size];

		for (unsigned int i = 0; i < this->Bones.size(); i++)
			this->calcBoneInv(this->Bones[i], nullptr);

		for (unsigned int i = 0; i < this->matrix_size; i++)
			this->matrix_data[i] = this->bind_matrix_data[i] * this->inv_matrix_data[i];
	}

	if (this->anim_is_play == false && this->Animations[0]->IsPlay()) {
		for (unsigned int i = 0; i < this->Bones.size(); i++)
			this->startAnimationBone(this->Bones[i], this->Animations[0]);

		for (unsigned int i = 0; i < this->Bones.size(); i++)
			this->calcBoneInv(this->Bones[i], nullptr);
		this->anim_is_play = true;
	}

	for (unsigned int i = 0; i < this->Bones.size(); i++) {
		for (unsigned int j = 0; j < this->Animations.size(); j++)
			this->animationBone(this->Bones[i], this->Animations[j]);
		this->calcBone(this->Bones[i], nullptr);
	}
}

// -------------------------------------------------------------------------------------------- //

mat4* Armature::GetData(){
	return this->matrix_data;
}

// -------------------------------------------------------------------------------------------- //

unsigned int Armature::GetDataSize(){
	return this->matrix_size;
}

// -------------------------------------------------------------------------------------------- //

Mesh * Armature::CreateArmature(){

	return nullptr;
}

// -------------------------------------------------------------------------------------------- //

void Armature::Read(ifstream * f){
	unsigned int name_size;
	f->read((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = new char[name_size];
	f->read(tmp_name, name_size);
	tmp_name[name_size] = '\0';
	this->name = tmp_name;

	unsigned int bone_size = 0;
	f->read((char*)(&bone_size), 4);
	for (unsigned int i = 0; i < bone_size; i++) {
		Bone* b = new Bone();
		b->Read(f);
		this->Bones.push_back(b);
	}

	unsigned int animation_size = 0;
	f->read((char*)(&animation_size), 4);
	for (unsigned int i = 0; i < animation_size; i++)
		this->Animations.push_back(new Animation(f));

}

// -------------------------------------------------------------------------------------------- //

void Armature::Write(ofstream * f){
	unsigned int name_size = this->name.size();
	f->write((char*)&name_size, sizeof(unsigned int));
	char* tmp_name = _strdup(this->name.c_str());
	f->write(tmp_name, name_size);

	unsigned int bone_size = this->Bones.size();
	f->write((char*)(&bone_size), 4);
	for (unsigned int i = 0; i < bone_size; i++)
		this->Bones[i]->Write(f);

	unsigned int animation_size = this->Animations.size();
	f->write((char*)(&animation_size), 4);
	for (unsigned int i = 0; i < animation_size; i++)
		this->Animations[i]->Write(f);
}
