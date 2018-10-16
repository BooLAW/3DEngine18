#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "Material.h"
#include "OpenGL.h"
#include "DebugDraw.h"



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
	if (!childs_list.empty())
	{
		for (std::vector<GameObject*>::iterator  it = childs_list.begin(); it != childs_list.end(); it++)
		{
			//Enable Client
			glEnableClientState(GL_VERTEX_ARRAY);
			ComponentMesh* aux_mesh = (ComponentMesh*)(*it)->GetComponent(MESH);
			ComponentMaterial* aux_material = (ComponentMaterial*)(*it)->GetComponent(MATERIAL);
			
				//glPushMatrix();
				//glMultMatrixf((*it)->transform->trans_matrix_l.Transposed().ptr());
			//Bind Vertices
			if (aux_mesh != nullptr)
			{
				glBindBuffer(GL_ARRAY_BUFFER, aux_mesh->mesh->vertices_id);
				glVertexPointer(3, GL_FLOAT, 0, NULL);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aux_mesh->mesh->indices_id);

				//Draw
				if (App->renderer3D->attributes.debug_draw_atribute) 
				{
					DebugDrawing(aux_mesh->mesh, Red);
				}
				//BindMaterial

				if (aux_mesh->mesh->num_tex_coords != 0)
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glBindBuffer(GL_ARRAY_BUFFER, aux_mesh->mesh->tex_coords_id);
					glTexCoordPointer(3, GL_FLOAT, 0, NULL);
				}
				//Bind Indices
				if (aux_material != nullptr && App->renderer3D->attributes.texture == true)
					glBindTexture(GL_TEXTURE_2D, (GLuint)aux_material->data->textures_id);

				glDrawElements(GL_TRIANGLES, aux_mesh->mesh->num_indices, GL_UNSIGNED_INT, NULL);
			}
			
			
			
			//Unbind
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			//Disable Client
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				//glPopMatrix();
			
			
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
		CONSOLE_LOG_WARNING("Can't return BB without a mesh");
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
	components_list.push_back(new_component);
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

bool GameObject::HasTex() const
{
	bool ret = false;
	for (int i = 0; i < components_list.size(); i++)
	{
		if (components_list[i]->type == ComponentType::MATERIAL)
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
	//float3 parent_pos = GetBB().CenterPoint() - new_parent->GetBB().CenterPoint();
	//PAU Update transform
	new_parent->SetChild(this);
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

void GameObject::SetChild(GameObject * child)
{
	if (child != nullptr)
	{
		child->parent = this;
		childs_list.push_back(child);
	}
}

GameObject * GameObject::GetChild(int id)
{
	return childs_list[id];
}

void GameObject::ActivateBB()
{
	if (this->IsActive())
	{
		ComponentMesh* mesh_tmp = (ComponentMesh*)GetComponent(ComponentType::MESH);
		if(mesh_tmp != nullptr)
			mesh_tmp->mesh->show_bb = !mesh_tmp->mesh->show_bb;
	}
}

Mesh * GameObject::GetMesh()
{
	if (this->IsActive())
	{
		ComponentMesh* mesh_tmp = (ComponentMesh*)GetComponent(ComponentType::MESH);
		if (mesh_tmp != nullptr)
			return mesh_tmp->mesh;
		else
			return nullptr;
	}
}

void GameObject::RecursiveUpdateTransformChilds()
{
	//PAUTODO
	//Make a function to update recursively all the childs and components related

}

const char * GameObject::GetName() const
{
	return name.c_str();
}

void GameObject::SetName(const char * name)
{
	this->name = name;
}
