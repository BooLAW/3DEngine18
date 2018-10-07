#include "ComponentMaterial.h"
#include "Material.h"
#include "GameObject.h"
#include "OpenGL.h"



ComponentMaterial::ComponentMaterial()
{
	type = ComponentType::MATERIAL;

	//allocate buffer for the component


	void* a = this;
	void** a_ptr = &a;
	uint size = sizeof(this);
	char* data = new char[size];
	memcpy(data, a_ptr, size);
}

ComponentMaterial::ComponentMaterial(GameObject * owner)
{
	this->owner = owner;
	type = ComponentType::MATERIAL;

	//allocate buffer for the component

	void* a = this;
	void** a_ptr = &a;
	uint size = sizeof(this);
	char* data = new char[size];
	memcpy(data, a_ptr, size);


}


ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Enable()
{
	SetActive(true);
}

void ComponentMaterial::Disable()
{
	SetActive(false);
}

void ComponentMaterial::InitTexture()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, data->textures_id);
}

void ComponentMaterial::EndTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
