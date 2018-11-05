#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "GameObject.h"
#include "OpenGL.h"
#include "Globals.h"
#include "DebugDraw.h"
#include "Material.h"
#include "ComponentTransform.h"

ComponentMesh::ComponentMesh()
{
	type = ComponentType::MESH;
	SetName("Component Mesh");
}

ComponentMesh::ComponentMesh(GameObject * new_owner)
{
	owner = new_owner;
}

void ComponentMesh::AddMesh(Mesh * new_mesh)
{
	mesh = new_mesh;
}

bool ComponentMesh::Update()
{
	if (active == false)
		return false;
	ComponentTransform* owner_trnasform = owner->comp_transform;
	if (owner_trnasform->updated_transform)
	{
		UpdateBoundingBox();
		owner_trnasform->updated_transform = false;
	}
	
}

void ComponentMesh::DrawInspectorInfo()
{
}

void ComponentMesh::CleanUp()
{
}

void ComponentMesh::UpdateBoundingBox()
{
	ComponentTransform* owner_transform = (ComponentTransform*)owner->GetComponent(ComponentType::TRANSFORM);

	if (owner_transform != nullptr)
	{
		mesh->bounding_box.SetNegativeInfinity();
		mesh->bounding_box = mesh->bounding_box.MinimalEnclosingAABB(mesh->vertices, mesh->num_vertices);
		mesh->bounding_box.TransformAsAABB(owner_transform->trans_matrix_g.Transposed());
	}
}


ComponentMesh::~ComponentMesh()
{
}
