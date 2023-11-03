#include <cmath>
#include <cfloat>
#include <typeinfo>
#include <algorithm>//std::clamp
#include <array>//std::array
#include <iostream>//std::cout
#include "collisionManager.h"
#include "math/compare.h"
#include "simulator/spawner.h"
#include "simulator/object.h"

using namespace physics;

void CollisionManager::DetectCollision(const std::vector<RigidObject*>& objects,const std::vector<std::unique_ptr<Constraint>>& constraints){
    std::vector<SphereBoxSpawner*> activatedSpawners;
    for (auto i = objects.begin(); i != objects.end(); ++i)
    {
        Collider* shape1 = (*i)->GetCollider();
        //(1) Rigid Bodies
        for (auto j = std::next(i, 1); j != objects.end(); ++j)
        {
            Collider* shape2 = (*j)->GetCollider();

            if (FindCollisionFeatures(shape1, shape2) == true) {
                if ((*i)->GetObjectType() == ObjectType::SPAWNER) {
                    activatedSpawners.push_back(dynamic_cast<SphereBoxSpawner*>(*i));
                }
                if ((*j)->GetObjectType() == ObjectType::SPAWNER) {
                    activatedSpawners.push_back(dynamic_cast<SphereBoxSpawner*>(*j));
                }
            }
        }

        //(2) constraints     
        for (auto& constraint : constraints) {
            if(FindCollisionFeatures(shape1, constraint.get())==true){
                if ((*i)->GetObjectType() == ObjectType::SPAWNER) {
                    activatedSpawners.push_back(dynamic_cast<SphereBoxSpawner*>(*i));
                }
            }
        }
    }
    for(SphereBoxSpawner* spanwer : activatedSpawners){
        spanwer->spawnAll();
    }
}

bool physics::CollisionManager::FindCollisionFeatures(const Collider* shape1, const Collider* shape2){
    if (typeid(*shape1) == typeid(SphereCollider))
    {
        const SphereCollider* sphere = static_cast<const SphereCollider*>(shape1);
        if (typeid(*shape2) == typeid(SphereCollider))
        {
            return FindCollisionFeatures(sphere, static_cast<const SphereCollider*>(shape2));
        }
        else if (typeid(*shape2) == typeid(BoxCollider))
        {
            return FindCollisionFeatures(static_cast<const BoxCollider*>(shape2), sphere);
        }
    }
    else if (typeid(*shape1) == typeid(BoxCollider))
    {
        const BoxCollider* box = static_cast<const BoxCollider*>(shape1);
        if (typeid(*shape2) == typeid(BoxCollider))
        {
            return FindCollisionFeatures(box, static_cast<const BoxCollider*>(shape2));
        }
        else if (typeid(*shape2) == typeid(SphereCollider))
        {
            return FindCollisionFeatures(box, static_cast<const SphereCollider*>(shape2));
        }
    }
    return false;
}

