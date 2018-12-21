#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "ComponentTransform.h"
#include "ComponentColliderCube.h"
#include "ComponentColliderSphere.h"
#include "Component.h"
#include "PhysBody.h"
#include "ModuleCamera3D.h"
#include "Camera.h"
#include "PanelGame.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "MathGeoLib/MathGeoLib.h"

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

}

ModulePhysics3D::~ModulePhysics3D()
{

}

bool ModulePhysics3D::Init()
{
	CONSOLE_LOG_INFO("Creating 3D Physics simulation");
	bool ret = true;
	//InitializeWorld();
	pdebug = true;

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	pdebug_draw = new PDebugDrawer();

	gravity = -10;
	return ret;
}

bool ModulePhysics3D::Start()
{
	CONSOLE_LOG_INFO("Creating Physics environment");


	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);

	pdebug_draw->setDebugMode(pdebug_draw->DBG_DrawWireframe);
	world->setDebugDrawer(pdebug_draw);

	world->setGravity(GRAVITY);

	//Big plane
	CreatePlane();



	return true;
}

void ModulePhysics3D::DrawModuleConfig()
{
	if (ImGui::CollapsingHeader("Physics"))
	{
		if (ImGui::InputFloat("Y Gravity",&gravity))
		{
			App->physics->SetGravity(gravity);
		}
		ImGui::Checkbox("Debug", &pdebug);
	}
}

update_status ModulePhysics3D::PreUpdate(float dt)
{


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
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		pdebug = !pdebug;

	if (bullet_test == true)
	{
		BulletTest();
		bullet_test = false;
	}
	if (pdebug)
	{
		world->debugDrawWorld();
	}


	return UPDATE_CONTINUE;
}

update_status ModulePhysics3D::PostUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

void ModulePhysics3D::UpdatePhysics()
{
	static float step = 0;
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_UP)
	{
		step = 1;
	}
	world->stepSimulation(App->dt, step);
	std::vector<float*> matrix_list;
	float *matrix = new float[16];
	int i = 0;
	for (std::vector<PhysBody*>::iterator item = bodies.begin(); item != bodies.end(); item++)
	{
		if ((*item)->has_primitive_render == true)
		{
			(*item)->GetTransform(matrix);
			if ((*item)->primitive_ptr != nullptr)
			{
				(*item)->primitive_ptr->transform.Set(matrix);
			}						
			i++;
		}	
		if (App->state == stopped)
		{
			(*item)->initial_pos = nullptr;
		}
	}

	for (int i = 0; i < primitive_list.size(); i++)
	{
		primitive_list[i]->Render();
	}

	if (App->state == stopped)
	{
		updateoncecollider = false;
	}

	if (App->state == playing)
	{

		for (std::vector<PhysBody*>::iterator item = bodies.begin(); item != bodies.end(); item++)
		{
			if ((*item)->dead == false)
			{
				if ((*item)->owner != nullptr)
				{
					//Get matrix from bullet physics
					float4x4 final_matrix4x4;
					(*item)->GetTransform(matrix);

					//Set Rotations
					final_matrix4x4[0][0] = matrix[0];	final_matrix4x4[0][1] = matrix[1];	final_matrix4x4[0][2] = matrix[2];
					final_matrix4x4[1][0] = matrix[4];	final_matrix4x4[1][1] = matrix[5];	final_matrix4x4[1][2] = matrix[6];
					final_matrix4x4[2][0] = matrix[8];	final_matrix4x4[2][1] = matrix[9];	final_matrix4x4[2][2] = matrix[10];
					final_matrix4x4.Transpose();

					float3 pos = float3(0, 0, 0);
					if ((*item)->initial_pos == nullptr)
					{
						(*item)->initial_pos = new float3( matrix[12], matrix[13], matrix[14] );
					}
					else
					{
						float3 local_pos = { matrix[12], matrix[13], matrix[14] };
						pos = local_pos - *(*item)->initial_pos;
					}
					
					//Matrix Translation and size



					float final_pos[3];
					float* user_offset = new float[3];

					if ((*item)->owner->HasColliderCube())
					{
						if (updateoncecollider == false)
						{
							(*item)->owner->GetColliderCube()->Update();
							updateoncecollider = true;
						}
						user_offset = (*item)->owner->GetColliderCube()->center_offset;		
						final_pos[0] = pos.x ;//- user_offset[0];
						final_pos[1] = pos.y ;//- user_offset[1];
						final_pos[2] = pos.z ;//- user_offset[2];
					}
					else
					{
						final_pos[0] = pos.x;
						final_pos[1] = pos.y;
						final_pos[2] = pos.z;
					}

										
					//Transform + size
					final_matrix4x4[0][3] = final_pos[0];
					final_matrix4x4[1][3] = final_pos[1];
					final_matrix4x4[2][3] = final_pos[2];
					final_matrix4x4[3][0] = 1;				final_matrix4x4[3][1] = 1;	final_matrix4x4[3][2] = 1;	final_matrix4x4[3][3] = matrix[15];			
					

					(*item)->owner->comp_transform->SetLocalMatrix(final_matrix4x4);

					(*item)->owner->comp_transform->UpdateTransformValues();
					(*item)->owner->comp_transform->updated_outside = false;
				}
			}

		}

	}

	if (matrix_list.size() > 0)
	{
		int i2 = 0;
		for (std::vector<Primitive*>::iterator item2 = primitive_list.begin(); item2 != primitive_list.end(); item2++)
		{
			(*item2)->transform.Set(matrix_list[i2]);
			i2++;
		}
	}




	//Debug Physics World
	if (pdebug)
	{
		world->debugDrawWorld();
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

	delete pdebug_draw;
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;

	return true;
}

