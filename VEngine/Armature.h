#pragma once
#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"

class Armature{

	void showBone(Bone* bone);
	unsigned int counterBone(Bone* bone);
	Bone* findBone(Bone* bone, string name);
	void calcBone(Bone* a_bone, Bone* p_bone);
	void calcBoneInv(Bone* a_bone, Bone* p_bone);
	void animationBone(Bone* bone, Animation* animation);
	void startAnimationBone(Bone* bone, Animation* animation);
	void testBone(Bone* bone);

	mat4* matrix_data;
	mat4* bind_matrix_data;
	mat4* inv_matrix_data;
	unsigned int matrix_size = 0;

	bool anim_is_play = false;

public:
	Armature();
	Armature(ifstream* f);
	~Armature();

	string name;

	vector<Animation*> Animations;

	vector<Bone*> Bones;
	unsigned int NumOfBones();
	Bone* FindBone(string name);
	void Display();

	void CalcData();
	mat4* GetData();
	unsigned int GetDataSize();

	Mesh* CreateArmature();

	void Read(ifstream* f);
	void Write(ofstream* f);
};

