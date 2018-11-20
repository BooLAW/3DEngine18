#ifndef __MODULE_PHYSICS_3D__
#define __MODULE_PHYSICS_3D__
//LINKS ----------------------------ç
//QuickstartGuide: https://docs.google.com/document/d/10sXEhzFRSnvFcl3XxNGhnD4N2SedqwdAvK3dsihxVUA/edit#heading=h.2ye70wns7io3
//Documentation: https://pybullet.org/wordpress/index.php/forum-2/
//How Unity Does it: https://unity3d.com/learn/tutorials/s/physics
	//RigidBody: https://docs.unity3d.com/Manual/RigidbodiesOverview.html
	//Character Controller: 
		//https://docs.unity3d.com/Manual/CharacterControllers.html
		// https://docs.unity3d.com/Manual/class-CharacterController.html
//----------------------

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

