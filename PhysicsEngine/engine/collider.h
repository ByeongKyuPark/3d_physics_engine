#pragma once
#include "body.h"
#include "engine/contact.h"
#include <functional>//std::function
#include <vector>

namespace physics
{
	class Constraint {
		friend class CollisionManager;
	public:
		virtual ~Constraint() {}
	};

	class Plane : public Constraint
	{
		friend class CollisionManager;
	public:
		~Plane() override {}

	protected:
		Vector3 normal;
		float distance;

	public:
		Plane(Vector3 normal, float offset);
	};

	class Collider
	{
	public:
		friend class CollisionManager;
		friend class PhysicsWorld;

	protected:
		RigidBody* rigidBody;

	public:
		virtual void SetScale(double, ...) = 0;
	};

	class SphereCollider : public Collider
	{
		friend class CollisionManager;
		friend class PhysicsWorld;

	protected:
		float radius;

	public:
		SphereCollider(RigidBody* _body, float _radius);
		void SetScale(double, ...);

	};

	class BoxCollider : public Collider //OBB
	{
		friend class CollisionManager;
		friend class PhysicsWorld;

	protected:
		Vector3 extents;

	public:
		BoxCollider(RigidBody* rigidBody, float extentsX, float extentsY, float extentsZ);
		void SetScale(double, ...);

		Vector3 GetLocalContactVertex(Vector3 collisionNormal, std::function<bool(float, float)>cmp) const;
	};
}

