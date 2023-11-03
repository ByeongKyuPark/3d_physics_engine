#include "simulator.h"
#include "math/mathConstants.h"
#include "graphics/textureImage.h"
#include "spawner.h"
#include <typeinfo>
#include <cmath>

Simulator::Simulator()
	: isRunning{ false }, shouldRenderContactInfo{ false }, timeStepMultiplier{ 1.f },
	cameraManager{}, physicsWorld {}, renderer{cameraManager,"rigid body simulator"}, 
	userInterface(renderer.GetWindow(), renderer.GetTextureBufferID())
{}

void Simulator::Run()
{
	double prevTime = glfwGetTime();
	double curTime, deltaTime;
	double accumulator{};

	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		curTime = glfwGetTime();
		deltaTime = curTime - prevTime;
		accumulator += deltaTime;
		prevTime = curTime;

		HandleKeyboardInput();

		//accumulator
		//	1. ensures consistent updates(fixed timestep)
		//	2. adaptive (can update multiple times in one frame if needed to catch up)
		while (accumulator >= Target_FPS_Inverse) {
			if (isRunning) {
				physicsWorld.Simulate(Target_FPS_Inverse * timeStepMultiplier); // consistent & fixed timestep for physics
			}
			accumulator -= Target_FPS_Inverse;
		}

		renderer.BindSceneFrameBuffer();
		renderer.Clear();

		//background
		renderer.RenderGround();

		//objects
		const std::vector<RigidObject*>& objects = physicsWorld.GetObjects();
		for (auto& object : objects) {
			renderer.RenderObject(object);
		}


		//contact points
		auto collisionManifolds = physicsWorld.GetManifold();
		for (const auto& manifold : collisionManifolds)
		{
			if (shouldRenderContactInfo) {
				if (manifold.contactPoint.p1.first == true) {//valid
					renderer.RenderCollisionContacts(manifold.contactPoint.p1.second, manifold.collisionNormal);
				}
				if (manifold.contactPoint.p2.first == true) {//valid
					renderer.RenderCollisionContacts(manifold.contactPoint.p2.second, manifold.collisionNormal);
				}
			}
		}

		//events
		while (eventQueue.empty() == false)
		{
			eventQueue.front().get()->Handle(*this);
			eventQueue.pop();
		}

		renderer.BindDefaultFrameBuffer();

		userInterface.renderAll(renderer.GetTextures(), eventQueue, objects, isRunning, selectedObjects);

		glfwSwapBuffers(renderer.GetWindow());
		glfwPollEvents();

	}
}

SphereObject* Simulator::AddSphere(math::Vector3 pos, TextureID textureID) {
	SphereObject* newObject = new SphereObject;
	physicsWorld.AddRigidBody(pos.x, pos.y, pos.z, newObject);
	physicsWorld.AddCollider(newObject->rigidBody, newObject);

	renderer.AddGraphicalShape(newObject);

	newObject->GetShape()->SetTextureID(textureID);
	physicsWorld.AddPhysicalObject(newObject);
	return newObject;
}

BoxObject* Simulator::AddBox(math::Vector3 pos, TextureID textureID) {
	BoxObject* newObject = new BoxObject;
	physicsWorld.AddRigidBody(pos.x, pos.y, pos.z, newObject);
	physicsWorld.AddCollider(newObject->GetRigidBody(), newObject);

	renderer.AddGraphicalShape(newObject);
	newObject->GetShape()->SetTextureID(textureID);

	physicsWorld.AddPhysicalObject(newObject);
	return newObject;
}

SphereBoxSpawner* Simulator::AddSpawner(math::Vector3 pos, TextureID textureID) {
	SphereBoxSpawner* newObject = new SphereBoxSpawner(physicsWorld, renderer);
	physicsWorld.AddRigidBody(pos.x, pos.y, pos.z, newObject);
	physicsWorld.AddCollider(newObject->GetRigidBody(), newObject);
	renderer.AddGraphicalShape(newObject);
	newObject->GetShape()->SetTextureID(textureID);

	physicsWorld.AddPhysicalObject(newObject);
	return newObject;
}

std::vector<RigidObject*>::iterator Simulator::RemoveObject(RigidObject* obj)
{
	physicsWorld.RemovePhysicsObject(obj);
	renderer.RemoveShape(obj);

	std::vector<RigidObject*>& objects = physicsWorld.GetObjects();
	std::vector<RigidObject*>::iterator itr = objects.begin();
	for (; itr != objects.end();) {
		if (*itr == obj) {
			delete* itr;
			break;
		}
		else {
			itr++;//https://backhugger.tistory.com/45
		}
	}
	return objects.erase(itr);
}

void Simulator::HandleKeyboardInput()
{
	if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(renderer.GetWindow(), GLFW_TRUE);
	}

	//move
	glm::vec3 offset(0.0f);
	if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_D) == GLFW_PRESS) {
		offset.x += 10.0f;
	}
	if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_A) == GLFW_PRESS) {
		offset.x -= 10.0f;
	}
	if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_W) == GLFW_PRESS) {
		offset.z += 10.0f;
	}
	if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_S) == GLFW_PRESS) {
		offset.z -= 10.0f;
	}
	if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_E) == GLFW_PRESS) {
		offset.y += 10.0f;
	}
	if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
		offset.y -= 10.0f;
	}
	cameraManager.MoveCamera(offset);


	//pause, resume
	static bool isSpaceRepeated = false;
	if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (!isSpaceRepeated)
		{
			isRunning = !isRunning;
			isSpaceRepeated = true;
		}
	}
	else if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE) {
		isSpaceRepeated = false;
	}

	//duplicate "Ctrl+D"
	static bool isCtrlDKeyRepeated = false;
	if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_D) == GLFW_PRESS &&
		glfwGetKey(renderer.GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		if (!isCtrlDKeyRepeated)
		{
			eventQueue.push(std::make_unique<DuplicateEvent>());
			isCtrlDKeyRepeated = true;
		}
	}
	else if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_D) == GLFW_RELEASE ||
		glfwGetKey(renderer.GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
		isCtrlDKeyRepeated = false;
	}
}

void Simulator::ClearSelectedObjectIDs()
{
	for (auto& obj : selectedObjects)
		obj->isSelected = false;

	selectedObjects.clear();
}