bool CollisionManager::FindCollisionFeatures(const BoxCollider* box, const SphereCollider* sphere) {
    constexpr int NUM_AXES = 3;
    Vector3 centerToCenter = sphere->rigidBody->GetPosition() - box->rigidBody->GetPosition();
    Vector3 extents = box->extents;
    Vector3 closestPoint = box->rigidBody->GetPosition();

    //for X,Y,Z axes
    for (int i = 0; i < NUM_AXES; ++i) {
        Vector3 axis = box->rigidBody->GetAxis(i);
        float extent = extents.x;
        if (i == 1) {
            extent = extents.y;
        }
        else if (i == 2) {
            extent = extents.z;
        }

        float projectionLength = centerToCenter.Dot(axis);
        // clamp the projection along the current axis
        projectionLength = std::clamp(projectionLength, -extent, extent);

        // accumulate for the closest point
        closestPoint += axis * projectionLength;
    }

    // Calculate squared distance to sphere center
    float distanceSquared = (closestPoint - sphere->rigidBody->GetPosition()).LengthSquared();

    if (distanceSquared > sphere->radius * sphere->radius) {
        return false; // no collision
    }
    //deal with collision
    CollisionManifold newContact;
    newContact.bodies[0] = sphere->rigidBody;
    newContact.bodies[1] = box->rigidBody;
    newContact.collisionNormal = sphere->rigidBody->GetPosition() - closestPoint;
    newContact.collisionNormal.Normalize();
    newContact.contactPoint ={
        { true, Vector3(sphere->rigidBody->GetPosition() - newContact.collisionNormal * sphere->radius) },
        { true, Vector3(closestPoint) }
        };
    newContact.penetrationDepth = sphere->radius - sqrtf(distanceSquared);
    newContact.restitution = objectRestitution;
    newContact.friction = friction;
    contacts.push_back(newContact);

    return true;
}
bool CollisionManager::FindCollisionFeatures(const SphereCollider* sphere1,const SphereCollider* sphere2,bool isForBroadPhaseTest){
    float distanceSquared = (sphere1->rigidBody->GetPosition() - sphere2->rigidBody->GetPosition()).LengthSquared();

    float radiusSum = sphere1->radius + sphere2->radius;
    if (distanceSquared > radiusSum * radiusSum) {
        return false;
    }
    if (isForBroadPhaseTest == true) {//no need to calc details
        return true;
    }

	Vector3 normal = sphere1->rigidBody->GetPosition() - sphere2->rigidBody->GetPosition();
	normal.Normalize();

    CollisionManifold newContact;
    newContact.bodies[0] = sphere1->rigidBody;
	newContact.bodies[1] = sphere2->rigidBody;
	newContact.collisionNormal = normal;
    newContact.contactPoint = { { true, Vector3(sphere1->rigidBody->GetPosition() - normal * sphere1->radius) },
                                { true, Vector3(sphere2->rigidBody->GetPosition() + normal * sphere2->radius) }};
	newContact.penetrationDepth = radiusSum - sqrtf(distanceSquared);
	newContact.restitution = objectRestitution;
	newContact.friction = friction;
    contacts.push_back(newContact);

    return true;
}

bool CollisionManager::FindCollisionFeatures(const SphereCollider* sphere, const Plane* plane){
    float distance = std::abs(plane->normal.Dot(sphere->rigidBody->GetPosition())-plane->distance);

    if (distance > sphere->radius) {
        return false;
    }

	CollisionManifold newContact;
	newContact.bodies[0] = sphere->rigidBody;
	newContact.bodies[1] = nullptr;
	newContact.collisionNormal = plane->normal;
	newContact.contactPoint={{ true,Vector3(sphere->rigidBody->GetPosition() - plane->normal * distance) },
		                    { false,Vector3{}}};
	newContact.penetrationDepth = sphere->radius - distance;
	newContact.restitution = groundRestitution;
	newContact.friction = friction;
	contacts.push_back(newContact);
	return true;
}

