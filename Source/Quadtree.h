#pragma once
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#define QUADTREECHILDS 8

class GameObject;
class ComponentMesh;
class Mesh;
class QuadtreeItem {
public:
	QuadtreeItem(AABB& box);
	~QuadtreeItem();
	void SetChildsNull();
	void ClearItems();
	bool IsItemFull()const;
	void InsertItem(Mesh* mesh_to_insert);
	void SubdivideItem();
	void CollectIntersections(std::list<Mesh*> intersections, AABB* bounding_box);

public:
	AABB item_box;
	std::list<Mesh*> item_elements;
	QuadtreeItem* childs[QUADTREECHILDS];
	int lvl = 0;
};

class Quadtree
{
public:
	Quadtree();
	virtual ~Quadtree();
	void Create(float3 min, float3 max);
	void Clear();
	void Insert(GameObject* go_to_insert);
private:
	QuadtreeItem * root_item = nullptr;
public:
	bool update_quadtree;
	float3 min_point, max_point;
};

