#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"


ModulePhysics3D::ModulePhysics3D(bool start_enabled) : Module(start_enabled)
{
	debug = true;

}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{

}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	CONSOLE_LOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	CONSOLE_LOG("Creating Physics environment");



	

	return true;
}

void ModulePhysics3D::DrawModuleConfig()
{
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	CONSOLE_LOG("Destroying 3D Physics simulation");

	

	return true;
}

void ModulePhysics3D::CreateSphere(vec position, int radius)
{
	Sphere new_sphere;
	new_sphere.pos = position;
	new_sphere.r = radius;
	spheres_list.push_back(new_sphere);
}

void ModulePhysics3D::CreateCube(vec minPoint, vec maxPoint)
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
				CONSOLE_LOG("Sphere %d collides with Sphere %d", listener, candidate);
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
				CONSOLE_LOG("Cube %d collides with Cube %d", listener, candidate);
			}
		}
		candidate = 0;
	}
	return collisions_list;
}