bool CollisionManager::FindCollisionFeatures(const BoxCollider* box1, const BoxCollider* box2){
    RigidBody b1;
    RigidBody b2;
    b1.SetPosition(box1->rigidBody->GetPosition());
    b2.SetPosition(box2->rigidBody->GetPosition());

    SphereCollider s1{ &b1, box1->extents.Length()};
    SphereCollider s2{ &b2, box2->extents.Length()};

    if (FindCollisionFeatures(&s1, &s2,true)==false) {//'true' : only for the broad phase test
        return false;
    }

    std::vector<Vector3> axes;

    //face(box1) <-> vertex(box2)
    for (int i{}; i < 3; ++i) {
        axes.push_back(box1->rigidBody->GetAxis(i));
    }

    //face(box2) <-> vertex(box1)
    for (int i{}; i < 3; ++i) {
        axes.push_back(box2->rigidBody->GetAxis(i));
    }

    //edge-edge
    for (int i{}; i < 3; ++i) {
        for (int j{}; j < 3; ++j) {
            Vector3 crossProduct = axes[i].Cross(axes[3 + j]);
            crossProduct.Normalize();
            axes.push_back(crossProduct);
        }
    }

    float minPenetration = FLT_MAX;
    int minAxisIdx = 0;

    for (int i{}; i < axes.size(); ++i){
        float penetration = CalcPenetration(*box1, *box2, axes[i]);

        if (penetration <= 0.f) {
            return false; //early exit
        }

        if (penetration <= minPenetration){
            minPenetration = penetration;
            minAxisIdx = i;
        }
    }

    CollisionManifold newContact;
    newContact.bodies[0] = box1->rigidBody;
    newContact.bodies[1] = box2->rigidBody;
    newContact.penetrationDepth = minPenetration;
    newContact.restitution = objectRestitution;
    newContact.friction = friction;

    // Calculate the vector pointing from the center of box2 to the center of box1.
    Vector3 box2ToBox1 = box1->rigidBody->GetPosition() - box2->rigidBody->GetPosition();

    // Determine the direction of the collision normal (collisionNormal).
    // want the collisionNormal to always point from box2 towards box1.
    newContact.collisionNormal = (axes[minAxisIdx].Dot(box2ToBox1) < 0) ? axes[minAxisIdx] * -1.f : axes[minAxisIdx];

    CalcOBBsContactPoints(*box1, *box2, newContact, minAxisIdx);
    
    contacts.push_back(newContact);
    return true;
}

bool physics::CollisionManager::FindCollisionFeatures(const Collider* collider, const Constraint* constraint){
    if (typeid(*collider) == typeid(SphereCollider)){
        const SphereCollider* sphere = static_cast<const SphereCollider*>(collider);
        if (typeid(*constraint) == typeid(Plane)) {
            return FindCollisionFeatures(sphere, static_cast<const Plane*>(constraint));
        }
    }
    else if (typeid(*collider) == typeid(BoxCollider)){
        const BoxCollider* box = static_cast<const BoxCollider*>(collider);
        if (typeid(*constraint) == typeid(Plane)) {
            return FindCollisionFeatures(box, static_cast<const Plane*>(constraint));
        }
    }
    return false;
}

bool CollisionManager::FindCollisionFeatures(const BoxCollider* box,const Plane* plane){
    Vector3 vertices[8];
    vertices[0] = Vector3(-box->extents.x, box->extents.y, box->extents.z);
    vertices[1] = Vector3(-box->extents.x, -box->extents.y, box->extents.z);
    vertices[2] = Vector3(box->extents.x, -box->extents.y, box->extents.z);
    vertices[3] = Vector3(box->extents.x, box->extents.y, box->extents.z);

    vertices[4] = Vector3(-box->extents.x, box->extents.y, -box->extents.z);
    vertices[5] = Vector3(-box->extents.x, -box->extents.y, -box->extents.z);
    vertices[6] = Vector3(box->extents.x, -box->extents.y, -box->extents.z);
    vertices[7] = Vector3(box->extents.x, box->extents.y, -box->extents.z);

    for (int i = 0; i < 8; ++i) {
        vertices[i] = box->rigidBody->GetLocalToWorldMatrix() * vertices[i];
    }

    bool hasContacted = false;

    for (int i = 0; i < 8; ++i)
    {
        float distance = plane->normal.Dot(vertices[i]);

        if (distance < plane->distance)
        {
            CollisionManifold newContact;
            newContact.bodies[0] = box->rigidBody;
            newContact.bodies[1] = nullptr;
            newContact.collisionNormal = plane->normal;
            newContact.contactPoint={{ true, Vector3(vertices[i]) },
                                        {false, Vector3{}}};
            newContact.penetrationDepth = plane->distance -distance;
            newContact.restitution = groundRestitution;
            newContact.friction = friction;

            contacts.push_back(newContact);
            hasContacted = true;
        }
    }
    return hasContacted;
}

