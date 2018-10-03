#include "ComponentMesh.h"
#include "GameObject.h"


ComponentMesh::ComponentMesh()
{
	type = ComponentType::MESH;
}

ComponentMesh::ComponentMesh(GameObject * new_owner)
{
	owner = new_owner;
}

void ComponentMesh::AddMesh(Mesh * new_mesh)
{
	mesh = new_mesh;
}


ComponentMesh::~ComponentMesh()
{
}
