#include "math/mathConstants.h"//PI
#include "event.h"
#include "simulator.h"
#include "objectSerialization.h"
#include "spawner.h"
#include <cmath>//cos,sin

int SaveScenarioEvent::savedScenarios = 1;

void ObjectAddEvent::Handle(Simulator& simulator) {
	switch (geometry) {
	case ObjectType::SPHERE:
		simulator.AddSphere();
		break;
	case ObjectType::BOX:
		simulator.AddBox();
		break;
	case ObjectType::SPAWNER:
		simulator.AddSpawner();
		break;
	default:
		break;
	}
}

void ObjectSelectEvent::Handle(Simulator& simulator) {
	std::vector<RigidObject*>& selectedObjects = simulator.GetSelectedObjects();
	if (isCtrlPressed == false)
	{
		for (const auto& obj : selectedObjects) {
			obj->SetSelected(false);
			//obj->isSelected = false;
		}
		selectedObjects.clear();
	}

	if (obj->GetIsSelected()) 
	{
		for (auto it = selectedObjects.begin(); it != selectedObjects.end(); ++it)
		{
			if (*it == obj)
			{
				selectedObjects.erase(it);
				break;
			}
		}
		obj->SetSelected(false);
	}
	else
	{
		selectedObjects.push_back(obj);
		obj->SetSelected(true);
	}
}

void SimulationToggleEvent::Handle(Simulator& simulator)
{
	simulator.isRunning = !simulator.isRunning;
}

void ObjectRemoveEvent::Handle(Simulator& simulator)
{
	std::vector<RigidObject*>& selectedObjects = simulator.GetSelectedObjects();
	for (const auto& obj : selectedObjects) {
		simulator.RemoveObject(obj);
	}
	selectedObjects.clear();
}

void ObjectPositionEvent::Handle(Simulator& simulator)
{
	physics::RigidBody* rigidBody = obj->GetRigidBody();
	rigidBody->SetPosition(position[0], position[1], position[2]);
	rigidBody->SetLinearVelocity(0.0f, 0.0f, 0.0f);
	rigidBody->SetAngularVelocity(0.0f, 0.0f, 0.0f);
}

void ObjectVelocityEvent::Handle(Simulator& simulator) {
	obj->GetRigidBody()->SetLinearVelocity(velocity[0], velocity[1], velocity[2]);
}

void ObjectScaleEvent::Handle(Simulator& simulator) {
	RigidObject* object = obj;
	object->SetScale(GRID_SCALE);
	//object->synchObjectData();
}

void ObjectMassEvent::Handle(Simulator& simulator) {
	obj->GetRigidBody()->SetMass(value);
}

void LeftMouseDragEvent::Handle(Simulator& simulator) {
	simulator.GetCameraManager().MoveCamera(
		glm::vec3(xOffset, yOffset, 0.0f)
	);
}

void RightMouseDragEvent::Handle(Simulator& simulator) {
	glm::vec3 dragStart = simulator.GetRenderer().ConvertScreenToWorld(glm::vec2(prevX, prevY));
	glm::vec3 dragEnd = simulator.GetRenderer().ConvertScreenToWorld(glm::vec2(curX, curY));

	glm::vec3 axis = glm::cross(dragStart, dragEnd);
	axis = glm::normalize(axis);
	dragEnd = glm::normalize(dragEnd);
	dragStart = glm::normalize(dragStart);
	float dotProduct = glm::dot(dragEnd, dragStart);
	if (dotProduct > 1.0f)
		dotProduct = 1.0f;
	float angle = -glm::acos(dotProduct);
	angle = glm::degrees(angle);

	simulator.GetCameraManager().RotateCamera(axis, angle);
}

void MouseWheelEvent::Handle(Simulator& simulator) {
	simulator.GetCameraManager().MoveCamera(glm::vec3(0.0f, 0.0f, value * 10.0f));
}

void LeftMouseClickEvent::Handle(Simulator& simulator) {
	glm::vec3 clickedPoint = simulator.GetRenderer().ConvertScreenToWorld(
		glm::vec2(screenX, screenY)
	);
	math::Vector3 origin(clickedPoint.x, clickedPoint.y, clickedPoint.z);

	glm::vec3 rayDirection = clickedPoint - simulator.GetCameraManager().GetCameraPosition();
	rayDirection = glm::normalize(rayDirection);
	math::Vector3 direction(rayDirection.x, rayDirection.y, rayDirection.z);

	float minDistance = FLT_MAX;
	RigidObject* minDistanceObject{ nullptr };
	for (const auto& object : simulator.GetSimulator().GetObjects())
	{
		float distance = simulator.GetSimulator().CalcDistanceBetweenRayAndObject(
			origin,
			direction,
			object
		);

		if (distance > 0.0f && distance < minDistance)
		{
			minDistance = distance;
			minDistanceObject = object;
		}
	}

	if (minDistanceObject != nullptr) {
		simulator.GetEventQueue().push(std::make_unique<ObjectSelectEvent>(minDistanceObject, isCtrlPressed));
	}
	else {
		simulator.ClearSelectedObjectIDs();
	}
}