float CollisionManager::CaclRaySphereHitPointDistance(const Vector3& origin,const Vector3& direction,const SphereCollider& sphere){
    Vector3 originToSphere = sphere.rigidBody->GetPosition() - origin;
    float originToSphereProjected = originToSphere.Dot(direction);
    float orthogonalDistanceSquared =
        originToSphere.LengthSquared() - originToSphereProjected * originToSphereProjected;
    if (orthogonalDistanceSquared > sphere.radius * sphere.radius) {
        return -1.0f;
    }

    float t1 = sqrtf(sphere.radius*sphere.radius - orthogonalDistanceSquared);
    float hitPointDistance = originToSphereProjected - t1;

    return hitPointDistance;
}

float CollisionManager::RayAndBox(const Vector3& origin,const Vector3& direction,const BoxCollider& box){
    Vector3 originToBox = box.rigidBody->GetPosition() - origin;
    float tNearMax = 0.0f;
    float tFarMin = FLT_MAX;
    
    for (int i = 0; i < 3; ++i)
    {
        Vector3 axis = box.rigidBody->GetAxis(i);
        float originToBoxProjected = axis.Dot(originToBox);
        float rayDirectionProjected = axis.Dot(direction);

        if (fabs(rayDirectionProjected) > 0.001f)
        {
            float t1 = (originToBoxProjected - box.extents[i]) / rayDirectionProjected;
            float t2 = (originToBoxProjected + box.extents[i]) / rayDirectionProjected;
            if (t1 > t2)
            {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }
            if (t1 > tNearMax)
                tNearMax = t1;
            if (t2 < tFarMin)
                tFarMin = t2;
            if (tFarMin < tNearMax)
                return -1.0f;
        }
        else
        {
            if (-originToBoxProjected - box.extents[i] > 0.0f
                    || -originToBoxProjected + box.extents[i] < 0.0f)
                return -1.0f;
        }
    }

    return tNearMax;
}

float CollisionManager::CalcPenetration(const BoxCollider& box1, const BoxCollider& box2, const Vector3& axis){
    Vector3 centerToCenter = box2.rigidBody->GetPosition() - box1.rigidBody->GetPosition();
    float projectedCenterToCenter = abs(centerToCenter.Dot(axis));

    float projectedSum = 
          abs((box1.rigidBody->GetAxis(0) * box1.extents.x).Dot(axis))
        + abs((box1.rigidBody->GetAxis(1) * box1.extents.y).Dot(axis))
        + abs((box1.rigidBody->GetAxis(2) * box1.extents.z).Dot(axis))
        
        + abs((box2.rigidBody->GetAxis(0) * box2.extents.x).Dot(axis))
        + abs((box2.rigidBody->GetAxis(1) * box2.extents.y).Dot(axis))
        + abs((box2.rigidBody->GetAxis(2) * box2.extents.z).Dot(axis));

    return projectedSum - projectedCenterToCenter;
}

