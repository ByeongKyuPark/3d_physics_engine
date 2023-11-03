#pragma once

#include "body.h"
#include <vector>
#include <utility>//std::pair

namespace physics
{
    struct ContactPoint {
        // If bool is false, either p1 or p2 are invalid and the contact points has not been found.
        // p1 and p2 are the contacts point on the object.
        std::pair<bool, Vector3> p1, p2;
    };
    //this struct refers to a specific space that contains all the possible initial conditions and final outcomes of a collision between objects
    struct CollisionManifold
    {
        RigidBody* bodies[2];
        Vector3 collisionNormal; //dir : body0 <--- body1
        float penetrationDepth;
        float restitution;
        float friction;
        float accumulatedNormalImpulse; //perpendicular to the collision surface, (frictions are parallel)
        ContactPoint contactPoint;

        CollisionManifold() :collisionNormal{}, penetrationDepth{}, restitution{}, friction{}, accumulatedNormalImpulse{}, contactPoint{} {
            bodies[0] = bodies[1] = nullptr;
        }
    };
} 