#include "Scene.h"
#include "res.h"


void Scene::Init(){

	this->WorldTransform = new Transform();

	this->Files = new vector<File*>();
	this->Models = new vector<Model*>();
	this->Meshes = new vector<Mesh*>();
	this->Animations = new vector<Animation*>();
	this->Armatures = new vector<Armature*>();
	this->Shaders = new vector<Shader*>();
	this->Textures_2D = new vector<Texture_2D*>();
	this->Materials = new vector<Material*>();
	this->Objects = new vector<Object*>();
	this->Scripts = new vector<Script*>();

	this->default_shader = new Shader();
	this->default_shader->name = "default_engine_shader";
	this->default_shader->Load("main.vshader");
	//this->default_shader->Load(_strdup(res_default_shader), strlen(res_default_shader));

	this->InitShader(this->default_shader);

	this->default_material = new Material();
	this->default_material->ambient = vec4(0.2, 0.2, 0.2, 1.0);
	this->default_material->diffuse = vec4(0.8, 0.8, 0.8, 1.0);
	this->default_material->specular = vec4(1.0, 1.0, 1.0, 1.0);
	this->default_material->shininess = 32.0f;
	this->default_material->name = "default_engine_material";
	this->default_material->shader = this->default_shader;


}

// -------------------------------------------------------------------------------------------- //

void Scene::InitShader(Shader * s){
	/*GLuint index = 0;
	index = glGetUniformBlockIndex(s->program, "Matrix");
	glUniformBlockBinding(s->program, index, 1);

	index = glGetUniformBlockIndex(s->program, "Bones");
	glUniformBlockBinding(s->program, index, 2);

	index = glGetUniformBlockIndex(s->program, "Material");
	glUniformBlockBinding(s->program, index, 3);

	index = glGetUniformBlockIndex(s->program, "Engine");
	glUniformBlockBinding(s->program, index, 4);

	index = glGetUniformBlockIndex(s->program, "LightPoint");
	glUniformBlockBinding(s->program, index, 5);

	index = glGetUniformBlockIndex(s->program, "LightSpot");
	glUniformBlockBinding(s->program, index, 6);

	index = glGetUniformBlockIndex(s->program, "LightDir");
	glUniformBlockBinding(s->program, index, 7);*/

	
}

// -------------------------------------------------------------------------------------------- //

void Scene::InitMaterial(Material * m){
	for (unsigned int i = 0; i < this->Textures_2D->size(); i++){

		if (m->diffuseMap != nullptr && m->diffuseMap->GetIndex() == 1) {
			if (m->diffuseMap->id == this->Textures_2D->at(i)->id) {
				m->diffuseMap->~Texture_2D();
				delete m->diffuseMap;
				m->diffuseMap = this->Textures_2D->at(i);
			}
		}

		if (m->specularMap != nullptr && m->specularMap->GetIndex() == 1) {
			if (m->specularMap->id == this->Textures_2D->at(i)->id) {
				m->specularMap->~Texture_2D();
				delete m->specularMap;
				m->specularMap = this->Textures_2D->at(i);
			}
		}

		if (m->emissionMap != nullptr && m->emissionMap->GetIndex() == 1) {
			if (m->emissionMap->id == this->Textures_2D->at(i)->id) {
				m->emissionMap->~Texture_2D();
				delete m->emissionMap;
				m->emissionMap = this->Textures_2D->at(i);
			}
		}

		//PBR

		if (m->bumpMap != nullptr && m->bumpMap->GetIndex() == 1) {
			if (m->bumpMap->id == this->Textures_2D->at(i)->id) {
				m->bumpMap->~Texture_2D();
				delete m->bumpMap;
				m->bumpMap = this->Textures_2D->at(i);
			}
		}

		if (m->normalMap != nullptr && m->normalMap->GetIndex() == 1) {
			if (m->normalMap->id == this->Textures_2D->at(i)->id) {
				m->normalMap->~Texture_2D();
				delete m->normalMap;
				m->normalMap = this->Textures_2D->at(i);
			}
		}

		if (m->aoMap != nullptr && m->aoMap->GetIndex() == 1) {
			if (m->aoMap->id == this->Textures_2D->at(i)->id) {
				m->aoMap->~Texture_2D();
				delete m->aoMap;
				m->aoMap = this->Textures_2D->at(i);
			}
		}
	}

	if (m->shader == nullptr) {
		m->shader = this->default_shader;
	}
	else {
		this->InitShader(m->shader);
	}
}

// -------------------------------------------------------------------------------------------- //

