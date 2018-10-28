#ifndef __MODULE_PHYSICS_3D__
#define __MODULE_PHYSICS_3D__


#include "Module.h"
#include "Globals.h"

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D( bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	void DrawModuleConfig()override;
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void CreateSphere(float3 position, int radius);
	void CreateCube(float3 minPoint, float3 maxPoint);
	std::list<float2> GetSphereCollisions();
	std::list<float2> GetCubeCollisions();
	
	private:

	bool debug;

	
	std::vector<AABB> cube_list;
	std::vector<Sphere> spheres_list;

};

#endif

