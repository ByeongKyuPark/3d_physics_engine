#pragma once
#include <vector>
#include <memory>//unique_ptr
#include <random>
#include "Object.h"
#include "math/mathConstants.h"
#include "engine/physicsWorld.h"
#include "engine/body.h"
#include "graphics/shape.h"
#include "graphics/textureImage.h"
#include "graphics/renderer.h"

using physics::RigidBody;
using physics::Quaternion;
using math::Vector3;
using physics::Matrix3;
using physics::Collider;
using physics::SphereCollider;
using physics::BoxCollider;
using graphics::Box;
using graphics::Sphere;
using graphics::Shape;
using graphics::Renderer;

template<typename T>
class ObjectPool {
private:
    static constexpr float SPAWNED_OBJECT_SCALE = 0.3f;

    physics::PhysicsWorld& physicsWorld;
    graphics::Renderer& renderer;
    std::random_device rd;
    std::mt19937 gen;
    int size;

    std::vector<std::unique_ptr<T>> pool;

private:
    void AddObject();
    void PopulateObjects();

public:
    ObjectPool(physics::PhysicsWorld& physicsWorld_, graphics::Renderer& renderer_, int size_) 
        :physicsWorld{ physicsWorld_ }, renderer{ renderer_ }, size{ size_ }, gen{rd()} {
        PopulateObjects();
    }
    void Resize(int newSize);
    void SpawnAll(Vector3 spawnerPos);
};


template<>
inline void ObjectPool<SphereObject>::AddObject(){
    SphereObject* newObject = new SphereObject;

    RigidBody* newBody = new RigidBody;
    newBody->SetMass(3.0f);
    newBody->SetPosition(0,0,0);
    newBody->SetLinearAcceleration(0.0f, 0.0f, 0.0f);

    Matrix3 inertiaTensor;
	float value = 0.4f * newBody->GetMass();
	inertiaTensor.SetDiagonal(value);
    newBody->SetInertiaTensor(inertiaTensor);

    newObject->SetRigidBody(newBody);

    Collider* newCollider{ nullptr };
    newCollider = new SphereCollider(newBody, SPAWNED_OBJECT_SCALE);
    newObject->SetCollider(newCollider);

    renderer.AddGraphicalShape(newObject);
    newObject->GetShape()->SetTextureID(TextureID::JEANS);

    newObject->SetScale(SPAWNED_OBJECT_SCALE);
    pool.push_back(std::unique_ptr<SphereObject>(newObject));
}

template<typename T>
inline void ObjectPool<T>::PopulateObjects() {
    for (int i{}; i < size; ++i) {
        AddObject();
    }
}

template<typename T>
inline void ObjectPool<T>::Resize(int newSize) {
    if (newSize < 0) {
        throw std::runtime_error("Spawner::resize(), negative size\n");
    }
    if (newSize < size) {
        for (int i{ newSize }; i < size; ++i) {
            physicsWorld.RemovePhysicsObject(pool[i].get());
            renderer.RemoveShape(pool[i].release());
        }
        pool.resize(newSize);
    }
    else if (newSize>size) {
        for (int i{ size }; i < newSize; ++i) {
            AddObject();
        }
    }
}

template<typename T>
inline void ObjectPool<T>::SpawnAll(Vector3 spawnerPos) {
    if (size <= 0) {
        return;
    }
    std::uniform_real_distribution<> posDist(-2.0, 2.0);  // range for position
    std::uniform_real_distribution<> upVelocityDist(0.1, 25.0);  // primary upwards velocity
    std::uniform_real_distribution<> sideVelocityDist(-5., 5.);  // spread in horizontal direction
    std::uniform_real_distribution<> degreeDist(0.0, 2.0 * math::PI); //range for angle
    std::uniform_real_distribution<> axisDist(-1.0, 1.0); //range for axis 

    for (auto& obj : pool) {

        //officially registers (owner is now the physicsWorld)
        float upVelocity = static_cast<float>(upVelocityDist(gen));
        float xVelocity = static_cast<float>(sideVelocityDist(gen));
        float zVelocity = static_cast<float>(sideVelocityDist(gen));

        Vector3 offset{ static_cast<float>(posDist(gen)),  static_cast<float>(posDist(gen)),  static_cast<float>(posDist(gen)) };

        obj.get()->GetRigidBody()->SetPosition(spawnerPos + offset); //ground is at y=0
        obj.get()->GetRigidBody()->SetLinearVelocity(Vector3{ xVelocity, upVelocity, zVelocity });
        obj.get()->GetRigidBody()->SetAngularVelocity(Vector3{ static_cast<float>(axisDist(gen)),  static_cast<float>(axisDist(gen)),  static_cast<float>(axisDist(gen)) });
        obj.get()->GetRigidBody()->SetOrientation(Quaternion{ static_cast<float>(degreeDist(gen)), Vector3{  static_cast<float>(axisDist(gen)),  static_cast<float>(axisDist(gen)),  static_cast<float>(axisDist(gen)) } });
        obj.get()->GetRigidBody()->SetLinearAcceleration(Vector3{ 0, -physics::PhysicsWorld::gravity, 0 }); // Assuming gravity effect

        physicsWorld.AddPhysicalObject(obj.release());//pass the ownership
        obj = nullptr;
        size--;
    }
    pool.resize(size);
}

template<>
inline void ObjectPool<BoxObject>::AddObject() {
    BoxObject* newObject = new BoxObject;

    RigidBody* newBody = new RigidBody;
    newBody->SetMass(2.5f);
    newBody->SetPosition(0, 0, 0);
    newBody->SetLinearAcceleration(0.0f, 0.0f, 0.0f);

    Matrix3 inertiaTensor;
    float value = newBody->GetMass() / 6.0f;
    inertiaTensor.SetDiagonal(value);
    newBody->SetInertiaTensor(inertiaTensor);

    newObject->SetRigidBody(newBody);

    Collider* newCollider{ nullptr };
    newCollider = new BoxCollider(newBody, SPAWNED_OBJECT_SCALE , SPAWNED_OBJECT_SCALE , SPAWNED_OBJECT_SCALE);
    newObject->SetCollider(newCollider);

    renderer.AddGraphicalShape(newObject);
    newObject->GetShape()->SetTextureID(TextureID::BALOONS);
    newObject->SetScale(SPAWNED_OBJECT_SCALE);

    pool.push_back(std::unique_ptr<BoxObject>(newObject));
}

class SphereBoxSpawner : public BoxObject {
private:
    ObjectPool<SphereObject> spherePool;
    ObjectPool<BoxObject> boxPool;

public:

    void spawnAll() {
        spherePool.SpawnAll(GetPosition());
        boxPool.SpawnAll(GetPosition());
    }
    SphereBoxSpawner(physics::PhysicsWorld& physicsWorld_, graphics::Renderer& renderer_, int initialSphereCount = 10, int initialBoxCount = 10)
        :spherePool(physicsWorld_, renderer_, initialSphereCount),
        boxPool(physicsWorld_, renderer_, initialBoxCount)
    {}
    ObjectType GetObjectType() const override final { return ObjectType::SPAWNER; }

};