void Scene::LoadFile(File * file){
	if (file == nullptr)
		return;

	file->Load();
	if (!file->Check())
		return;

	switch (file->GetType()){

	case FT_IMAGE: {
		cout << "Load image 2D: " << file->GetName() << "\n";
		Texture_2D* tmp_texture = new Texture_2D(file);
		this->Textures_2D->push_back(tmp_texture);
		break;
	}

	case FT_MODEL: {
		cout << "Load model: " << file->GetName() << "\n";
		Model* tmp_model = new Model(file);
		this->Models->push_back(tmp_model);

		{
			/*vector<Mesh*>* v2 = tmp_model->GetMeshes();
			unsigned int v0_szie = this->Meshes->size();
			this->Meshes->resize(v0_szie + (*v2).size());
			memcpy(&( (*this->Meshes[v0_szie]) ), &((*v2)[0]), sizeof(Mesh*) * (*v2).size());*/
			
			vector<Mesh*>* B = tmp_model->GetMeshes();
			this->Meshes->insert(this->Meshes->end(), B->begin(), B->end());

		}


		break;
	}

	/*case FT_SHADER: {
		cout << "Load shader: " << file->GetName() << "\n";
		Shader* tmp_shader = new Shader(file);
		this->Shaders.push_back(tmp_shader);
		break;
	}*/

	/*case FT_SCRIPT: {
		cout << "Load script: " << file->GetName() << "\n";
		Script* tmp_script = new Script();
		file->GetData(&tmp_script->context, &tmp_script->context_size);
		this->Scripts.push_back(tmp_script);
		break;
	}*/

	default:
		break;
	}
}

// -------------------------------------------------------------------------------------------- //

void Scene::ObjWrite(Object * obj, ofstream* f){
	f->write((char*)&obj->type, sizeof(unsigned int));

	switch (obj->type) {

	case OT_MESH: {
		((ObjectMesh*)obj)->Write(f);
		break;
	}

	case OT_LIGHT_POINT: {
		((ObjectLightPoint*)obj)->Write(f);
		break;
	}

	case OT_LIGHT_SPOT: {
		((ObjectLightSpot*)obj)->Write(f);
		break;
	}

	case OT_LIGHT_DIR: {
		((ObjectLightDir*)obj)->Write(f);
		break;
	}

	case OT_CAMERA: {
		((ObjectCamera*)obj)->Write(f);
		break;
	}

	default:
		obj->Write(f);
		break;
	}

	unsigned int children_size = obj->children->size();
	f->write((char*)&children_size, sizeof(unsigned int));

	for (size_t i = 0; i < obj->children->size(); i++)
		ObjWrite(obj->children->at(i), f);
}

// -------------------------------------------------------------------------------------------- //

Object* Scene::ObjRead(ifstream* f, Object* p){
	Object* obj;
	unsigned int type;
	f->read((char*)&type, sizeof(unsigned int));

	switch (type) {

	case OT_NULL: {
		obj = new Object();
		obj->Read(f);
		obj->parent = p;
		break;
	}

	case OT_MESH: {
		obj = new ObjectMesh(f);
		obj->parent = p;
		ObjectMesh* tmp = (ObjectMesh*)obj;
		
		if (!tmp->mesh->IsLoad()) {
			bool found = false;
			for (unsigned int i = 0; i < this->Meshes->size(); i++){
				if (tmp->mesh->id == this->Meshes->at(i)->id) {
					found = true;
					delete tmp->mesh;
					tmp->mesh = this->Meshes->at(i);
					break;
				}
			}
			if (!found) {
				delete tmp->mesh;
				tmp->mesh = nullptr;
			}
		}
		
		if (tmp->material == nullptr) {
			tmp->material = this->default_material;
		}
		else {
			if (!tmp->material->IsLoad()) {
				bool found = false;
				for (unsigned int i = 0; i < this->Materials->size(); i++){
					if (tmp->material->id == this->Materials->at(i)->id) {
						delete tmp->material;
						tmp->material = this->Materials->at(i);
						found = true;
						break;
					}
				}
				if (!found) {
					tmp->material = this->default_material;
				}
			}
		}

		break;
	}

	case OT_LIGHT_POINT: {
		obj = new ObjectLightPoint();
		((ObjectLightPoint*)obj)->Read(f);
		obj->parent = p;
		break;
	}

	case OT_LIGHT_SPOT: {
		obj = new ObjectLightSpot();
		((ObjectLightSpot*)obj)->Read(f);
		obj->parent = p;
		break;
	}

	case OT_LIGHT_DIR: {
		obj = new ObjectLightDir();
		((ObjectLightDir*)obj)->Read(f);
		obj->parent = p;
		break;
	}

	case OT_CAMERA: {
		obj = new ObjectCamera();
		((ObjectCamera*)obj)->Read(f);
		obj->parent = p;
		break;
	}

	default: {
		obj = new Object();
		obj->parent = p;
		break;
	}

	}
	unsigned int children_size;
	f->read((char*)&children_size, sizeof(unsigned int));

	for (size_t i = 0; i < children_size; i++)
		obj->children->push_back(ObjRead(f, obj));
		
	return obj;
}

// -------------------------------------------------------------------------------------------- //

Scene::Scene(){
	this->Init();
}

// -------------------------------------------------------------------------------------------- //

Scene::Scene(string path){
	this->Init();
	this->Load(path);
}

// -------------------------------------------------------------------------------------------- //

