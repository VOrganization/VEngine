#include "Mesh.h"



Mesh::Mesh(){
	
}

// -------------------------------------------------------------------------------------------- //

Mesh::~Mesh(){
	if (context == nullptr)
		return;

	vkDestroyBuffer(this->context->GetDevice(), this->vertexBuffer, VK_ALLOCK);
	this->vertexBuffer = VK_NULL_HANDLE;

	vkFreeMemory(this->context->GetDevice(), this->vertexBufferMemory, VK_ALLOCK);
	this->vertexBufferMemory = VK_NULL_HANDLE;

	this->vertices.clear();

	if (this->index.size() != 0) {
		vkDestroyBuffer(this->context->GetDevice(), this->indexBuffer, VK_ALLOCK);
		this->indexBuffer = VK_NULL_HANDLE;

		vkFreeMemory(this->context->GetDevice(), this->indexBufferMemory, VK_ALLOCK);
		this->indexBufferMemory = VK_NULL_HANDLE;

		this->index.clear();
	}
}

// -------------------------------------------------------------------------------------------- //

bool Mesh::IsLoad(){
	return (vertexBuffer != VK_NULL_HANDLE);
}

// -------------------------------------------------------------------------------------------- //

void Mesh::Init(GFContext * c){
	if (this->vertices.size() == 0) {
		return;
	}

	this->context = c;

	VkDeviceSize bufferSize = sizeof(Vertex) * this->vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(this->context->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, this->vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(this->context->GetDevice(), stagingBufferMemory);

	this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	this->context->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(this->context->GetDevice(), stagingBuffer, VK_ALLOCK);
	vkFreeMemory(this->context->GetDevice(), stagingBufferMemory, VK_ALLOCK);

	if (this->index.size() != 0) {
		VkDeviceSize bufferSize = sizeof(uint16_t) * this->index.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(this->context->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->index.data(), (size_t)bufferSize);
		vkUnmapMemory(this->context->GetDevice(), stagingBufferMemory);

		this->context->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		this->context->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(this->context->GetDevice(), stagingBuffer, VK_ALLOCK);
		vkFreeMemory(this->context->GetDevice(), stagingBufferMemory, VK_ALLOCK);
	}

}

// -------------------------------------------------------------------------------------------- //

void Mesh::Draw(VkCommandBuffer commandBuffer){
	if (this == nullptr) {
		return;
	}

	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	if (this->index.size() != 0) {
		vkCmdBindIndexBuffer(commandBuffer, this->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(commandBuffer, this->index.size(), 1, 0, 0, 0);
	}
	else {
		vkCmdDraw(commandBuffer, this->vertices.size(), 1, 0, 0);
	}
}

 // -------------------------------------------------------------------------------------------- //

void Mesh::Read(ifstream * f){
	bool d;
	f->read((char*)&d, 1);

	if (d) {
		char* _id = new char[32];
		f->read(_id, 32);
		_id[32] = '\0';
		this->id = _id;
	}
	else {
		unsigned int size;
		f->read((char*)&size, sizeof(unsigned int));

		for (unsigned int i = 0; i < size; i++){
			char type;
			f->read(&type, 1);

		}
	}
}

// -------------------------------------------------------------------------------------------- //

void Mesh::Write(ofstream * f){
	/*if (this->id == "") {
		bool d = false;
		f->write((char*)&d, 1);

		unsigned int size = 0;
		size += (this->Vertex != nullptr ? 1 : 0);
		size += (this->UV != nullptr ? 1 : 0);
		size += (this->Normal != nullptr ? 1 : 0);
		size += (this->Index != nullptr ? 1 : 0);

		f->write((char*)&size, sizeof(unsigned int));

		if (this->Vertex != nullptr) {
			f->write("V", 1);
			unsigned int d_size = this->Size;
			f->write((char*)&d_size, sizeof(unsigned int));
			f->write((char*)this->Vertex, sizeof(vec3) * d_size);
		}

		if (this->UV != nullptr) {
			f->write("U", 1);
			unsigned int d_size = this->Size;
			f->write((char*)&d_size, sizeof(unsigned int));
			f->write((char*)this->UV, sizeof(vec2) * d_size);
		}

		if (this->Normal != nullptr) {
			f->write("N", 1);
			unsigned int d_size = this->Size;
			f->write((char*)&d_size, sizeof(unsigned int));
			f->write((char*)this->Normal, sizeof(vec3) * d_size);
		}

		if (this->Index != nullptr) {
			f->write("I", 1);
			unsigned int d_size =this->IndexSize;
			f->write((char*)&d_size, sizeof(unsigned int));
			f->write((char*)this->Index, d_size);
		}

	}
	else {
		bool d = true;
		f->write((char*)&d, 1);
		f->write(_strdup(this->id.c_str()), 32);
	}*/
}
