#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "PhysBody.h"
#include "ModuleCamera3D.h"
#include "Camera.h"

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

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
}

ModulePhysics3D::~ModulePhysics3D()
{

}

bool ModulePhysics3D::Init()
{
	CONSOLE_LOG_INFO("Creating 3D Physics simulation");
	bool ret = true;
	//InitializeWorld();
	
	return ret;
}

bool ModulePhysics3D::Start()
{
	CONSOLE_LOG_INFO("Creating Physics environment");
	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);

	world->setGravity(GRAVITY);

	////Big plane
	CreatePlane();

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
	world->stepSimulation(dt, 15);

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		if (numContacts > 0)
		{
			PhysBody* pbodyA = (PhysBody*)obA->getUserPointer();
			PhysBody* pbodyB = (PhysBody*)obB->getUserPointer();

			if (pbodyA && pbodyB)
			{				
				for (std::vector<Module*>::iterator item = pbodyA->collision_listeners.begin(); item != pbodyA->collision_listeners.begin(); item++)
				{
					(*item)->OnCollision(pbodyA, pbodyB);
				}

				for (std::vector<Module*>::iterator item = pbodyB->collision_listeners.begin(); item != pbodyB->collision_listeners.begin(); item++)
				{
					(*item)->OnCollision(pbodyB, pbodyA);
				}
			}
		}
	}
	return UPDATE_CONTINUE;
}

update_status ModulePhysics3D::Update(float dt)
{
	if (bullet_test == true)
	{
		BulletTest();
		bullet_test = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		PCube c(1.0f, 1.0f, 1.0f);
		c.SetPos(App->camera->editor_cam->frustum.pos.x, App->camera->editor_cam->frustum.pos.y, App->camera->editor_cam->frustum.pos.z);
		float force = 30.0f;		
	}
	return UPDATE_CONTINUE;
}

update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModulePhysics3D::UpdatePhysics()
{
	int i = 0;
	for (std::vector<PhysBody*>::iterator item = bodies.begin(); item != bodies.end(); item++)
	{
		(*item)->GetRigidBody()->applyForce({ 0,6,0 }, { 0,1,0 });
	}
	for (std::vector<PhysBody*>::iterator item = bodies.begin(); item != bodies.end(); item++)
	{
		float matrix[16];
		(*item)->GetTransform(matrix);

		if (cube_list.size() > i)
		{
			cube_list.at(i)->transform.Set(matrix);
		}
		i++;
	}

	for (int i = 0; i < cube_list.size(); i++)
	{		
		cube_list[i]->Render();
	}
}

bool ModulePhysics3D::CleanUp()
{
	CONSOLE_LOG_INFO("Destroying 3D Physics simulation");
	// Remove from the world all collision bodies
	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	for (std::vector<btTypedConstraint*>::iterator item = constraints.begin(); item != constraints.end(); item++)
	{
		world->removeConstraint((*item));
		delete (*item);
	}

	constraints.clear();

	for (std::vector<btDefaultMotionState*>::iterator item = motions.begin(); item != motions.end(); item++)
		delete (*item);

	motions.clear();

	for (std::vector<btCollisionShape*>::iterator item = shapes.begin(); item != shapes.end(); item++)
		delete (*item);

	shapes.clear();

	for (std::vector<PhysBody*>::iterator item = bodies.begin(); item != bodies.end(); item++)
		delete (*item);

	bodies.clear();

	delete world;

	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;

	return true;
}

void ModulePhysics3D::CreateSphere(float3 position, int radius)
{
	PSphere new_sphere;
	new_sphere.pos = position;
	new_sphere.radius = (float)radius;
	//spheres_list.push_back(new_sphere);
}

void ModulePhysics3D::CreateCube(float3 minPoint, float3 maxPoint)
{
	AABB new_cube;
	new_cube.minPoint = minPoint;
	new_cube.maxPoint = maxPoint;
	//pcube_list.push_back(new_cube);
}

std::list<float2> ModulePhysics3D::GetCubeCollisions()
{
	int listener = 0;
	int candidate = 0;

	std::list<float2> collisions_list;

	for (listener; listener < pcube_list.size(); listener++)
	{
		bool collided = false;
		for (candidate; candidate < pcube_list.size(); candidate++)
		{
			if (listener == candidate)
				continue;
			collided = pcube_list[listener].Intersects(pcube_list[candidate]);
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

PhysBody* ModulePhysics3D::AddBody(PCube& cube, float mass)
{
	PCube aux = cube;
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.dimensions.x*0.5f, cube.dimensions.y*0.5f, cube.dimensions.z*0.5f));
	shapes.push_back(colShape);
	

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(cube.transform.ptr());

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody* pbody = new PhysBody(body);

	world->addRigidBody(body);
	bodies.push_back(pbody);

	cube_list.push_back(&cube);

	return pbody;
}

PhysBody * ModulePhysics3D::AddBody(const PSphere& sphere, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.push_back(colShape);

	btTransform startTransform;
	btScalar* btsphere = new btScalar();
	startTransform.setFromOpenGLMatrix(sphere.transform.ptr());

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody* pbody = nullptr;

	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}

void ModulePhysics3D::CreatePlane()
{
	btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

	btDefaultMotionState* myMotionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);
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
		world->addRigidBody(body);
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

		world->addRigidBody(body);
	}

	/// Do some simulation

	///-----stepsimulation_start-----
	for (int i = 0; i < 20; i++)
	{
		world->stepSimulation(1.f / 60.f, 10);

		//print positions of all objects
		for (int j = world->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = world->getCollisionObjectArray()[j];
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
	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		world->removeCollisionObject(obj);
		delete obj;
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
}



