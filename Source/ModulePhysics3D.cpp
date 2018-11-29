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
	//CreatePlane();
	return ret;
}

bool ModulePhysics3D::Start()
{
	CONSOLE_LOG_INFO("Creating Physics environment");
	sphere_test.radius = 4;
	sphere_test.pos = float3(0, 10, 0);
	sphere_test.dead = false;
	return true;
}

void ModulePhysics3D::DrawModuleConfig()
{
	if (ImGui::CollapsingHeader("Physics"))
	{
		if (ImGui::Button("Test"))
			bullet_test = true;
	}
}

update_status ModulePhysics3D::PreUpdate(float dt)
{	
	return UPDATE_CONTINUE;
}

update_status ModulePhysics3D::Update(float dt)
{
	if (bullet_test == true)
	{
		BulletTest();
		bullet_test = false;
	}
	for (int i = 0; spheres_list.size() > i; i++)
	{
		spheres_list[i];
	}
	sphere_test.Render();
	return UPDATE_CONTINUE;
}

update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModulePhysics3D::CleanUp()
{
	CONSOLE_LOG_INFO("Destroying 3D Physics simulation");
	//delete collision shapes
		/*for (int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}*/

	//delete dynamics world
	delete dynamicsWorld;
	//delete solver
	delete constraint_solver;
	//delete broadphase
	delete broad_phase;
	//delete dispatcher
	delete coll_dispatcher;

	delete coll_config;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	//collisionShapes.clear();
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

//std::list<float2> ModulePhysics3D::GetSphereCollisions()
//{
//	//int listener = 0;
//	//int candidate = 0;
//	//
//	//std::list<float2> collisions_list;
//
//	//for (listener; listener < spheres_list.size(); listener++)
//	//{
//	//	bool collided = false;
//	//	for (candidate; candidate < spheres_list.size(); candidate++)
//	//	{
//	//		if (listener == candidate)
//	//			continue;
//	//		collided = spheres_list[listener].Intersects(spheres_list[candidate]);
//	//		if (collided)
//	//		{
//	//			collisions_list.push_back({(float) listener,(float)candidate });
//	//			CONSOLE_LOG_INFO("Sphere %d collides with Sphere %d",listener, candidate);
//	//		}
//	//	}
//	//	candidate = 0;
//	//}
//
//
//	//return new list<float2>;
//}

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

PhysBody * ModulePhysics3D::AddBody(const PSphere& sphere, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.push_back(colShape);

	btTransform startTransform;
	btScalar* btsphere = new btScalar();
	sphere.transform;
	//startTransform.setFromOpenGLMatrix(&sphere.transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody* pbody = nullptr;

	dynamicsWorld->addRigidBody(body);
	//bodies.push_back(pbody);

	return pbody;
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
	dynamicsWorld->setGravity(btVector3(0, -10000, 0));
}

void ModulePhysics3D::CreatePlane()
{
	btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

		btDefaultMotionState* myMotionState = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

		btRigidBody* body = new btRigidBody(rbInfo);
		dynamicsWorld->addRigidBody(body);
}

void ModulePhysics3D::ShootSphere()
{
}

void ModulePhysics3D::BulletTest()
{
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	///create a few basic rigid bodies

	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -56, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}

	{
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();


		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, -50000, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(0, 200, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}

	/// Do some simulation

	///-----stepsimulation_start-----
	for (int i = 0; i < 20; i++)
	{
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

		//print positions of all objects
		for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = obj->getWorldTransform();
			}
			CONSOLE_LOG_INFO("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			if (j != 0 && trans.getOrigin().getY() < -5)//where the box is
				CONSOLE_LOG_INFO("Box Hitted by body %d \n", j);
		}
	}

	///-----stepsimulation_end-----

	//cleanup in the reverse order of creation/initialization

	///-----cleanup_start-----

	//remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}
}


