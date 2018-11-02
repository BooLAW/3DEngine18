#include "Octree.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "DebugDraw.h"
#include "Mesh.h"


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
	item_elements.clear();
}

void OctreeItem::ClearItems()
{
	if (HasChilds())
	{
		for (int i = 0; i < OCTREECHILDS; i++)
		{
			RELEASE(childs[i]);
		}
	}
	
	item_elements.clear();
}

bool OctreeItem::IsItemFull() const
{
	bool ret = item_elements.size() == 4;
	return ret;
}

void OctreeItem::InsertItem(Mesh * mesh_to_insert)
{
	if (mesh_to_insert == nullptr) return;
	if (HasChilds())
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
	//recursive function
	else if (!HasChilds())
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
}

void OctreeItem::RemoveItem()
{
	if (HasChilds())
	{
		item_elements.clear();
		parent = nullptr;

		delete(this);
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			childs[i]->RemoveItem();
		}
	}
}

void OctreeItem::SubdivideItem()
{
	if (!last)
	{
		for (int i = 0; i < OCTREECHILDS; i++)
		{
			childs[i]->SubdivideItem();
		}
	}
	else
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
					float3 min_point(item_box.minPoint.x + iz * length.x, item_box.minPoint.y + iy * length.y, item_box.minPoint.z + ix * length.z);
					float3 max_point(min_point.x + length.x, min_point.y + length.y, min_point.z + length.z);

					new_box.minPoint = min_point;
					new_box.maxPoint = max_point;
					childs[id] = new OctreeItem(new_box);
					childs[id]->parent = this;
					id++;
				}
			}
		}

		for (int i = 0; i < OCTREECHILDS; i++)
		{
			for (int j = 0; j < item_elements.size(); j++)
			{
				//Check wheter it intersects or it's inside our bb
				if (childs[i]->item_box.Contains(item_elements[j]->bounding_box) || childs[i]->item_box.Intersects(item_elements[j]->bounding_box))
				{
					childs[i]->item_elements.push_back(item_elements[j]);
				}
				if (childs[i]->IsItemFull())
					childs[i]->SubdivideItem();
			}
		}
		
		for (int i = 0; i < item_elements.size(); i++)
		{
			item_elements.erase(item_elements.begin() + i);
			i--;
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
		root_item->Draw();
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

void Octree::Remove(GameObject * go_to_insert)
{

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

void Octree::Divide()
{
	if (root_item->last)
	{
		root_item->SubdivideItem();
	}
	else
	{
		for (int i = 0; i < OCTREECHILDS; i++)
		{
			root_item->childs[i]->SubdivideItem();
		}
	}
}

void OctreeItem::CollectIntersections(std::list<Mesh*> intersections, AABB * bounding_box)
{
	if (HasChilds())
	{
		for (int i = 0; i < OCTREECHILDS; i++)
		{
			childs[i]->CollectIntersections(intersections, bounding_box);
		
		}
	}

	for (std::vector<Mesh*>::iterator it = item_elements.begin(); it != item_elements.end(); it++)
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
	static float3 points[8];//is a box
	item_box.GetCornerPoints(points);
	BoxDD(points, Blue);

	if (HasChilds())
	{
		for (int i = 0; i < 8; i++)
		{
			childs[i]->Draw();
		}
		
	}

}

bool OctreeItem::HasChilds() const
{
	return childs[0] != nullptr;
}
