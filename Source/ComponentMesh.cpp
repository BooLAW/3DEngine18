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
	//The problem is with the vertices and the num vertices
	mesh->bounding_box.Enclose(mesh->vertices, mesh->num_vertices/3);
	
	mesh->bounding_box.TransformAsAABB(global_matrix);
	

}

bool ComponentMesh::FirstPoint(LineSegment mouse_ray, float3 & firstpoint, float & point_distance)
{
	ComponentTransform* owner_transform = owner->comp_transform;
	bool ret = false;
	float closest_distance = VERYFAR;
	float distance;
	float3 coll_point;
	bool colliding = false;
	Triangle triangle_to_check;
	if (mesh == nullptr || mesh->vertices == nullptr)
		return ret;
	
	//Translate point
	mouse_ray.Transform(owner_transform->trans_matrix_g.Inverted());
	//Check and and fill it
	for (int i = 0; i < mesh->num_indices; i += 3)
	{  
		triangle_to_check = CreateTriangleFromIndices(i);
		colliding = mouse_ray.Intersects(triangle_to_check, &distance, &coll_point);
		if (colliding == true)
		{
			distance = mouse_ray.a.Distance(coll_point);
			if (distance < closest_distance)
			{
				firstpoint = coll_point;
				closest_distance = distance;
				ret = true;
			}
		}
	}
	firstpoint += owner_transform->trans_matrix_g.TranslatePart();
	point_distance = closest_distance;
	return ret;
}

Triangle ComponentMesh::CreateTriangleFromIndices(int i)
{
	Triangle ret; 
	float3 a = mesh->vertices[mesh->indices[i]];
	float3 b = mesh->vertices[mesh->indices[i + 1]];
	float3 c = mesh->vertices[mesh->indices[i + 2]];

	ret.a = a;
	ret.b = b;
	ret.c = c;

	return ret;
}



ComponentMesh::~ComponentMesh()
{
}
