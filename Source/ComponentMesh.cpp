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

}

void ComponentMesh::DrawInspectorInfo()
{
}

void ComponentMesh::CleanUp()
{
}

void ComponentMesh::UpdateBoundingBox(float4x4 global_matrix)
{
	mesh->bounding_box.SetNegativeInfinity();
	mesh->bounding_box.Enclose((float3*)mesh->vertices, mesh->num_vertices);
	mesh->bounding_box.Transform(global_matrix);
	
}


ComponentMesh::~ComponentMesh()
{
}
