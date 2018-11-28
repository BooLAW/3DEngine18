#ifndef __MODULE_PHYSICS_3D__
#define __MODULE_PHYSICS_3D__
//LINKS ----------------------------ç
//QuickstartGuide: https://github.com/bulletphysics/bullet3/blob/master/docs/BulletQuickstart.pdf
//Documentation: https://pybullet.org/wordpress/index.php/forum-2/
//How Unity Does it: https://unity3d.com/learn/tutorials/s/physics
	//RigidBody: https://docs.unity3d.com/Manual/RigidbodiesOverview.html
	//Character Controller: 
		//https://docs.unity3d.com/Manual/CharacterControllers.html
		// https://docs.unity3d.com/Manual/class-CharacterController.html
//----------------------

#include "Module.h"
#include "Globals.h"
#include "Primitive.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;
class btDefaultMotionState;
class btTypedConstraint;

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
	//Prev Assignment3
	void CreateSphere(float3 position, int radius);
	void CreateCube(float3 minPoint, float3 maxPoint);
	//std::list<float2> GetSphereCollisions();
	std::list<float2> GetCubeCollisions();
	//-------------------------
	//Assignment 3--------------
	void InitializeWorld();
	void CreatePlane();
	void ShootSphere();
	void BulletTest();
	bool bullet_test = false;
	//-----------------------
private:
	bool debug;
	
	
	std::vector<AABB> cube_list;
	std::vector<PSphere> spheres_list;

	//physics
	btDefaultCollisionConfiguration*	coll_config;
	btCollisionDispatcher*				coll_dispatcher;
	btBroadphaseInterface*				broad_phase;
	btSequentialImpulseConstraintSolver* constraint_solver;
	btDiscreteDynamicsWorld*			dynamicsWorld;
	

	std::list<btCollisionShape*> shapes;
	std::list<PhysBody3D*> bodies;
	std::list<btDefaultMotionState*> motions;
	std::list<btTypedConstraint*> constraints;
};
#endif