void CollisionManager::CalcOBBsContactPoints(const BoxCollider& box1, const BoxCollider& box2, CollisionManifold& newContact, int minPenetrationAxisIdx) const{
    //  	1. for cases 0 to 5, vertices are found to define contact points
    if (minPenetrationAxisIdx >= 0 && minPenetrationAxisIdx < 3)
    {
        Vector3 contactPoint = box2.GetLocalContactVertex(newContact.collisionNormal, math::Less);

        contactPoint = box2.rigidBody->GetLocalToWorldMatrix() * contactPoint;
        newContact.contactPoint={{true, contactPoint + newContact.collisionNormal * newContact.penetrationDepth},
                                {true, contactPoint}};
    }
    else if (minPenetrationAxisIdx >= 3 && minPenetrationAxisIdx < 6) {
        Vector3 contactPoint = box1.GetLocalContactVertex(newContact.collisionNormal, math::Greater);

        contactPoint = box1.rigidBody->GetLocalToWorldMatrix() * contactPoint;
        newContact.contactPoint={{true, contactPoint},
                            {true, contactPoint - newContact.collisionNormal * newContact.penetrationDepth}};
    }
    //    2. for cases 6 to 15, points on the edges of the bounding boxes are used.
    else
    {
        // Determine the local contact vertex on box1 based on the collision's hit normal.
        // hit normal : box1 < -- - box2
        // For example:
        // If the dot product of box1's X-axis and the hit normal is positive,
        // it indicates that the collision is happening on the negative side of the X-axis.
        // In such a case, the x-coordinate of the contact vertex (vertexOne) should be negative 
        // to represent the correct side of the box where the collision occurred.
        Vector3 vertexOne = box1.GetLocalContactVertex(newContact.collisionNormal, math::Greater);
        Vector3 vertexTwo = box2.GetLocalContactVertex(newContact.collisionNormal, math::Less);


        int testAxis1{ -1 }, testAxis2{-1};
        
        switch (minPenetrationAxisIdx)
        {
        case 6: // x,x
            testAxis1 = 0;
            testAxis2 = 0;
            break;

        case 7: // x,y
            testAxis1 = 0;
            testAxis2 = 1;
            break;

        case 8: // x,z
            testAxis1 = 0;
            testAxis2 = 2;
            break;

        case 9: // y,x
            testAxis1 = 1;
            testAxis2 = 0;
            break;

        case 10: // y,y
            testAxis1 = 1;
            testAxis2 = 1;
            break;

        case 11: // y,z
            testAxis1 = 1;
            testAxis2 = 2;
            break;

        case 12: // z,x
            testAxis1 = 2;
            testAxis2 = 0;
            break;

        case 13: // z,y
            testAxis1 = 2;
            testAxis2 = 1;
            break;

        case 14: // z,z
            testAxis1 = 2;
            testAxis2 = 2;
            break;

        default:
            break;
        }
        //orientation of the colliding edges (not the moving away)
        Vector3 edge1, edge2;//world space
        //ex. case 6(x,x): vertexTwo.x <0, then local x-axis should be flipped
        edge1 = (vertexOne[testAxis1] < 0) ? box1.rigidBody->GetAxis(testAxis1) : box1.rigidBody->GetAxis(testAxis1) * -1.f;
        edge2 = (vertexTwo[testAxis2] < 0) ? box2.rigidBody->GetAxis(testAxis2) : box2.rigidBody->GetAxis(testAxis2) * -1.f;

        //local -> world
        vertexOne = box1.rigidBody->GetLocalToWorldMatrix() * vertexOne;
        vertexTwo = box2.rigidBody->GetLocalToWorldMatrix() * vertexTwo;

        //1. calculate the dot product between edge1 and edge2:
        float k = edge1.Dot(edge2);//cosine

        //2.  point on the edge of box1 closest to the initial contact point on box2
        //    The calculation involves projecting the vector from contactPoint1 to contactPoint2 onto the direction of edge1 - edge2 * k.
        Vector3 closestPointOne = { vertexOne + edge1 * ((vertexTwo - vertexOne).Dot(edge1 - edge2 * k) / (1 - k * k)) };

        //3. point on the edge of box2 closest to 
        //projecting the vector from closestPointOne to vertexTwo onto direction2.
        Vector3 closestPointTwo{ vertexTwo + edge2 * ((closestPointOne - vertexTwo).Dot(edge2)) };

        newContact.contactPoint={{ true, closestPointOne },
                                { true, closestPointTwo }};
    }
}


