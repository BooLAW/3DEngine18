#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"


#ifdef _DEBUG
#pragma comment (lib, "NvidiaPhysX/Libs/PhysX3DEBUG_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PhysX3CommonDEBUG_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PxFoundationDEBUG_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PhysX3ExtensionsDEBUG_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PhysX3CookingDEBUG_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PxPvdSDKDEBUG_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PxTaskDEBUG_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PsFastXmlDEBUG_x86.lib")
#else
#pragma comment (lib, "NvidiaPhysX/Libs/PhysX3_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PhysX3Common_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PxFoundation_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PhysX3Extensions_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PhysX3Cooking_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PxPvdSDK_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PxTask_x86.lib")
#pragma comment (lib, "NvidiaPhysX/Libs/PsFastXml_x86.lib")
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
	return ret;
}

bool ModulePhysics3D::Start()
{
	CONSOLE_LOG_INFO("Creating Physics environment");
	return true;
}

void ModulePhysics3D::DrawModuleConfig()
{
	if (ImGui::CollapsingHeader("Physics"))
	{
	
	}
}

update_status ModulePhysics3D::PreUpdate(float dt)
{	
	return UPDATE_CONTINUE;
}

update_status ModulePhysics3D::Update(float dt)
{
	if (App->state == playing)
	{
		//now we will use the main scene only but we can check all the scenes and
		myScene->simulate(dt);
		myScene->fetchResults(true);
		//CHECK COLLISIONS-------------------------
		//http://gameworksdocs.nvidia.com/PhysX/3.4/PhysXGuide/Manual/Threading.html
	}

	return UPDATE_CONTINUE;
}

update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModulePhysics3D::CleanUp()
{
	CONSOLE_LOG_INFO("Destroying 3D Physics simulation");


	//myPhysics->release();

	//Do not forget to release the foundation object as well, but only after all other PhysX modules have been released:
	//myFoundation->release();
	return true;
}

void ModulePhysics3D::CreateSphere(float3 position, int radius)
{
	PSphere new_sphere;
	new_sphere.pos = position;
	new_sphere.radius = (float)radius;
	spheres_list.push_back(new_sphere);
}

void ModulePhysics3D::CreateCube(float3 minPoint, float3 maxPoint)
{
	AABB new_cube;
	new_cube.minPoint = minPoint;
	new_cube.maxPoint = maxPoint;
	cube_list.push_back(new_cube);
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

void ModulePhysics3D::InitMainScene()
{
	physx::PxSceneDesc sceneDesc(myPhysics->getTolerancesScale());

	//create CudaManager
	physx::PxCudaContextManagerDesc cudaCMD;
	myCudaManager = PxCreateCudaContextManager(*myFoundation, cudaCMD);
	sceneDesc.gpuDispatcher = myCudaManager->getGpuDispatcher();


}

bool ModulePhysics3D::HasValidScenes()
{
	return (myPhysics!=nullptr && myPhysics->getNbScenes() > 0);
}

uint ModulePhysics3D::GetNumScenes()const
{
	return myPhysics->getNbScenes();
}

