#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"



GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}

void GameObject::Draw()
{
	if (!active)
		return;
	if (!components_list.empty())
	{
		for (int i = 0; i < components_list.size(); i++)
		{
			components_list[i]->Update();
		}
	}
}

AABB GameObject::GetBB()
{
	ComponentMesh* aux;
	if (HasMesh())
	{
		aux = (ComponentMesh*)GetComponent(MESH);
		return aux->mesh->bounding_box;
	}
	else
		CONSOLE_LOG("Can't return BB without a mesh");
}

Component * GameObject::GetComponent(ComponentType type)
{
	for (std::vector<Component*>::iterator item = components_list.begin(); item != components_list.end(); item++)
	{
		if ((*item)->type == type)
			return (*item);
	}
	return nullptr;
}

void GameObject::PushComponent(Component * new_component)
{
}

void GameObject::RemoveComponent(ComponentType type)
{

}

bool GameObject::HasMesh() const
{
	bool ret = false;
	for (int i = 0; i < components_list.size(); i++)
	{
		if (components_list[i]->type == ComponentType::MESH)
			ret = true;
	}
	return ret;
}
