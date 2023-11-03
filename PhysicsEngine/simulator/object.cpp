#include "object.h"

math::Vector3 RigidObject::GetPosition() const{
    return rigidBody->GetPosition();
}

math::Vector3 RigidObject::GetLinearVelocity() const{
    return rigidBody->GetLinearVelocity();
}

math::Vector3 RigidObject::GetAngularVelocity() const{
    return rigidBody->GetAngularVelocity();
}

math::Vector3 RigidObject::GetLinearAcceleration() const{
	return rigidBody->GetAcceleration();
}

float RigidObject::GetMass() const{
    return rigidBody->GetMass();
}

void RigidObject::FreePhysicsComponents() {
    delete rigidBody; rigidBody = nullptr;
    delete collider; collider = nullptr;
}

math::Vector3 SphereObject::GetScale() const{
    return{ radius,radius,radius };
}

void SphereObject::SetScale(float value){
    radius = value;
    SynchObjectData();
}

void SphereObject::SetScale(math::Vector3 v)
{
    radius = v.x;
    SynchObjectData();
}

void SphereObject::SynchObjectData()
{
    if (rigidBody->GetInverseMass() != 0.0f)
    {
        physics::Matrix3 inertiaTensor;
        inertiaTensor.SetDiagonal(0.4f * rigidBody->GetMass() * radius*radius);
        rigidBody->SetInertiaTensor(inertiaTensor);
    }

    collider->SetScale(radius);

    shape->GenerateShapeVertices(radius);
    shape->SetupPolygonAndFrameVAOs();
}

math::Vector3 BoxObject::GetScale() const{
    return { extentsX,extentsY,extentsZ };
}

void BoxObject::SetScale(float value)
{
    SetScale(math::Vector3{ value, value, value });
}

void BoxObject::SetScale(math::Vector3 v)
{
    extentsX = v.x;
    extentsY = v.y;
    extentsZ = v.z;
    SynchObjectData();
}

void BoxObject::SynchObjectData()
{
    if (rigidBody->GetInverseMass() != 0.0f)
    {
        physics::Matrix3 inertiaTensor;
        float k = rigidBody->GetMass() / 12;
        float x = extentsX * 2.0f;
        float y = extentsY * 2.0f;
        float z = extentsZ * 2.0f;
        inertiaTensor[0] = k * (y*y + z*z);
        inertiaTensor[4] = k * (x*x + z*z);
        inertiaTensor[8] = k * (y*y + x*x);
        rigidBody->SetInertiaTensor(inertiaTensor);
    }
    collider->SetScale(extentsX, extentsY, extentsZ);
    shape->GenerateShapeVertices({ extentsX, extentsY, extentsZ });
    shape->SetupPolygonAndFrameVAOs();
}