void ObjectFixPositionEvent::Handle(Simulator& simulator) {
	if (shouldBeFixed)
	{
		obj->SetFixed(true);
		obj->GetRigidBody()->SetInverseMass(0.0f);
		obj->GetRigidBody()->SetInverseInertiaTensor(physics::Matrix3(0.0f));
		obj->GetRigidBody()->SetLinearVelocity(0.0f, 0.0f, 0.0f);
		obj->GetRigidBody()->SetAngularVelocity(0.0f, 0.0f, 0.0f);
	}
	else
	{
		obj->SetFixed(false);
		obj->GetRigidBody()->SetMass(5.0f);

		physics::Matrix3 inertiaTensor;
		Vector3 GRID_SCALE = obj->GetScale();
		if (typeid(*obj) == typeid(SphereObject))
		{
			SphereObject* sphere = static_cast<SphereObject*>(obj);
			inertiaTensor.SetDiagonal(0.4f * sphere->GetRigidBody()->GetMass() * GRID_SCALE.x * GRID_SCALE.x);
		}
		else if (typeid(*obj) == typeid(BoxObject))
		{
			BoxObject* box = static_cast<BoxObject*>(obj);
			float k = obj->GetRigidBody()->GetMass() / 12;
			float x = GRID_SCALE.x * 2.0f;
			float y = GRID_SCALE.y * 2.0f;
			float z = GRID_SCALE.z * 2.0f;
			inertiaTensor[0] = k * (y * y + z * z);
			inertiaTensor[4] = k * (x * x + z * z);
			inertiaTensor[8] = k * (y * y + x * x);
		}
		obj->GetRigidBody()->SetInertiaTensor(inertiaTensor);
	}
}

void ToggleContactRenderEvent::Handle(Simulator& simulator) {
	simulator.shouldRenderContactInfo = flag;
}

void ClearAllObjectsEvent::Handle(Simulator& simulator)
{
	simulator.isRunning = false;

	std::vector<RigidObject*>& objects = simulator.GetSimulator().GetObjects();
	auto iter = objects.begin();
	while (iter != objects.end())
	{
		iter = simulator.RemoveObject(*iter);
	}
	simulator.GetSelectedObjects().clear();
}

void GroundRestitutionEvent::Handle(Simulator& simulator) {
	simulator.GetSimulator().SetGroundRestitution(value);
}

void ObjectRestitutionEvent::Handle(Simulator& simulator) {
	simulator.GetSimulator().SetObjectRestitution(value);
}

void GravityEvent::Handle(Simulator& simulator) {
	simulator.GetSimulator().SetGravity(value);
}

void ObjectRotateEvent::Handle(Simulator& simulator) {
	RigidObject* target = obj;

	physics::Matrix4 transformMat = obj->GetRigidBody()->GetLocalToWorldMatrix();
	physics::Matrix3 rotationMat=transformMat.Extract3x3Matrix();

	math::Vector3 axisWorld(axisX, axisY, axisZ);
	math::Vector3 axisLocal = rotationMat.Transpose() * axisWorld;
	axisLocal.Normalize();

	physics::Quaternion quat(degree,axisLocal);

	obj->GetRigidBody()->RotateByQuat(quat);
}

