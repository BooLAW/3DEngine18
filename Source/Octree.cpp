#include "Octree.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "DebugDraw.h"
#include "Mesh.h"
#include "Application.h"
#include"ModuleScene.h"
#include"Globals.h"



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
		for (int i = 0; i < OCTREECHILDS; i++)
		{
			if (childs[i]->item_box.Intersects(mesh_to_insert->bounding_box))
			{
				childs[i]->InsertItem(mesh_to_insert);
				break;
			}
		}
		
	}
	//recursive function
	if(!HasChilds())
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

void OctreeItem::RemoveItem(Mesh* mesh_to_remove)
{
	if (mesh_to_remove == nullptr)
	{
		CONSOLE_LOG_INFO("Octree mesh was nullptr, can't remove it");
		return;
	}

	for (std::vector<Mesh*>::iterator it = item_elements.begin(); it != item_elements.end(); it++)
	{
		if ((*it) == mesh_to_remove)
		{
			item_elements.erase(it);
			break;
		}
		
	}
	if (HasChilds())
	{
		int childs_contents_num = 0;
		for (int i = 0; i < OCTREECHILDS; i++)
		{
			childs[i]->RemoveItem(mesh_to_remove);
			childs_contents_num += childs[i]->item_elements.size();
		}
		if (childs_contents_num == 0)
			ClearItems();
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
					float3 min_point(item_box.minPoint.x + iz * length.x, item_box.minPoint.y + iy * length.y, item_box.minPoint.z + ix * length.z);
					float3 max_point(min_point.x + length.x, min_point.y + length.y, min_point.z + length.z);

					new_box.minPoint = min_point;
					new_box.maxPoint = max_point;
					childs[id] = new OctreeItem(new_box);
					id++;
				}
			}
		}

		for (std::vector<Mesh*>::iterator it = item_elements.begin(); it != item_elements.end();)
		{
			if ((*it) != nullptr)
			{
				for (int i = 0; i < OCTREECHILDS; i++)
				{
					if (childs[i]->item_box.Intersects((*it)->bounding_box.ToOBB().MinimalEnclosingAABB()))
					{
						childs[i]->InsertItem(*it);
					}
				}
				it = item_elements.erase(it);
			}
			else
			{
				it++;
			}
		}
	
	
}

Octree::Octree()
{
	update_octree = false;
}


Octree::~Octree()
{
	Clear();
}

void Octree::Create(float3 min,float3 max)
{
	Clear();

	AABB new_box(min, max);
	root_item = new OctreeItem(new_box);
	min_point = min;
	max_point = max;
	update_octree = true;
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
		root_item->Draw();
	
}

void Octree::Insert(GameObject * go_to_insert)
{
	if (go_to_insert == nullptr || go_to_insert->HasMesh() == false)
		return;
	update_octree = false;
	Mesh* mesh = go_to_insert->GetMesh();
	if (root_item != nullptr)
	{
		//Check if it's without the limits
		update_octree = CheckNoResizeToInsert(mesh);

		//Add it to the root node
		if (update_octree == false)
			root_item->InsertItem(mesh);
		

	}
}

void Octree::Insert(Mesh * mesh)
{
	if (root_item != nullptr)
	{
		//Check if it's without the limits
		update_octree = CheckNoResizeToInsert(mesh);
		//Add it to the root node
		if (update_octree == false)
			root_item->InsertItem(mesh);
		
	}
}

bool Octree::CheckNoResizeToInsert(Mesh* mesh)
{
	bool ret = false;

	if (mesh->bounding_box.minPoint.x < min_point.x)
	{
		min_point.x = mesh->bounding_box.minPoint.x;
		ret = true;
	}
	if (mesh->bounding_box.minPoint.y < min_point.y)
	{
		min_point.y = mesh->bounding_box.minPoint.y;
		ret = true;
	}
	if (mesh->bounding_box.minPoint.z < min_point.z)
	{
		min_point.z = mesh->bounding_box.minPoint.z;
		ret = true;
	}
	if (mesh->bounding_box.maxPoint.x > max_point.x)
	{
		max_point.x = mesh->bounding_box.maxPoint.x;
		ret = true;
	}
	if (mesh->bounding_box.maxPoint.y > max_point.y)
	{
		max_point.y = mesh->bounding_box.maxPoint.y;
		ret = true;
	}
	if (mesh->bounding_box.maxPoint.z > max_point.z)
	{
		max_point.z = mesh->bounding_box.maxPoint.z;
		update_octree = true;
	}



	return ret;
}

void Octree::Remove(GameObject * go_to_remove)
{
	if (root_item != nullptr && go_to_remove->HasMesh())
	{
		update_octree = go_to_remove->GetBB().minPoint.Equals(min_point) && go_to_remove->GetBB().maxPoint.Equals(max_point);
		//If we need to update the octree, we will handle that in the scene update if not, just erase the box
		if (!update_octree)
		{
			root_item->RemoveItem(go_to_remove->GetMesh());
		}
	}
}

void Octree::RefactorOctree()
{
	// Recalculate New Dimensions
	min_point = max_point = float3::zero;
	for (std::list<GameObject*>::iterator it = App->scene_intro->octree_objects.begin(); it != App->scene_intro->octree_objects.end(); it++)
	{
		Recalculate((*it)->GetBB().minPoint, (*it)->GetBB().maxPoint);
	}
	// Create Octree with that dimensions
	Create(min_point, max_point);;
	// Insert Elements
	for (std::list<GameObject*>::iterator it = App->scene_intro->octree_objects.begin(); it != App->scene_intro->octree_objects.end(); it++)
	{
		Insert(*it);
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

void Octree::Divide()
{
	if (!root_item->HasChilds())
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
	static float3 points[OCTREECHILDS];//is a box
	item_box.GetCornerPoints(points);
	BoxDD(points, Blue);

	if (HasChilds())
	{
		for (int i = 0; i < OCTREECHILDS; i++)
		{
			childs[i]->Draw();
		}
		
	}

}

bool OctreeItem::HasChilds() const
{
	return childs[0] != nullptr;
}
