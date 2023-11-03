#pragma once

#include "body.h"
#include "collisionManager.h"
#include "engine/contact.h"
#include "simulator/object.h"
#include <memory>//std::unique_ptr
#include <vector>
#include <unordered_map>

namespace physics
{
    class PhysicsWorld
    {
    public:
        typedef std::vector<physics::CollisionManifold> Manifolds;

        static float gravity;

    private:

        std::vector<RigidObject*> objects;
        std::vector<std::unique_ptr<Constraint>> constraints;//plane has no rigid rigidBody
        // http://gamedev.tutsplus.com/tutorials/implementation/create-custom-2d-physics-engine-aabb-circle-impulse-resolution/

        CollisionManager collisionManager;


    public:
        PhysicsWorld();
        ~PhysicsWorld();

        Manifolds GetManifold() const { return collisionManager.contacts; }

        std::vector<RigidObject*>& GetObjects() { return objects; }

        void Simulate(float duration);

        void AddRigidBody(float posX, float posY, float posZ, RigidObject* obj);

        void AddCollider(RigidBody*, RigidObject* obj);
        void AddPhysicalObject(RigidObject* obj);

        void RemovePhysicsObject(RigidObject* id);

        float CalcDistanceBetweenRayAndObject(
            const Vector3& rayOrigin,
            const Vector3& rayDirection,
            RigidObject* obj
        );

        void SetGroundRestitution(float value);
        void SetObjectRestitution(float value);
        void SetGravity(float value);
    };
}