//https://allenchou.net/2013/12/game-physics-constraints-sequential-impulse/
//https://code.tutsplus.com/series/how-to-create-a-custom-physics-engine--gamedev-12715
void CollisionManager::SequentialImpulse(CollisionManifold contact, float deltaTime) {
    // Compute the effective mass
    float inverseMassSum = contact.bodies[0]->GetInverseMass();
    if (contact.bodies[1]) {
        inverseMassSum += contact.bodies[1]->GetInverseMass();
    }
    if (inverseMassSum == 0.0f) {
        return;
    }

    // Contact point relative to the body's position
    Vector3 r1 = contact.contactPoint.p1.second - contact.bodies[0]->GetPosition();
    Vector3 r2;

    // Inverse inertia tensors
    Matrix3 i1 = contact.bodies[0]->GetInverseInertiaTensorWorld();
    Matrix3 i2;

    // Denominator terms
    Vector3 termInDenominator1 = (i1 * r1.Cross(contact.collisionNormal)).Cross(r1);
    Vector3 termInDenominator2;

    if (contact.bodies[1]) {
        r2 = contact.contactPoint.p2.second - contact.bodies[1]->GetPosition();
        i2 = contact.bodies[1]->GetInverseInertiaTensorWorld();
        termInDenominator2 = (i2 * r2.Cross(contact.collisionNormal)).Cross(r2);
    }

    // Compute the final effective mass

                        //1. linear part      
    float effectiveMass = inverseMassSum 
                                +
                        //2. angular part
                        (termInDenominator1 + termInDenominator2).Dot(contact.collisionNormal);
    if (effectiveMass == 0.0f) {
        return;
    }

    // Relative velocities
    Vector3 relativeVel = contact.bodies[0]->GetLinearVelocity() + contact.bodies[0]->GetAngularVelocity().Cross(r1);
    if (contact.bodies[1]) {
        relativeVel -= (contact.bodies[1]->GetLinearVelocity() + contact.bodies[1]->GetAngularVelocity().Cross(r2));
    }

    float relativeSpeed = relativeVel.Dot(contact.collisionNormal);

    // Baumgarte Stabilization (for penetration & sinking resolution)
    float baumgarte = 0.0f;
    constexpr float CORRECTION_RATIO = 0.1f;
    if (contact.penetrationDepth > penetrationTolerance) {
        baumgarte = ((contact.penetrationDepth - penetrationTolerance) * CORRECTION_RATIO / deltaTime);
    }

    float restitutionTerm = 0.0f;
    if (relativeSpeed > closingSpeedTolerance) {
        restitutionTerm = contact.restitution * (relativeSpeed - closingSpeedTolerance);
    }

    // Compute the impulse
    float jacobianImpulse = ((-(1 + restitutionTerm) * relativeSpeed) + baumgarte) / effectiveMass;

    if (isnan(jacobianImpulse)) {
        return; 
    }

    // Clamp the accumulated impulse
    float oldAccumulatedNormalImpulse = contact.accumulatedNormalImpulse;
    contact.accumulatedNormalImpulse = std::max(oldAccumulatedNormalImpulse + jacobianImpulse, 0.0f);
    jacobianImpulse = contact.accumulatedNormalImpulse - oldAccumulatedNormalImpulse;

    // Apply impulses to the bodies
    ApplyImpulses(contact, jacobianImpulse, r1, r2, contact.collisionNormal);

    // Compute and apply frictional impulses using the two tangents
    ApplyFrictionImpulses(contact, r1, r2);
}

void CollisionManager::ResolveCollision(float deltaTime){
    for (int i = 0; i < iterationLimit; ++i){
        for (auto& contact : contacts){
			SequentialImpulse(contact, deltaTime);
        }
    }
}

