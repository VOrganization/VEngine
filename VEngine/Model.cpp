#include "Model.h"

const float PI = 3.14159265359f;

static vec3 ai_to_glm_vec3(aiVector3D * d) {
	return vec3(d->x, d->y, d->z);
}

static float qtd(float d) {
	return (180.0f * (2.0f * acos(d))) / PI;
}

static vec3 aiquat_to_rot(aiQuaterniont<float>* d) {
	float qx = d->x;
	float qy = d->y;
	float qz = d->z;
	float qw = d->w;

	float qx2 = d->x * d->x;
	float qy2 = d->y * d->y;
	float qz2 = d->z * d->z;
	float qw2 = d->w * d->w;

	float x, y, z;

	float test = qx*qy + qz*qw;
	if (test > 0.499) {
		y = 360 / PI*atan2(qx, qw);
		z = 90;
		x = 0;
		return vec3(x, y, z);
	}
	if (test < -0.499) {
		y = -360 /PI*atan2(qx, qw);
		z = -90;
		x = 0;
		return vec3(x, y, z);
	}

	float h = atan2(2 * qy*qw - 2 * qx*qz, 1 - 2 * qy2 - 2 * qz2);
	float a = asin(2 * qx*qy + 2 * qz*qw);
	float b = atan2(2 * qx*qw - 2 * qy*qz, 1 - 2 * qx2 - 2 * qz2);
	y = round(h * 180 / PI);
	z = round(a * 180 / PI);
	x = round(b * 180 / PI);

	return vec3(x, y, z);
}

static vec3 mesh_process_vec3(vec3 d) {
	vec3 o = d;

	/*vec4 c = vec4(o, 1);

	float angle = 90;
	angle = radians(angle);

	mat4 r;
	r[0] = vec4(1, 0, 0, 0);
	r[1] = vec4(0, cos(angle), -sin(angle), 0);
	r[2] = vec4(0, sin(angle), cos(angle), 0);
	r[3] = vec4(0, 0, 0, 1);

	vec4 o4 = r * c;
	o = vec3(o4);*/

	return o;
}

static Transform* convert_to_transform(const aiMatrix4x4* from) {
	aiVector3D ai_s;
	aiVector3D ai_p;
	aiQuaterniont<float> ai_r;
	from->Decompose(ai_s, ai_r, ai_p);

	vec3 p = ai_to_glm_vec3(&ai_p);
	p = mesh_process_vec3(p);

	vec3 r = aiquat_to_rot(&ai_r);
	r = mesh_process_vec3(r);

	vec3 s = ai_to_glm_vec3(&ai_s);

	return new Transform(p, r, s);
}

static Transform* convert_to_transform(aiVector3D* ai_p, aiQuaterniont<float>* ai_r, aiVector3D* ai_s) {
	vec3 p = ai_to_glm_vec3(ai_p);
	p = mesh_process_vec3(p);

	vec3 r = aiquat_to_rot(ai_r);
	r = mesh_process_vec3(r);

	vec3 s = ai_to_glm_vec3(ai_s);

	return new Transform(p, r, s);
}



// -------------------------------------------------------------------------------------------- //

void Model::processNode(aiNode * node, const aiScene * scene, Object* o){

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		ObjectMesh* tmp_mesh = new ObjectMesh();
		processMesh(mesh, scene, tmp_mesh);
		o->children->push_back(tmp_mesh);
	}

	
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		Object* tmp_obj = new Object();
		tmp_obj->name = node->mChildren[i]->mName.C_Str();

		tmp_obj->transform = convert_to_transform(&node->mChildren[i]->mTransformation);

		/*bool found = false;
		for (size_t x = 0; x < this->animations.size(); x++){
			for (size_t y = 0; y < this->animations[x]->Channels.size(); y++){
				string n = this->animations[x]->Channels[y]->name;
				if (n == tmp_obj->name) {
					tmp_obj->animations.push_back(this->animations[x]);
					found = true;
					break;
				}
			}
			if (found)
				break;
		}*/

		processNode(node->mChildren[i], scene, tmp_obj);
		o->children->push_back(tmp_obj);
	}
}

// -------------------------------------------------------------------------------------------- //

