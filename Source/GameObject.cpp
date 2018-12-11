#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "Material.h"
#include "OpenGL.h"
#include "DebugDraw.h"
#include "Camera.h"
#include "ComponentCamera.h"


GameObject::GameObject()
{
	parent = nullptr;
	active = true;
	static_go = false;
	root_go = false;

	ComponentTransform* new_transform = new ComponentTransform(this);
	PushComponent(new_transform);

	comp_transform = new_transform;
	
	new_transform->type = ComponentType::TRANSFORM;
}


GameObject::~GameObject()
{

}

void GameObject::Draw()
{
	if (!active)
		return;

	//Enable Client
	glEnableClientState(GL_VERTEX_ARRAY);
	ComponentMesh* aux_mesh = (ComponentMesh*)GetComponent(MESH);
	ComponentMaterial* aux_material = (ComponentMaterial*)GetComponent(MATERIAL);
	ComponentTransform* aux_trans = comp_transform;
	
	float4x4 v_matrix = float4x4::identity;
	//Similar to Pushing a matrix
	if (aux_trans != nullptr)
	{
		GLfloat matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		v_matrix.Set((float*)matrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((GLfloat*)((aux_trans->trans_matrix_g).Transposed() * v_matrix).v);

		
	}
	//Bind Vertices
	if (aux_mesh != nullptr)
	{
		glBindBuffer(GL_ARRAY_BUFFER, aux_mesh->mesh->vertices_id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aux_mesh->mesh->indices_id);

		//BindMaterial
		if (aux_mesh->mesh->num_tex_coords != 0)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, aux_mesh->mesh->tex_coords_id);
			glTexCoordPointer(3, GL_FLOAT, 0, NULL);			
		}
		//Bind Indices
		if (aux_material != nullptr && App->renderer3D->attributes.texture == true)
		{						
			glBindTexture(GL_TEXTURE_2D, (GLuint)aux_material->data->textures_id);								
		}
		glColor3f(aux_mesh->mesh->color.x, aux_mesh->mesh->color.y, aux_mesh->mesh->color.z);
		glDrawElements(GL_TRIANGLES, aux_mesh->mesh->num_indices, GL_UNSIGNED_INT, NULL);
		
	}
	
	//"Poping" Global Matrix
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)v_matrix.v);
	//Unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Disable Client
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	if (IsSelected() && HasMesh())
		DrawBB();
	if (draw_normals)
	{
		DrawNormals();
	}
	

}

void GameObject::DrawBB()
{
	if (HasMesh())
	{
		DebugDrawing(GetMesh(), Red);
	}

}

