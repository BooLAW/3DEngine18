#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "Bullet/include//btBulletDynamicsCommon.h"

#ifdef _DEBUG
#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif



ModulePhysics3D::ModulePhysics3D(bool start_enabled) : Module(start_enabled)
{
	debug = true;

}

ModulePhysics3D::~ModulePhysics3D()
{

}

bool ModulePhysics3D::Init()
{
	CONSOLE_LOG_INFO("Creating 3D Physics simulation");
	bool ret = true;
	InitializeWorld();
	return ret;
}
void InitializeWorld()
{

}
bool ModulePhysics3D::Start()
{
	CONSOLE_LOG_INFO("Creating Physics environment");
	return true;
}

void ModulePhysics3D::DrawModuleConfig()
{
}

update_status ModulePhysics3D::PreUpdate(float dt)
{	
	return UPDATE_CONTINUE;
}

update_status ModulePhysics3D::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModulePhysics3D::CleanUp()
{
	CONSOLE_LOG_INFO("Destroying 3D Physics simulation");

	return true;
}

void ModulePhysics3D::CreateSphere(float3 position, int radius)
{
	Sphere new_sphere;
	new_sphere.pos = position;
	new_sphere.r = radius;
	spheres_list.push_back(new_sphere);
}

void ModulePhysics3D::CreateCube(float3 minPoint, float3 maxPoint)
{
	AABB new_cube;
	new_cube.minPoint = minPoint;
	new_cube.maxPoint = maxPoint;
	cube_list.push_back(new_cube);
}

std::list<float2> ModulePhysics3D::GetSphereCollisions()
{
	int listener = 0;
	int candidate = 0;
	
	std::list<float2> collisions_list;

	for (listener; listener < spheres_list.size(); listener++)
	{
		bool collided = false;
		for (candidate; candidate < spheres_list.size(); candidate++)
		{
			if (listener == candidate)
				continue;
			collided = spheres_list[listener].Intersects(spheres_list[candidate]);
			if (collided)
			{
				collisions_list.push_back({(float) listener,(float)candidate });
				CONSOLE_LOG_INFO("Sphere %d collides with Sphere %d",listener, candidate);
			}
		}
		candidate = 0;
	}
	return collisions_list;
}

std::list<float2> ModulePhysics3D::GetCubeCollisions()
{
	int listener = 0;
	int candidate = 0;

	std::list<float2> collisions_list;

	for (listener; listener < cube_list.size(); listener++)
	{
		bool collided = false;
		for (candidate; candidate < cube_list.size(); candidate++)
		{
			if (listener == candidate)
				continue;
			collided = cube_list[listener].Intersects(cube_list[candidate]);
			if (collided)
			{
				collisions_list.push_back({ (float)listener,(float)candidate });
				CONSOLE_LOG_INFO("Cube %d collides with Cube %d",listener, candidate);
			}
		}
		candidate = 0;
	}
	return collisions_list;
}

void ModulePhysics3D::InitializeWorld()
{
	//create Config
	coll_config = new btDefaultCollisionConfiguration();
	//create Dispatcher
	 coll_dispatcher = new btCollisionDispatcher(coll_config);
	//create broadphase
	broad_phase = new btDbvtBroadphase();

	constraint_solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(coll_dispatcher, broad_phase, constraint_solver, coll_config);

	//set Real Gravity for now
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