void Model::processMesh(aiMesh * mesh, const aiScene * scene, ObjectMesh* o){
	o->name = mesh->mName.C_Str();

	Mesh* m = new Mesh();

	for (unsigned int i = 0; i < mesh->mNumVertices; i++){
		Vertex v;

		v.pos.x = mesh->mVertices[i].x;
		v.pos.y = mesh->mVertices[i].y;
		v.pos.z = mesh->mVertices[i].z;

		//v.pos.y = -v.pos.y;

		if (mesh->HasNormals()) {
			v.normal.x = mesh->mNormals[i].x;
			v.normal.y = mesh->mNormals[i].y;
			v.normal.z = mesh->mNormals[i].z;

			//v.normal.y = -v.normal.y;
		}

		if (mesh->mTextureCoords[0]) {
			v.uv.x = mesh->mTextureCoords[0][i].x;
			v.uv.y = mesh->mTextureCoords[0][i].y;
		}

		m->vertices.push_back(v);
	}

	if (mesh->HasFaces()) {
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
				m->index.push_back(mesh->mFaces[i].mIndices[j]);
			}
		}
	}


	/*if (mesh->HasBones()) {
		for (size_t i = 0; i < mesh->mNumVertices; i++){
			blendWeights[i] = vec4(-1);
			blendIndices[i] = ivec4(-1);
		}

		vector<Bone*> bones;

		for (size_t i = 0; i < mesh->mNumBones; i++){
			Bone* bone = new Bone(mesh->mBones[i]->mName.C_Str(), (*convert_to_transform(&mesh->mBones[i]->mOffsetMatrix)));

			bone->index = i;

			for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				vec4 bi = blendIndices[mesh->mBones[i]->mWeights[j].mVertexId];
				vec4 bw = blendWeights[mesh->mBones[i]->mWeights[j].mVertexId];
				for (size_t n = 0; n < 4; n++){
					if (bi[n] == -1 && bw[n] == -1) {
						bi[n] = i;
						bw[n] = mesh->mBones[i]->mWeights[j].mWeight;
						break;
					}
				}
				blendIndices[mesh->mBones[i]->mWeights[j].mVertexId] = bi;
				blendWeights[mesh->mBones[i]->mWeights[j].mVertexId] = bw;
			}

			bones.push_back(bone);
		}

		if (this->armatures.size() > 0) {
			for (size_t i = 0; i < this->armatures.size(); i++){
				if (this->armatures[i]->NumOfBones() == mesh->mNumBones) {
					o->armature = this->armatures[i];
					break;
				}
			}
		}
		else{
			Armature* arm = new Armature();
			this->processArmature(scene->mRootNode, arm, &bones);
			this->armatures.push_back(arm);
			o->armature = arm;
		}

		for (size_t i = 0; i < mesh->mNumVertices; i++) {
			for (size_t n = 0; n < 4; n++) {
				if (blendWeights[i][n] == -1)
					blendWeights[i][n] = 0;
				if (blendIndices[i][n] == -1)
					blendIndices[i][n] = 0;
			}
		}

		m->SetBone(blendIndices, blendWeights);
	}
	else {
		delete[] blendIndices;
		delete[] blendWeights;
	}*/


	MD5 md5;
	string tmp_id = o->name + to_string(mesh->mNumFaces);
	if (tmp_id[0] == 'g') {
		tmp_id = tmp_id.erase(0, 2);
	}
	m->id = md5.digestString(_strdup(tmp_id.c_str()));

	o->mesh = m;

	meshes.push_back(m);
}

// -------------------------------------------------------------------------------------------- //

void Model::processArmature(aiNode * node, Armature * armature, vector<Bone*>* bones){
	if (this->animations.size() > 0) {
		for (size_t i = 0; i < this->animations.size(); i++){
			for (size_t j = 0; j < this->animations[i]->Channels.size(); j++){
				bool found = false;
				for (size_t x = 0; x < bones->size(); x++){
					if ((*bones)[x]->name == this->animations[i]->Channels[j]->name) {
						armature->Animations.push_back(this->animations[i]);
						found = true;
						break;
					}
				}
				if (found)
					break;
			}
		}
	}

	aiNode* node_armature = this->findParentBone(node, bones);
	
	armature->name = node_armature->mName.C_Str();

	for (size_t i = 0; i < node_armature->mNumChildren; i++){
		Bone* b = nullptr;
		this->processBone(node_armature->mChildren[i], &b, bones);
		if (b != nullptr)
			armature->Bones.push_back(b);
	}

}

// -------------------------------------------------------------------------------------------- //

