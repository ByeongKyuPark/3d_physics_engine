#include "body.h"
#include <cmath>
#include <iostream>

using namespace physics;

void RigidBody::Integrate(float duration)
{
    if (massInverse == 0.0f) {
        return;
    }

    //1. linear Velocity
    Vector3 linearAcceleration = force * massInverse;
    velocity += linearAcceleration * duration;
    velocity *= powf(linearDamping, duration);

    //2. angular Velocity
    Vector3 angularAcceleration = inverseInertiaTensorWorld * torque;
    angularVelocity += angularAcceleration * duration;
    angularVelocity *= powf(angularDamping, duration);

    //3.Pos, Orientation
    position += velocity * duration;
    orientation += orientation.RotateByScaledVector(angularVelocity, duration / 2.0f);
    orientation.Normalize();    

    //5.update accordingly
    UpdateTransformMatrix();
    TransformInertiaTensor();

    force.Clear();
    torque.Clear();
}

void RigidBody::AddForce(const Vector3& _force){
    force += _force;
}

void RigidBody::AddForceAt(const Vector3& _force, const Vector3& point)
{
    force += _force;
    Vector3 pointFromCenter = point - position;
    torque += pointFromCenter.Cross(_force);
}

Vector3 RigidBody::GetAxis(int index) const
{
    if (index < 0 || index > 2){
        throw std::runtime_error("RigidBody::getAxis(): index, out of bounds\n");
    }

    Vector3 result(
        localToWorldCoord[index*4],
        localToWorldCoord[index*4 + 1],
        localToWorldCoord[index*4 + 2]
    );
    result.Normalize();

    return result;
}

void RigidBody::RotateByQuat(const Quaternion& quat)
{
    Quaternion newOrientation = GetOrientation() * quat;
    newOrientation.Normalize();
    SetOrientation(newOrientation);
}

void RigidBody::UpdateTransformMatrix()
{
    // First column
    localToWorldCoord.columns[0].m128_f32[0] = 1.0f - 2.0f * (orientation.y * orientation.y + orientation.z * orientation.z);
    localToWorldCoord.columns[0].m128_f32[1] = 2.0f * (orientation.x * orientation.y + orientation.w * orientation.z);
    localToWorldCoord.columns[0].m128_f32[2] = 2.0f * (orientation.x * orientation.z - orientation.w * orientation.y);
    localToWorldCoord.columns[0].m128_f32[3] = 0.0f; // Assuming homogeneous coordinate for direction vectors is 0

    // Second column
    localToWorldCoord.columns[1].m128_f32[0] = 2.0f * (orientation.x * orientation.y - orientation.w * orientation.z);
    localToWorldCoord.columns[1].m128_f32[1] = 1.0f - 2.0f * (orientation.x * orientation.x + orientation.z * orientation.z);
    localToWorldCoord.columns[1].m128_f32[2] = 2.0f * (orientation.y * orientation.z + orientation.w * orientation.x);
    localToWorldCoord.columns[1].m128_f32[3] = 0.0f; // Assuming homogeneous coordinate for direction vectors is 0

    // Third column
    localToWorldCoord.columns[2].m128_f32[0] = 2.0f * (orientation.x * orientation.z + orientation.w * orientation.y);
    localToWorldCoord.columns[2].m128_f32[1] = 2.0f * (orientation.y * orientation.z - orientation.w * orientation.x);
    localToWorldCoord.columns[2].m128_f32[2] = 1.0f - 2.0f * (orientation.x * orientation.x + orientation.y * orientation.y);
    localToWorldCoord.columns[2].m128_f32[3] = 0.0f; // Assuming homogeneous coordinate for direction vectors is 0

    // Fourth column (position)
    localToWorldCoord.columns[3].m128_f32[0] = position[0];
    localToWorldCoord.columns[3].m128_f32[1] = position[1];
    localToWorldCoord.columns[3].m128_f32[2] = position[2];
    localToWorldCoord.columns[3].m128_f32[3] = 1.0f; // Homogeneous coordinate for position is 1
}

void RigidBody::TransformInertiaTensor()
{
    Matrix3 rotationMatrix=localToWorldCoord.Extract3x3Matrix();
    inverseInertiaTensorWorld = (rotationMatrix * inverseInertiaTensor) * rotationMatrix.Transpose();
}

void RigidBody::SetMass(float value)
{
    massInverse = 1.0f / value;
}

void RigidBody::SetInverseMass(float value)
{
    massInverse = value;
}

void RigidBody::SetInertiaTensor(const Matrix3& mat)
{
    inverseInertiaTensor = mat.Inverse();
    TransformInertiaTensor();
}

void RigidBody::SetInverseInertiaTensor(const Matrix3& mat)
{
    inverseInertiaTensor = mat;
    TransformInertiaTensor();
}

void RigidBody::SetPosition(const Vector3& vec)
{
    position = vec;
    UpdateTransformMatrix();
}

void RigidBody::SetPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
    UpdateTransformMatrix();
}

void RigidBody::SetOrientation(const Quaternion& quat)
{
    orientation = quat;
    UpdateTransformMatrix();
    TransformInertiaTensor();
}

void RigidBody::SetLinearVelocity(const Vector3& vec)
{
    velocity = vec;
}

void RigidBody::SetLinearVelocity(float x, float y, float z)
{
    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
}

void RigidBody::SetAngularVelocity(const Vector3& vec){
    angularVelocity = localToWorldCoord.Extract3x3Matrix() * vec;
}

void RigidBody::SetAngularVelocity(float x, float y, float z){
    angularVelocity = localToWorldCoord.Extract3x3Matrix() * Vector3(x, y, z);
}

void RigidBody::SetLinearAcceleration(const Vector3& vec){
    linearAcceleration = vec;
}

void RigidBody::SetLinearAcceleration(float x, float y, float z){
    linearAcceleration.x = x;
    linearAcceleration.y = y;
    linearAcceleration.z = z;
}

void RigidBody::SetLinearDamping(float value){
    linearDamping = value;
}

float RigidBody::GetMass() const{
    return 1.0f / massInverse;
}

float RigidBody::GetInverseMass() const{
    return massInverse;
}

Matrix3 RigidBody::GetInverseInertiaTensor() const{
    return inverseInertiaTensor;
}

Matrix3 RigidBody::GetInverseInertiaTensorWorld() const{
    return inverseInertiaTensorWorld;
}

Vector3 RigidBody::GetPosition() const{
    return position;
}

Vector3 RigidBody::GetLinearVelocity() const{
    return velocity;
}

Vector3 RigidBody::GetAngularVelocity() const{
    return localToWorldCoord.Extract3x3Matrix().Transpose() * angularVelocity;
}

Vector3 RigidBody::GetAcceleration() const{
    return linearAcceleration;
}

float RigidBody::GetLinearDamping() const{
    return linearDamping;
}

Matrix4 RigidBody::GetLocalToWorldMatrix() const{
    return localToWorldCoord;
}
