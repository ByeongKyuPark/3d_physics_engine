#pragma once

#include "engine/body.h"
#include "engine/collider.h"
#include "graphics/shape.h"
#include "geometry.h"
#include <glm/glm.hpp>

class PhysicsSystem;

class RigidObject
{
public:
    friend class Simulator;

protected:
    physics::RigidBody* rigidBody; // 1. Physical entities without a 'shape' (corresponds to points)
    physics::Collider* collider;   // 2. Additional physics properties corresponding to 'shapes'
    graphics::Shape* shape;        // 3. Graphical elements

    ObjectType type;

    bool isSelected;
    bool IsFixed;

    virtual void SynchObjectData() = 0;
public:
    RigidObject() : rigidBody{}, collider{}, shape{}, type{},isSelected(false), IsFixed(false) {}
    virtual ~RigidObject() {};

    virtual ObjectType GetObjectType() const = 0;
    physics::RigidBody* GetRigidBody() { return rigidBody; }
    const physics::RigidBody* GetRigidBody()const { return rigidBody; }
    physics::Collider* GetCollider() { return collider; }

    bool GetIsSelected() const { return isSelected; }
    bool GetIsFixed() const { return IsFixed; }
    
    math::Vector3 GetPosition() const;
    math::Vector3 GetLinearVelocity() const;
    math::Vector3 GetAngularVelocity() const;
    
    math::Vector3 GetLinearAcceleration() const;
    float GetMass() const;
    virtual float GetHeight()const = 0;
    virtual math::Vector3 GetScale() const = 0;

    graphics::Shape* GetShape() { return shape; }
    const graphics::Shape* GetShape() const { return shape; }

    void SetFixed(bool isFixed_) { IsFixed = isFixed_; }
    void SetSelected(bool isSelected_) { isSelected = isSelected_; }
    void SetRigidBody(physics::RigidBody* rb) { rigidBody = rb; }
    void SetCollider(physics::Collider* col) { collider = col; }
    void SetShape(graphics::Shape* Shape) { shape = Shape; }
    void FreePhysicsComponents();

    virtual void SetScale(float) = 0;
    virtual void SetScale(math::Vector3) = 0;
};

class SphereObject : public RigidObject
{
    virtual void SynchObjectData() override final;
protected:
    float radius;

public:
    SphereObject() : radius(1.0f) {}

    ObjectType GetObjectType() const override final { return ObjectType::SPHERE; }
    math::Vector3 GetScale() const override final;
    void SetScale(float) override final;
    virtual void SetScale(math::Vector3) override final;
    float GetHeight()const override final { return radius; }
};

class BoxObject : public RigidObject
{
    virtual void SynchObjectData() override final;
protected:
    float extentsX;
    float extentsY;
    float extentsZ;

public:
    BoxObject() : extentsX(0.5f), extentsY(0.5f), extentsZ(0.5f) {}

    ObjectType GetObjectType() const override { return ObjectType::BOX; }
    math::Vector3 GetScale() const override final;
    void SetScale(float) override final;
    void SetScale(math::Vector3) override final;
    float GetHeight()const override final { return extentsY; }
};