void ModulePhysics3D::CreateSphere(float3 position, int radius)
{
	PSphere new_sphere;
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

void ModulePhysics3D::LoadPhysBodies()
{
	for (std::vector<GameObject*>::iterator item = App->scene_intro->go_list.begin(); item != App->scene_intro->go_list.end(); item++)
	{
		if ((*item)->HasRigidBody())
		{
			SwitchPhysBody((*item)->physbody);			
		}
	}
}

void ModulePhysics3D::SwitchPhysBody(PhysBody * body_to_switch)
{
	int shape_type = body_to_switch->GetRigidBody()->getCollisionShape()->getShapeType();

	switch (shape_type)
	{
		case 0://Cube
		{			
			//Store Primitive
			PCube* cube = new PCube();
			cube = (PCube*)body_to_switch->primitive_ptr;							

			//Store Position of GO and set it to primitive
			float* transform_matrix = new float[16];
			transform_matrix = body_to_switch->owner->comp_transform->trans_matrix_g.ptr();
			cube->SetPos(transform_matrix[3], transform_matrix[7], transform_matrix[11]);
			
			//Remove Rigid Body
			world->removeRigidBody(body_to_switch->GetRigidBody());		

			//Create New Rigid Body and link it to GO
			body_to_switch->owner->physbody = AddBody(*cube, cube->mass);
			body_to_switch->owner->physbody->owner = body_to_switch->owner;

			break;
		}
		case 8://Sphere
		{
			PSphere* sphere = (PSphere*)body_to_switch->primitive_ptr;
			body_to_switch->dead = true;
			world->removeRigidBody(body_to_switch->GetRigidBody());

			float* transform_matrix = new float[16];
			transform_matrix = body_to_switch->owner->comp_transform->trans_matrix_g.ptr();
			sphere->SetPos(transform_matrix[3], transform_matrix[7], transform_matrix[11]);

			body_to_switch->owner->physbody = AddBody(*sphere, sphere->mass);
			body_to_switch->owner->physbody->owner = body_to_switch->owner;
			break;
		}
	}


}

btDiscreteDynamicsWorld * ModulePhysics3D::GetWorld()const
{
	return world;
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

	pbody->use_gravity = true;
	pbody->dimensions = cube.dimensions;
	pbody->primitive_ptr = &cube;

	world->addRigidBody(body);
	bodies.push_back(pbody);

	if (cube.has_primitive_render)
	{
		primitive_list.push_back((Primitive*)&cube);
	}
	

	return pbody;
}

PhysBody * ModulePhysics3D::AddBody(PSphere& sphere, float mass, bool isCollider, bool addForce, float force)
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
	PhysBody* pbody = new PhysBody(body);

	if (addForce)
	{
		//Grab direction of the camera
		btVector3 editor_cam_dir;
		editor_cam_dir.setX(App->camera->current_game_camera->GetCamera()->frustum.front.x);
		editor_cam_dir.setY(App->camera->current_game_camera->GetCamera()->frustum.front.y);
		editor_cam_dir.setZ(App->camera->current_game_camera->GetCamera()->frustum.front.z);

		//Adding 40 Newtons of force
		editor_cam_dir = editor_cam_dir * force;
		pbody->GetRigidBody()->applyCentralImpulse(editor_cam_dir);

		//The spawn position is changed on the Start() method of this module
	}

	if (isCollider)
	{
		pbody->has_primitive_render = false;
	}
	else
	{
		pbody->has_primitive_render = true;
		primitive_list.push_back((Primitive*)&sphere);
		pbody->primitive_ptr = &sphere;
	}


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

void ModulePhysics3D::ShootSphere(float force,float radius)
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && App->imgui->game->MouseOver())
	{
		PSphere* test = new PSphere();
		test->radius = radius;
		float position[3];
		position[0] = App->camera->current_game_camera->GetCamera()->frustum.pos.x;
		position[1] = App->camera->current_game_camera->GetCamera()->frustum.pos.y;
		position[2] = App->camera->current_game_camera->GetCamera()->frustum.pos.z;

		test->SetPos(position[0], position[1], position[2]);
		App->physics->AddBody(*test, 1, false, true);
	}
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

void ModulePhysics3D::SetGravity(float new_gravity)
{
	//Gravity will always be setted on the Y axis
	world->setGravity({ 0.0f,new_gravity,0.0f});

}

void ModulePhysics3D::ToggleDebugDraw()
{
	pdebug = !pdebug;
}

float ModulePhysics3D::GetGravity() const
{
	return gravity;
}

void ModulePhysics3D::AddConstraintP2P(PhysBody& bodyA, PhysBody& bodyB, const float3& anchorA, const float3& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z));
	world->addConstraint(p2p);
	constraints.push_back(p2p);
	p2p->setDbgDrawSize(2.0f);
}