void GameObject::DrawNormals()
{
	DebugDrawNormals(GetMesh(), Blue);
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

bool GameObject::HasCam() const
{
	bool ret = false;
	for (int i = 0; i < components_list.size(); i++)
	{
		if (components_list[i]->type == ComponentType::CAMERA)
			ret = true;
	}
	return ret;
}

bool GameObject::HasChilds() const
{
	return childs_list.empty() == false;
}

bool GameObject::HasController() const
{
	bool ret = false;
	for (int i = 0; i < components_list.size(); i++)
	{
		if (components_list[i]->type == ComponentType::CONTROLLER)
			ret = true;
	}
	return ret;
}

bool GameObject::HasRigidBody() const
{
	bool ret = false;
	for (int i = 0; i < components_list.size(); i++)
	{
		if (components_list[i]->type == ComponentType::RIGIDBODY)
			ret = true;
	}
	return ret;

}

bool GameObject::HasCollider() const
{
	bool ret = false;
	for (int i = 0; i < components_list.size(); i++)
	{
		if (components_list[i]->type == ComponentType::COLLIDER)
			ret = true;
	}
	return ret;
}

bool GameObject::HasPhysBody() const
{
	if (physbody != nullptr)
	{
		return true;
	}
	else
		return false;
	
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
		{
			childs_list[i]->SetStatic(is_static);
			if (is_static)
			{
				App->scene_intro->AddToOctree(childs_list[i]);
			}
			else
			{
				App->scene_intro->RemoveFromOctree(childs_list[i]);
			}
		}
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

bool GameObject::IsSelected() const
{
	return selected;
}

void GameObject::SetSelected(bool selected)
{
	this->selected = selected;
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
	parent_uid = new_parent->uid;
}

void GameObject::SetParent(std::vector<GameObject*> go_list, UINT32 parent_uid)
{
	for (std::vector<GameObject*>::iterator it = go_list.begin(); it != go_list.end(); it++)
	{
		//Find the parent uid in the go_list
		if ((*it)->GetUID() == parent_uid)
		{
			//Check if you don't have a parent assigned
			if (parent == NULL)
			{								
				const char* name_go = GetName();
				//Assign the parent of the GO;
				parent = (*it);
				(*it)->AddChild(this);
					
				
			}
		}		
	}
}



void GameObject::AddChild(GameObject * new_child)
{
	if (new_child != nullptr)
	{
		new_child->SetParent(this);
	}
}

void GameObject::AddChild(std::vector<GameObject*> go_list, UINT32 uid)
{
	for (std::vector<GameObject*>::iterator it = go_list.begin(); it != go_list.end(); it++)
	{
		if ((*it)->GetUID() == uid)
		{
			(*it)->SetChild(this);
			SetChild((*it));
		}
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

void GameObject::DeleteChild(GameObject * child)
{
	for (std::vector<GameObject*>::iterator it = childs_list.begin(); it != childs_list.end(); it++)
	{
		if ((*it) == child)
		{
			childs_list.erase(it);
			return;
		}
	}
}

int GameObject::GetNumChilds() const
{
	return childs_list.size();
}

void GameObject::SetParentUID(UINT32 id)
{
	parent_uid = id;
}

void GameObject::SetUID(UINT32 id)
{
	uid = id;
}

Camera * GameObject::GetCamera()
{
	ComponentCamera* aux = (ComponentCamera*)this->GetComponent(ComponentType::CAMERA);
	if (aux != nullptr)
		return aux->cam;
	
	return nullptr;
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

ComponentMesh * GameObject::GetCMesh()
{
	if (this->IsActive())
	{
		ComponentMesh* mesh_tmp = (ComponentMesh*)GetComponent(ComponentType::MESH);
		if (mesh_tmp != nullptr)
			return mesh_tmp;
		else
			return nullptr;
	}
}

ComponentCamera * GameObject::GetCCamera()
{
	if (this->IsActive())
	{
		ComponentCamera* cam_tmp = (ComponentCamera*)GetComponent(ComponentType::CAMERA);
		if (cam_tmp != nullptr)
			return cam_tmp;
		else
			return nullptr;
	}
}

ComponentRigidBody * GameObject::GetRigidBody()
{
	if (this->IsActive())
	{
		ComponentRigidBody* rb_tmp = (ComponentRigidBody*)GetComponent(ComponentType::RIGIDBODY);
		if (rb_tmp != nullptr)
			return rb_tmp;
		else
			return nullptr;
	}
}

ComponentCollider * GameObject::GetCollider()
{
	if (this->IsActive())
	{
		ComponentCollider* rb_tmp = (ComponentCollider*)GetComponent(ComponentType::COLLIDER);
		if (rb_tmp != nullptr)
			return rb_tmp;
		else
			return nullptr;
	}
}

Material * GameObject::GetMaterial()
{
	ComponentMaterial* mat_tmp = (ComponentMaterial*)GetComponent(ComponentType::MATERIAL);
	if (mat_tmp != nullptr)
		return mat_tmp->data;
	else
		return nullptr;
}

void GameObject::RecursiveUpdateTransformChilds()
{
	//Make a function to update recursively all the childs and components related
	for (int i = 0; i < GetNumChilds(); i++)
	{
		childs_list[i]->comp_transform->CalculateGlobalMatrix();
	}

}

void GameObject::RecalculateBoundingBox(GameObject* child)
{
	Mesh* mesh = child->GetMesh();
	ComponentTransform* c_transform = child->comp_transform;
	if (mesh == nullptr)
	{
		CONSOLE_LOG_DEBUG("Can't RecalculateBB, mesh == nullptr");
		return;
	}
}

void GameObject::RecursiveRecalculateBoundingBox(float4x4 transform, GameObject * go)
{
	//recalculate me
	RecalculateBoundingBox(go);
	//recalculate my childs
	if (go->HasChilds())
	{
		for (int i = 0; i < go->GetNumChilds(); i++)
		{
			ComponentTransform* trans = go->comp_transform;
			Mesh*  mesh = go->GetMesh();
			if (HasMesh())
			{
				RecursiveRecalculateBoundingBox(trans->trans_matrix_g, childs_list[i]);
				trans->updated_transform = false;
			}
		}
	}

}

const char * GameObject::GetName() const
{
	return name.c_str();
}

void GameObject::SetName(const char * name)
{
	this->name = name;
}

void GameObject::ClearRelations()
{
	for (int i = 0; i < childs_list.size(); i++)
	{
		childs_list[i]->parent = nullptr;

		if (childs_list[i]->HasChilds())
			childs_list[i]->ClearRelations();
		else
		{
			childs_list[i] = nullptr;
			
		}
	}
	childs_list.clear();
}

UINT32 GameObject::GetUID() const
{
	return uid;
}

UINT32 GameObject::GetParentUID() const
{
	return parent_uid;
}

void GameObject::SetRootGoFlag(bool flag)
{
	root_go = flag;
}

void GameObject::SetFirstUpdate(bool flag)
{
	first_update = flag;
}

void GameObject::SetNumMeshes(uint num)
{
	num_meshes = num;
}

uint GameObject::GetNumMeshes() const
{
	return num_meshes;
}

bool GameObject::GetFirstUpdate() const
{
	return first_update;
}

void GameObject::SetDrawNormals(bool flag)
{
	draw_normals = flag;
}

bool GameObject::GetDrawNormals() const
{
	return draw_normals;
}
