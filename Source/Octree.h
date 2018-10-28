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
	void SetChildsNull();
	void ClearItems();
	bool IsItemFull()const;
	void InsertItem(Mesh* mesh_to_insert);
	void SubdivideItem();
	void CollectIntersections(std::list<Mesh*> intersections, AABB* bounding_box);
	void Draw();
	bool HasChilds()const;
public:
	AABB item_box;
	std::list<Mesh*> item_elements;
	OctreeItem* childs[OCTREECHILDS];
	int lvl = 0;
};

class Octree
{
public:
	Octree();
	virtual ~Octree();
	void Create(float3 min, float3 max);
	void Clear();
	void DrawOctree(bool active);
	void Insert(GameObject* go_to_insert);
	void Insert(Mesh* mesh);
	void CollectIntersections(std::list<Mesh*> intersections, AABB* bounding_box);
	void Recalculate(float3 min, float3 max);
	void ExpandBox(float3 min, float3 max);
private:
	OctreeItem * root_item = nullptr;
public:
	bool update_quadtree;
	float3 min_point, max_point;
};

#endif
