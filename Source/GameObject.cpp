#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"



GameObject::GameObject()
{
	//Generate unique id

	parent = nullptr;
	active = true;
	static_go = false;
	root_go = false;

	ComponentTransform* new_transform = new ComponentTransform(this);
	PushComponent(new_transform);

	transform = new_transform;

	new_transform->type = ComponentType::TANSFORM;
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

void GameObject::SetActive(bool active)
{
	this->active = active;
	//recursively activate all childs
	if (childs_list.size() > 0)
	{
		for (int i = 0; i < childs_list.size(); i++)
			childs_list[i]->SetActive(active);
	}
	
}

bool GameObject::IsActive() const
{
	return active;
}

void GameObject::SetStatic(bool is_static)
{
	this->static_go = is_static;
	//recursively set static all childs
	if (childs_list.size() > 0)
	{
		for (int i = 0; i < childs_list.size(); i++)
			childs_list[i]->SetStatic(is_static);
	}
}

bool GameObject::IsStatic() const
{
	return static_go;
}

bool GameObject::IsRoot() const
{
	return root_go;
}

GameObject * GameObject::GetParent()const
{
	return parent;
}

void GameObject::SetParent(GameObject * new_parent)
{
	float3 parent_pos = GetBB().CenterPoint() - new_parent->GetBB().CenterPoint();
	//PAU Update transform
	new_parent->AddChild(this);
	parent = new_parent;
}

void GameObject::AddChild(GameObject * new_child)
{
	if (new_child != nullptr)
	{
		new_child->SetParent(this);
		childs_list.push_back(new_child);
	}
}

GameObject * GameObject::GetChild(int id)
{
	return childs_list[id];
}

const char * GameObject::GetName() const
{
	return name.c_str();
}

void GameObject::SetName(const char * name)
{
	this->name = name;
}
