#include "collider.h"
#include <cstdarg>

using namespace physics;

SphereCollider::SphereCollider(RigidBody* _body, float _radius)
{
	rigidBody = _body;
	radius = _radius;
}

void SphereCollider::SetScale(double value, ...)
{
	radius = value;
}

BoxCollider::BoxCollider(RigidBody* _body, float _halfX, float _halfY, float _halfZ)
{
	rigidBody = _body;
	extents.x = _halfX;
	extents.y = _halfY;
	extents.z = _halfZ;
}

void BoxCollider::SetScale(double value, ...)
{
	extents.x = value;

	va_list args;
	va_start(args, value);

	extents.y = va_arg(args, double);
	extents.z = va_arg(args, double);

	va_end(args);
}

Vector3 physics::BoxCollider::GetLocalContactVertex(Vector3 collisionNormal, std::function<bool(float, float)> cmp) const {
	Vector3 contactPoint{  extents.x,  extents.y,  extents.z };

	if (cmp( rigidBody->GetAxis(0).Dot(collisionNormal), 0)) {
		contactPoint.x *= -1.0f;
	}
	if (cmp( rigidBody->GetAxis(1).Dot(collisionNormal), 0)) {
		contactPoint.y *= -1.0f;
	}
	if (cmp( rigidBody->GetAxis(2).Dot(collisionNormal), 0)) {
		contactPoint.z *= -1.0f;
	}
	return contactPoint;
}

Plane::Plane(Vector3 _normal, float _offset)
{
	normal = _normal;
	distance = _offset;
}