aiNode * Model::findParentBone(aiNode * node, vector<Bone*>* bones){
	bool found = false;
	for (size_t i = 0; i < (*bones).size(); i++){
		if (node->mName.C_Str() == (*bones)[i]->name) {
			found = true;
			break;
		}
	}


	if (found) {
		if (node->mParent != nullptr) {
			found = false;
			for (size_t i = 0; i < (*bones).size(); i++) {
				if (node->mParent->mName.C_Str() == (*bones)[i]->name) {
					found = true;
					break;
				}
			}

			if (!found)
				return node->mParent;
		}
	}
	
	for (size_t i = 0; i < node->mNumChildren; i++) {
		aiNode* n = this->findParentBone(node->mChildren[i], bones);
		if (n != nullptr)
			return n;
	}

	return nullptr;
}

// -------------------------------------------------------------------------------------------- //

void Model::processBone(aiNode * node, Bone ** bone, vector<Bone*>* bones){

	for (size_t i = 0; i < (*bones).size(); i++){
		if (node->mName.C_Str() == (*bones)[i]->name) {
			(*bone) = (*bones)[i];
			break;
		}
	}

	if (bone == nullptr)
		return;

	for (size_t i = 0; i < node->mNumChildren; i++) {
		Bone* b = nullptr;
		this->processBone(node->mChildren[i], &b, bones);
		if (b != nullptr)
			(*bone)->Bones.push_back(b);
	}
}

// -------------------------------------------------------------------------------------------- //

void Model::CalcAnimation(const aiScene* scene){
	if (!scene->HasAnimations())
		return;

	for (size_t i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* anim = scene->mAnimations[i];
		vector<AnimChannel*> chnn;
		for (size_t j = 0; j < anim->mNumChannels; j++) {
			AnimChannel* ch = new AnimChannel();
			aiNodeAnim* n = anim->mChannels[j];
			ch->name = n->mNodeName.C_Str();
			for (size_t y = 0; y < n->mNumPositionKeys; y++) {
				Transform* t = convert_to_transform(&n->mPositionKeys[y].mValue, &n->mRotationKeys[y].mValue, &n->mScalingKeys[y].mValue);
				ch->KeyFrames.push_back(new KeyFrame(n->mPositionKeys[y].mTime, t));
			}
			chnn.push_back(ch);
		}

		Animation* a = new Animation(anim->mDuration, anim->mTicksPerSecond, &chnn);
		a->name = anim->mName.C_Str();
		a->SetLoop(true);
		a->SetAutoStart(true);

		this->animations.push_back(a);
	}
}

// -------------------------------------------------------------------------------------------- //

Object * Model::clone(Object * obj){
	Object* tmp;
	if (obj->type = OT_MESH) {
		tmp = new ObjectMesh();
		MESH(tmp)->mesh = MESH(obj)->mesh;
		MESH(tmp)->material = MESH(obj)->material;
	}
	else
		tmp = new Object();

	tmp->name = obj->name;
	tmp->transform = obj->transform;

	for (size_t i = 0; i < obj->children->size(); i++)
		tmp->children->push_back(this->clone(obj->children->at(i)));

	return tmp;
}

// -------------------------------------------------------------------------------------------- //

Model::Model(){
	this->obj = new Object();
}

// -------------------------------------------------------------------------------------------- //

Model::Model(string path){
	this->obj = new Object();

}

// -------------------------------------------------------------------------------------------- //

Model::Model(File * file){
	this->obj = new Object();
	this->obj->name = file->GetName();

	char* data;
	unsigned int size;
	file->GetData(&data, &size);

	this->name = file->GetChecksum();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(data, (size_t)size, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}
	else {
		this->CalcAnimation(scene);
		processNode(scene->mRootNode, scene, this->obj);
	}

}

// -------------------------------------------------------------------------------------------- //

Model::~Model(){
	this->obj->~Object();
	for (size_t i = 0; i < this->meshes.size(); i++){
		this->meshes[i]->~Mesh();
		delete this->meshes[i];
		this->meshes[i] = 0;
	}
}

// -------------------------------------------------------------------------------------------- //

Object * Model::GetRootObject(){
	return this->obj;
}

// -------------------------------------------------------------------------------------------- //

Object * Model::CloneObject(){
	return this->clone(this->obj);
}

// -------------------------------------------------------------------------------------------- //

vector<Mesh*>* Model::GetMeshes(){
	return &this->meshes;
}

// -------------------------------------------------------------------------------------------- //

vector<Animation*>* Model::GetAnimations(){
	return &this->animations;
}

// -------------------------------------------------------------------------------------------- //

vector<Armature*>* Model::GetArmature(){
	return &this->armatures;
}

// -------------------------------------------------------------------------------------------- //