Scene::~Scene(){
	//Files
	for (size_t i = 0; i < this->Files->size(); i++){
		delete this->Files->at(i);
		this->Files->at(i) = nullptr;
	}
	////Models
	//for (size_t i = 0; i < this->Models.size(); i++){
	//	this->Models[i]->~Model();
	//	delete this->Models[i];
	//	this->Models[i] = NULL;
	//}
	//Mesh
	for (size_t i = 0; i < this->Meshes->size(); i++){
		delete this->Meshes->at(i);
		this->Meshes->at(i) = nullptr;
	}
	//Textures
	for (size_t i = 0; i < this->Textures_2D->size(); i++) {
		delete this->Textures_2D->at(i);
		this->Textures_2D->at(i) = nullptr;
	}
	//Material
	for (size_t i = 0; i < this->Materials->size(); i++){
		delete this->Materials->at(i);
		this->Materials->at(i) = nullptr;
	}
	//delete this->default_material;
	//Object
	for (size_t i = 0; i < this->Objects->size(); i++) {
		delete this->Objects->at(i);
		this->Objects->at(i) = nullptr;
	}
	//Shader
	for (size_t i = 0; i < this->Shaders->size(); i++){
		delete this->Shaders->at(i);
		this->Shaders->at(i) = nullptr;
	}
	if (this->default_shader != nullptr) {
		delete this->default_shader;
		this->default_shader = nullptr;
	}
}

// -------------------------------------------------------------------------------------------- //

void Scene::Load(string path){
	ifstream* f = new ifstream(path.c_str(), ios::binary);

	cout << "\n\nStart Loading\n\n";

	char* checkchar = new char[32];
	f->read(checkchar, 1);
	if (checkchar[0] != 'S')
		return;
	f->read(checkchar, 5);
	checkchar[5] = '\0';
	cout << "Editor Version: " << checkchar << "\n\n";
	f->read(checkchar, 1);
	if (checkchar[0] != 'S')
		return;

	

	unsigned int files_size;
	f->read((char*)&files_size, 4);
	cout << "File: " << files_size << "\n";
	for (unsigned int i = 0; i < files_size; i++) {
		File* tmp_file = new File(f);
		this->LoadFile(tmp_file);
		this->Files->push_back(tmp_file);
	}

	cout << "\n";

	unsigned int model_size;
	f->read((char*)&model_size, 4);
	cout << "Model: " << model_size << "\n";

	cout << "\n";

	unsigned int mesh_size;
	f->read((char*)&mesh_size, 4);
	cout << "Mesh: " << mesh_size << "\n";

	cout << "\n";

	unsigned int armature_size;
	f->read((char*)&armature_size, 4);
	cout << "Armature: " << armature_size << "\n";

	cout << "\n";

	unsigned int animation_size;
	f->read((char*)&animation_size, 4);
	cout << "Animation: " << animation_size << "\n";

	cout << "\n";

	unsigned int animation_system_size;
	f->read((char*)&animation_system_size, 4);
	cout << "Animation System: " << animation_system_size << "\n";

	cout << "\n";

	unsigned int shader_size;
	f->read((char*)&shader_size, 4);
	cout << "Shader: " << shader_size << "\n";

	cout << "\n";

	unsigned int texture_size;
	f->read((char*)&texture_size, 4);
	cout << "Texture: " << texture_size << "\n";

	cout << "\n";

	unsigned int material_size;
	f->read((char*)&material_size, 4);
	cout << "Material: " << material_size << "\n";
	for (unsigned int i = 0; i < material_size; i++){
		Material* mat = new Material(f);
		mat->engine_id = i;
		this->InitMaterial(mat);
		this->Materials->push_back(mat);
		cout << "Load Material: " << mat->name << "\n";
	}

	cout << "\n";

	unsigned int render_size;
	f->read((char*)&render_size, 4);
	cout << "Render: " << render_size << "\n";

	cout << "\n";
	
	unsigned int script_size;
	f->read((char*)&script_size, 4);
	cout << "Script: " << script_size << "\n";

	cout << "\n";


	unsigned int objects_size;
	f->read((char*)&objects_size, 4);
	cout << "Object: " << objects_size << "\n";
	for (unsigned int i = 0; i < objects_size; i++){
		Object* obj = ObjRead(f, nullptr);
		this->Objects->push_back(obj);
		cout << "Add object: " << obj->name << "\n";
	}

	f->close();
	this->load = true;
	cout << "\nStop Loading\n";
}

// -------------------------------------------------------------------------------------------- //

void Scene::Save(string path){
	//ofstream f(path.c_str(), ios::binary);
	//f.write((char*)"SCENE", 5);

	////sekcja plikow
	//unsigned int files_size = this->Files->size();
	//f.write((char*)&files_size, sizeof(unsigned int));
	//for (unsigned int i = 0; i < this->Files->size(); i++) {
	//	if (Files->at(i) != nullptr) {
	//		Files[i]->Write(&f);
	//	}
	//	else
	//		f.write((char*)"NULL", 4);
	//}

	////sekcja obiektow
	//unsigned int object_size = this->Objects.size();
	//f.write((char*)&object_size, sizeof(unsigned int));
	//for (size_t i = 0; i < object_size; i++){
	//	if (this->Objects[i] != nullptr)
	//		ObjWrite(this->Objects[i], &f);
	//	else
	//		f.write((char*)"NULL", 4);
	//}

	//f.close();
}
