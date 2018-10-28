#include "Quadtree.h"
#include "GameObject.h"
#include "ComponentMesh.h"


OctreeItem::OctreeItem(AABB& box)
{
	SetChildsNull();
	item_box = box;
}

OctreeItem::~OctreeItem()
{
	ClearItems();
}

void OctreeItem::SetChildsNull()
{
	for (int i = 0; i < QUADTREECHILDS; i++)
	{
		childs[i] = nullptr;
	}
}

void OctreeItem::ClearItems()
{
	for (int i = 0; i < QUADTREECHILDS; i++)
	{
		RELEASE(childs[i]);
	}
}

bool OctreeItem::IsItemFull() const
{
	return item_elements.size() == 4;
}

void OctreeItem::InsertItem(Mesh * mesh_to_insert)
{
	if (mesh_to_insert == nullptr) return;
	if (childs[0] == nullptr)
	{
		if (!IsItemFull())
		{
			item_elements.push_back(mesh_to_insert);
		}
		else
		{
			SubdivideItem();
		}
	}
	//recursive function
	if (childs[0] != nullptr)
	{
		for (int i = 0; i < 8; i++)
		{
			if (childs[i]->item_box.Intersects(mesh_to_insert->bounding_box))
			{
				childs[i]->InsertItem(mesh_to_insert);
				break;
			}
		}
	}
}

void OctreeItem::SubdivideItem()
{
	AABB new_box;
	float3 center_point = item_box.CenterPoint();
	float3 length = item_box.Size() / 2;

	int id = 0;
	//iterator to make the new 8 boxes
	for (int ix = 0; ix < 2; ix++)
	{
		for (int iy = 0; iy < 2; iy++)
		{
			for (int iz = 0; iz < 2; iz++)
			{
				float3 min_point(item_box.minPoint.x + ix * length.x , item_box.minPoint.y + iy * length.y, item_box.minPoint.z + iz * length.z);
				float3 max_point(min_point.x + length.x,min_point.y + length.y,min_point.z + length.z);

				new_box.minPoint = min_point;
				new_box.maxPoint = max_point;
				childs[id] = new OctreeItem(new_box);
				id++;
			}
		}
	}

	for (std::list<Mesh*>::iterator item = item_elements.begin(); item != item_elements.end(); item++)
	{
		if ((*item) != nullptr)
		{
			for (int i = 0; i < QUADTREECHILDS; i++)
			{
				if (childs[i]->item_box.Intersects((*item)->bounding_box))
				{
					childs[i]->InsertItem((*item));
				}
			}
			item = item_elements.erase(item);
		}
		else
		{
			item++;
		}
	}
}

Octree::Octree()
{
	update_quadtree = false;
}


Octree::~Octree()
{
	Clear();
}

void Octree::Create(float3 min,float3 max)
{
	AABB new_box(min, max);
	root_item->item_box = new_box;
	min_point = min;
	max_point = max;
	update_quadtree = true;
}

void Octree::Clear()
{
	RELEASE(root_item);
}

void Octree::Insert(GameObject * go_to_insert)
{
	if (go_to_insert == nullptr || go_to_insert->HasMesh() == false)
		return;
	update_quadtree = false;
	Mesh* mesh = go_to_insert->GetMesh();
	if (root_item != nullptr)
	{
		//Check if it's without the limits
		if (mesh->bounding_box.minPoint.x < min_point.x)
		{
			min_point.x = mesh->bounding_box.minPoint.x;
			update_quadtree = true;
		}
		if (mesh->bounding_box.minPoint.y < min_point.y)
		{
			min_point.y = mesh->bounding_box.minPoint.y;
			update_quadtree = true;
		}
		if (mesh->bounding_box.minPoint.z < min_point.z)
		{
			min_point.z = mesh->bounding_box.minPoint.z;
			update_quadtree = true;
		}
		if (mesh->bounding_box.maxPoint.x > max_point.x)
		{
			max_point.x = mesh->bounding_box.maxPoint.x;
			update_quadtree = true;
		}
		if (mesh->bounding_box.maxPoint.y > max_point.y)
		{
			max_point.y = mesh->bounding_box.maxPoint.y;
			update_quadtree = true;
		}
		if (mesh->bounding_box.maxPoint.z > max_point.z)
		{
			max_point.z = mesh->bounding_box.maxPoint.z;
			update_quadtree = true;
		}
		//Add it to the root node
		if (update_quadtree == false)
		{
			root_item->InsertItem(mesh);
		}

	}
		
}

void OctreeItem::CollectIntersections(std::list<Mesh*> intersections, AABB * bounding_box)
{
	if (childs[0] != nullptr)
	{
		for (int i = 0; i < 8; i++)
		{
			if (childs[i]->item_box.Intersects(*bounding_box))
			{
				childs[i]->CollectIntersections(intersections, bounding_box);
			}
		}
	}

	for (std::list<Mesh*>::iterator it = item_elements.begin(); it != item_elements.end(); it++)
	{
		if ((*it) == nullptr) continue;
		if ((*it)->bounding_box.Intersects(*bounding_box))
		{
			intersections.push_back(*it);
		}
	}
}