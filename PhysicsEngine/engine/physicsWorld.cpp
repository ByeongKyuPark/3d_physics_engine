#include "physicsWorld.h"
#include "simulator/object.h"
#include <iterator>
#include <typeinfo>
#include <cmath>
#include <iostream>

using namespace physics;

float PhysicsWorld::gravity = 9.8f;

PhysicsWorld::PhysicsWorld()  {
    constraints.emplace_back(std::make_unique<Plane>(Vector3(0.0f, 1.0f, 0.0f), 0.0f));
}

PhysicsWorld::~PhysicsWorld(){
    for (auto& obj : objects) {
        obj->FreePhysicsComponents();
    }
}

void PhysicsWorld::Simulate(float duration)
{
    //0. reset
    collisionManager.contacts.clear();

    //1. gravity
    for (auto& obj : objects) {
        obj->GetRigidBody()->AddForce(Vector3{ 0.f,-gravity,0.f } * obj->GetRigidBody()->GetMass());
    }

    //2. detect collisions
    collisionManager.DetectCollision(objects, constraints);

    //3. resolve collisions
    collisionManager.ResolveCollision(duration);

    //4. Integrate
    for (auto& obj : objects){
        obj->GetRigidBody()->Integrate(duration);
    }
}

void PhysicsWorld::AddRigidBody(float posX, float posY, float posZ,RigidObject* obj)
{
    RigidBody* newBody = new RigidBody;
    newBody->SetMass(5.0f);
    newBody->SetPosition(posX, posY, posZ);
    newBody->SetLinearAcceleration(0.0f, -gravity, 0.0f);

    Matrix3 inertiaTensor;

    if (dynamic_cast<SphereObject*>(obj)!=nullptr){
        float value = 0.4f * newBody->GetMass();
        inertiaTensor.SetDiagonal(value);
    }
    else if (dynamic_cast<BoxObject*>(obj)!=nullptr){
        float value = newBody->GetMass() / 6.0f;
        inertiaTensor.SetDiagonal(value);
    }
    newBody->SetInertiaTensor(inertiaTensor);

    obj->SetRigidBody(newBody);
}

void PhysicsWorld::AddCollider(RigidBody* rigidBody, RigidObject* obj){
    Collider* newCollider{nullptr};

    if (dynamic_cast<SphereObject*>(obj) != nullptr) {
        newCollider = new SphereCollider(rigidBody, 1.0f);
    }
    else if (dynamic_cast<BoxObject*>(obj) != nullptr) {
        newCollider = new BoxCollider(rigidBody, 0.5f, 0.5f, 0.5f);
    }

    obj->SetCollider(newCollider);
}

void physics::PhysicsWorld::AddPhysicalObject(RigidObject* obj) {
    objects.push_back(obj);
}

void PhysicsWorld::RemovePhysicsObject(RigidObject* obj){
    if (obj == nullptr) {
        throw std::runtime_error("nullptr passed to removePhysicsObject");
    }
    obj->FreePhysicsComponents();
}

float PhysicsWorld::CalcDistanceBetweenRayAndObject(
    const Vector3& rayOrigin,
    const Vector3& rayDirection,
    RigidObject* obj
)
{
    float distance;
    Collider* collider=obj->GetCollider();

    if (typeid(*collider) == typeid(SphereCollider))
    {
        SphereCollider* sphere = static_cast<SphereCollider*>(collider);
        distance = collisionManager.CaclRaySphereHitPointDistance(rayOrigin, rayDirection, *sphere);
    }
    else if (typeid(*collider) == typeid(BoxCollider))
    {
        BoxCollider* box = static_cast<BoxCollider*>(collider);
        distance = collisionManager.RayAndBox(rayOrigin, rayDirection, *box);
    }

    return distance;
}

void PhysicsWorld::SetGroundRestitution(float value){
    collisionManager.groundRestitution = value;
}

void PhysicsWorld::SetObjectRestitution(float value){
    collisionManager.objectRestitution = value;
}

void PhysicsWorld::SetGravity(float value){
    gravity = value;
    for(auto& obj:objects){
        obj->GetRigidBody()->SetLinearAcceleration(0.0f, -gravity, 0.0f);
    }
}
