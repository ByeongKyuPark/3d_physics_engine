#pragma once

#include "collider.h"
#include "simulator/object.h"
#include <memory>//std::unique_ptr
#include <vector>
#include <unordered_map>

namespace physics
{
    class CollisionManager
    {
        friend class PhysicsWorld;
        
    private:
        float friction;
        float objectRestitution;
        float groundRestitution;

        int iterationLimit;
        float penetrationTolerance;
        float closingSpeedTolerance;

        std::vector<physics::CollisionManifold> contacts;

    public:
        CollisionManager()
            : friction(0.6f), objectRestitution(0.5f), groundRestitution(0.2f),
            iterationLimit(30), penetrationTolerance(0.0005f), closingSpeedTolerance(0.005f) {}
    
        void DetectCollision(const std::vector<RigidObject*>& objects, const std::vector<std::unique_ptr<Constraint>>& constraints);
        void ResolveCollision(float deltaTime);
    
    private:
        //(1)RigidBodies
        bool FindCollisionFeatures(const Collider*,const Collider*);
        bool FindCollisionFeatures(const BoxCollider*,const SphereCollider*);
        bool FindCollisionFeatures(const SphereCollider*,const SphereCollider*,bool isForBroadPhaseTest=false);
        bool FindCollisionFeatures(const BoxCollider*,const BoxCollider*);

        //(2)Constraints
        bool FindCollisionFeatures(const Collider*,const Constraint*);

        bool FindCollisionFeatures(const SphereCollider*,const Plane*);
        bool FindCollisionFeatures(const BoxCollider*,const Plane*);

        float CaclRaySphereHitPointDistance(const Vector3& origin,const Vector3& direction,const SphereCollider&);
        float RayAndBox(const Vector3& origin, const Vector3& direction, const BoxCollider&);
    
    private:
        float CalcPenetration( const BoxCollider& box1, const BoxCollider& box2, const Vector3& axis);
        void CalcOBBsContactPoints(const BoxCollider& box1, const BoxCollider& box2, CollisionManifold& newContact, int minPenetrationAxisIdx) const;
        void SequentialImpulse(CollisionManifold contact, float deltaTime);
        void ApplyImpulses(CollisionManifold& contact, float jacobianImpulse, const Vector3& r1, const Vector3& r2, const Vector3& direction);
        void ApplyFrictionImpulses(CollisionManifold& contact, const Vector3& r1, const Vector3& r2);
        float ComputeTangentialImpulses(CollisionManifold& contact, const Vector3& r1, const Vector3& r2, const Vector3& tangent);
    };
}