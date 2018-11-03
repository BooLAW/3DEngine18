
#ifndef __OCTREE_H__
#define __OCTREE_H__

#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"

#define OCTREECHILDS 8

class GameObject;
class ComponentMesh;
class Mesh;
class OctreeItem {
public:
	OctreeItem(AABB& box);
	~OctreeItem();
	bool HasChilds()const;
	void SetChildsNull();
	void ClearItems();
	bool IsItemFull()const;
	void InsertItem(Mesh* mesh_to_insert);
	void RemoveItem(Mesh* mesh_to_remove);
	void SubdivideItem();

	void CollectIntersections(std::list<Mesh*> intersections, AABB* bounding_box);
	
	void Draw();
public:
	AABB item_box;
	std::vector<Mesh*> item_elements;
	OctreeItem* childs[OCTREECHILDS];
	bool last;
};

class Octree
{
public:
	Octree();
	virtual ~Octree();
	void Create(float3 min, float3 max);
	void Clear();
	void Insert(GameObject* go_to_insert);
	void Insert(Mesh* mesh);
	bool CheckNoResizeToInsert(Mesh* mesh);
	void Remove(GameObject* go_to_insert);

	void RefactorOctree();
	void CollectIntersections(std::list<Mesh*> intersections, AABB* bounding_box);
	void Divide();

	void Recalculate(float3 min, float3 max);
	void ExpandBox(float3 min, float3 max);

	void DrawOctree(bool active);

private:
	OctreeItem * root_item = nullptr;
public:
	bool update_octree;
	float3 min_point, max_point;
	int num_elements = 0;
};

#endif