float CollisionManager::ComputeTangentialImpulses(CollisionManifold& contact, const Vector3& r1, const Vector3& r2, const Vector3& tangent) {
    float inverseMassSum = contact.bodies[0]->GetInverseMass();
    Vector3 termInDenominator1 = (contact.bodies[0]->GetInverseInertiaTensorWorld() * r1.Cross(tangent)).Cross(r1);
    Vector3 termInDenominator2;

    if (contact.bodies[1]) {
        inverseMassSum += contact.bodies[1]->GetInverseMass();
        termInDenominator2 = (contact.bodies[1]->GetInverseInertiaTensorWorld() * r2.Cross(tangent)).Cross(r2);
    }

    // Compute the effective mass for the friction/tangential direction
    float effectiveMassTangential = inverseMassSum + (termInDenominator1 + termInDenominator2).Dot(tangent);
    if (effectiveMassTangential == 0.0f) {
        return 0.0f;
    }

    // Calculate relative velocities along the tangent
    Vector3 relativeVel = contact.bodies[0]->GetLinearVelocity() + contact.bodies[0]->GetAngularVelocity().Cross(r1);
    if (contact.bodies[1]) {
        relativeVel -= (contact.bodies[1]->GetLinearVelocity() + contact.bodies[1]->GetAngularVelocity().Cross(r2));
    }

    float relativeSpeedTangential = relativeVel.Dot(tangent);

    // Compute the frictional impulse
    float frictionImpulseMagnitude = -relativeSpeedTangential / effectiveMassTangential;

    // Coulomb's law: The frictional impulse should not be greater than the friction coefficient times the normal impulse
    float maxFriction = contact.friction * contact.accumulatedNormalImpulse;
    frictionImpulseMagnitude = std::clamp(frictionImpulseMagnitude, -maxFriction, maxFriction);

    return frictionImpulseMagnitude;
}

void CollisionManager::ApplyFrictionImpulses(CollisionManifold& contact, const Vector3& r1, const Vector3& r2) {

    // Compute the two friction directions
    Vector3 tangent1, tangent2;
    //erin catto - Box2D
    if (abs(contact.collisionNormal.x) >= 0.57735f) {
        tangent1 = Vector3(contact.collisionNormal.y, -contact.collisionNormal.x, 0.0f);
    }
    else {
        tangent1 = Vector3(0.0f, contact.collisionNormal.z, -contact.collisionNormal.y);
    }
    tangent2 = contact.collisionNormal.Cross(tangent1);

    // Compute the impulses in each direction and apply
    float jacobianImpulseT1 = ComputeTangentialImpulses(contact, r1, r2, tangent1);
    ApplyImpulses(contact, jacobianImpulseT1, r1, r2, tangent1);

    float jacobianImpulseT2 = ComputeTangentialImpulses(contact, r1, r2, tangent2);
    ApplyImpulses(contact, jacobianImpulseT2, r1, r2, tangent2);
}

void CollisionManager::ApplyImpulses(CollisionManifold& contact, float jacobianImpulse, const Vector3& r1, const Vector3& r2, const Vector3& direction) {
    Vector3 linearImpulse = direction * jacobianImpulse;
    Vector3 angularImpulse1 = r1.Cross(direction) * jacobianImpulse;
    Vector3 angularImpulse2 = r2.Cross(direction) * jacobianImpulse;

    contact.bodies[0]->SetLinearVelocity(
        contact.bodies[0]->GetLinearVelocity() + linearImpulse * contact.bodies[0]->GetInverseMass()
    );
    contact.bodies[0]->SetAngularVelocity(
        contact.bodies[0]->GetAngularVelocity() + contact.bodies[0]->GetInverseInertiaTensorWorld() * angularImpulse1
    );
    if (contact.bodies[1]) {
        contact.bodies[1]->SetLinearVelocity(
            contact.bodies[1]->GetLinearVelocity() - linearImpulse * contact.bodies[1]->GetInverseMass()
        );
        contact.bodies[1]->SetAngularVelocity(
            contact.bodies[1]->GetAngularVelocity() - contact.bodies[1]->GetInverseInertiaTensorWorld() * angularImpulse2
        );
    }
}