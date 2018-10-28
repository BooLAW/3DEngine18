#include "Octree.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "DebugDraw.h"


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
	for (int i = 0; i < OCTREECHILDS; i++)
	{
		childs[i] = nullptr;
	}
}

void OctreeItem::ClearItems()
{
	for (int i = 0; i < OCTREECHILDS; i++)
	{
		RELEASE(childs[i]);
	}
}

bool OctreeItem::IsItemFull() const
{
	bool ret = item_elements.size() == 4;
	return ret;
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
	//iterator to make the new 8 boxes(2 to the 3)
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
			for (int i = 0; i < OCTREECHILDS; i++)
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
	root_item = new OctreeItem(new_box);
	min_point = min;
	max_point = max;
	update_quadtree = true;
}

void Octree::Clear()
{
	RELEASE(root_item);
}

void Octree::DrawOctree(bool active)
{
	if (!active)
		return;
	else
	{
		if (root_item != nullptr)
		{
			root_item->Draw();
		}
	}
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

void Octree::Insert(Mesh * mesh)
{
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

void Octree::CollectIntersections(std::list<Mesh*> intersections, AABB * bounding_box)
{
	if (root_item != nullptr)
	{
		if (bounding_box->Intersects(root_item->item_box))
		{
			root_item->CollectIntersections(intersections, bounding_box);
		}
	}
}

void Octree::Recalculate(float3 min, float3 max)
{

	if (min_point.Equals(min) && max_point.Equals(max))
	{
		min_point = min;
		max_point = max;
	}
	else
		ExpandBox(min,max);

	
}

void Octree::ExpandBox(float3 min, float3 max)
{

	if (min.x < min_point.x)
	{
		min_point.x = min.x;
	}
	if (min.y < min_point.y)
	{
		min_point.y = min.y;
	}
	if (min.z < min_point.z)
	{
		min_point.z = min.z;
	}
	if (max.x > max_point.x)
	{
		max_point.x = max.x;
	}
	if (max.y > max_point.y)
	{
		max_point.y = max.y;
	}
	if (max.z > max_point.z)
	{
		max_point.z = max.z;
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

void OctreeItem::Draw()
{
	if (HasChilds())
	{
		for (int i = 0; i < 8; i++)
		{
			static float3 points[8];//is a box
			item_box.GetCornerPoints(points);
			BoxDD(points,Blue);
		}
		
	}

}

bool OctreeItem::HasChilds() const
{
	return childs[0] != nullptr;
}
