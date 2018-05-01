#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Armature.h"
#include "ObjectMesh.h"
#include "Animation.h"
#include "File.h"

#define MESH(obj) ((ObjectMesh*)obj)

class Model{

	void processNode(aiNode* node, const aiScene* scene, Object* o);
	void processMesh(aiMesh* mesh, const aiScene* scene, ObjectMesh* o);

	void processArmature(aiNode* node, Armature* armature, vector<Bone*>* bones);
	aiNode* findParentBone(aiNode* node, vector<Bone*>* bones);
	void processBone(aiNode* node, Bone** bone, vector<Bone*>* bones);



	void CalcAnimation(const aiScene* scene);

	Object* obj;
	vector<Mesh*> meshes;
	vector<Animation*> animations;
	vector<Armature*> armatures;

	string name;

	Object* clone(Object* obj);


public:
	Model();
	Model(string path);
	Model(File* file);
	~Model();

	Object* GetRootObject();
	Object* CloneObject();

	vector<Mesh*>* GetMeshes();
	vector<Animation*>* GetAnimations();
	vector<Armature*>* GetArmature();

};

