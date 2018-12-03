#ifndef __MODULE_PHYSICS_3D__
#define __MODULE_PHYSICS_3D__


#include "Module.h"
#include "Globals.h"
#include "Primitive.h"
#include "NvidiaPhysX/Include/PxPhysicsAPI.h"
#include <map>

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D(bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	void DrawModuleConfig()override;

	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();
	//Prev Assignment3
	void CreateSphere(float3 position, int radius);
	void CreateCube(float3 minPoint, float3 maxPoint);
	//std::list<float2> GetSphereCollisions();
	std::list<float2> GetCubeCollisions();
	//-------------------------

	//Assignment 3--------------
	void NvidiaBuild();
	void NvidiaShutDown();
	void InitMainScene();
	void NvidiaValuesToNull();
	//-----------------------
private:
	bool debug;


	std::vector<AABB> cube_list;
	std::vector<PSphere> spheres_list;

	//Nvidia
	physx::PxScene* myScene;
	physx::PxFoundation* myFoundation;
	physx::PxPvd* myPvd;
	physx::PxCooking* myCooking;
	physx::PxPhysics* myPhysics;
};
#endif