void OrientationResetEvent::Handle(Simulator& simulator) {
	obj->GetRigidBody()->SetOrientation(physics::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
}

void ToggleWorldAxisRenderEvent::Handle(Simulator& simulator) {
	math::Vector3 pos = obj->GetRigidBody()->GetPosition();
	simulator.GetRenderer().RenderWorldAxisAt(axisIdx, pos.x, pos.y, pos.z);
}

void RemoveFloatingObjectsEvent::Handle(Simulator& simulator) {
	std::vector<RigidObject*>& selectedObjects = simulator.GetSelectedObjects();
	auto it = selectedObjects.begin();
	while (it != selectedObjects.end())
	{
		bool found{ false };
		std::vector<RigidObject*>& objs = simulator.GetSimulator().GetObjects();
		auto it2 = objs.begin();
		while (it2 != objs.end()) {
			if (*it == *it2) {
				found = true;
				break;
			}
			it2++;
		}
		if (found) {
			it = selectedObjects.erase(it);
		}
		else {
			++it;
		}

	}

	std::vector<RigidObject*>& objs = simulator.GetSimulator().GetObjects();
	std::vector<RigidObject*>::iterator iter = objs.begin();
	while (iter != objs.end())
	{
		if ((*iter)->GetIsFixed() == false)
			iter = simulator.RemoveObject(*iter);
		else
			++iter;
	}
}

void TimeStepChangeEvent::Handle(Simulator& simulator) {
	simulator.SetTimeStepMultiplier(value);
}

void SaveScenarioEvent::Handle(Simulator& simulator)
{
	std::vector<RigidObject*>& objs= simulator.GetSimulator().GetObjects();
	std::vector<ObjectData> serializedObjs;
	for (RigidObject* obj : objs) {
		const physics::RigidBody* body= obj->GetRigidBody();
		ObjectData data;
		data.pos = body->GetPosition();
		data.scl = obj->GetScale();
		data.vel = body->GetLinearVelocity();
		data.angVel = body->GetAngularVelocity();
		data.type = obj->GetObjectType();
		data.orientation = body->GetOrientation();
		data.textureID = obj->GetShape()->GetTextureID();
		data.mass = body->GetMass();
		data.IsFixed = obj->GetIsFixed();
		serializedObjs.push_back(data);
	}
	SaveObjectsToJson(serializedObjs, ++SaveScenarioEvent::savedScenarios);
}

void PresetLoadEvent::Handle(Simulator& simulator)
{
	std::vector<ObjectData> loadedObjects;
	LoadObjectsFromJson(loadedObjects,presetIdx);
	for (const ObjectData& objData : loadedObjects) {
		RigidObject* obj{ nullptr };
		if (objData.type == ObjectType::BOX) {
			obj = simulator.AddBox(objData.pos, TextureID(objData.textureID));
		}
		else if (objData.type == ObjectType::SPHERE) {
			obj = simulator.AddSphere(objData.pos, TextureID(objData.textureID));
		}
		else if (objData.type == ObjectType::SPAWNER) {
			obj = simulator.AddSpawner(objData.pos, TextureID(objData.textureID));
		}
		else {
			throw std::runtime_error("unidentified object type");
			//...
		}
		obj->SetScale(objData.scl);
		obj->GetRigidBody()->SetMass(objData.mass);
		obj->GetRigidBody()->SetLinearVelocity(objData.vel);
		obj->GetRigidBody()->SetOrientation(objData.orientation);

		if (objData.IsFixed == true) {
			ObjectFixPositionEvent evt2(obj, true);
			evt2.Handle(simulator);
		}
	}

}

void DuplicateEvent::Handle(Simulator& simulator){
	std::vector<RigidObject*>& selectedObjects = simulator.GetSelectedObjects();
	for (const RigidObject* obj : selectedObjects) {
		RigidObject* newObj{ nullptr };
		const physics::RigidBody* body = obj->GetRigidBody();
		if (obj->GetObjectType() == ObjectType::BOX) {
			newObj = simulator.AddBox(body->GetPosition(), TextureID(obj->GetShape()->GetTextureID()));
		}
		else if (obj->GetObjectType() == ObjectType::SPHERE) {
			newObj = simulator.AddSphere(body->GetPosition(), TextureID(obj->GetShape()->GetTextureID()));
		}
		else if (obj->GetObjectType() == ObjectType::SPAWNER) {
			newObj = simulator.AddSpawner(body->GetPosition(), TextureID(obj->GetShape()->GetTextureID()));
		}
		else {
			throw std::runtime_error("unidentified object type");
			//...
		}
		newObj->SetScale(obj->GetScale());
		newObj->GetRigidBody()->SetLinearVelocity(body->GetLinearVelocity());
		newObj->GetRigidBody()->SetOrientation(body->GetOrientation()
		);

		if (obj->GetIsFixed() == true) {
			ObjectFixPositionEvent evt2(newObj, true);
			evt2.Handle(simulator);
		}
	}
}

void DeselectObjectsEvent::Handle(Simulator& simulator){
	std::vector<RigidObject*>& selectedObjects = simulator.GetSelectedObjects();
	for (RigidObject* ro : selectedObjects) {
		ro->SetSelected(false);
	}
	selectedObjects.clear();
}
