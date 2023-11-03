#pragma once

#include "geometry.h"
#include "object.h"

class Simulator;
/* 
Why Use an Event System?
    1. Responsive GUI
        : The user interface becomes more responsive, as it doesn't have to wait for the actual processing of the event. It only triggers the event and moves on.
    2. Consistent Calculations
        : Deferring event processing ensures that game calculations remain consistent. For example, updating object positions in the middle of a physics calculation could lead to unexpected results.
    3. Potential for Performance Optimization
        : By queuing events and processing them in batches, we can optimize certain operations. For instance, if there are 10 events to add objects, we can handle them simultaneously rather than processing each one individually. (Note: This optimization needs further development.)
*/

struct Event
{
public:
    Event() {}
    virtual void Handle(Simulator& simulator) = 0;
    virtual ~Event() {}
};
struct DuplicateEvent : public Event {
    virtual void Handle(Simulator& simulator) override final;
};
struct SaveScenarioEvent : public Event {
    static int savedScenarios;
    virtual void Handle(Simulator& simulator) override final;
};

struct PresetLoadEvent : public Event {
    int presetIdx;
    PresetLoadEvent(int presetIdx_) :presetIdx{ presetIdx_ } {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ObjectAddEvent : public Event
{
public:
    ObjectType geometry;

    ObjectAddEvent(ObjectType _geometry)
        : geometry(_geometry) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct DeselectObjectsEvent : public Event {
    virtual void Handle(Simulator& simulator) override final;
};
struct ObjectSelectEvent : public Event
{
public:
    RigidObject* obj;
    unsigned int id;
    bool isCtrlPressed;

    ObjectSelectEvent(RigidObject* Obj, bool ctrl)
        : obj(Obj), isCtrlPressed(ctrl) {}
    ObjectSelectEvent(unsigned int _id, bool ctrl)
        : id(_id), isCtrlPressed(ctrl) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ObjectRemoveEvent : public Event {
    virtual void Handle(Simulator& simulator) override final;
};

struct SimulationToggleEvent : public Event {
    virtual void Handle(Simulator& simulator) override final;
};

struct ObjectPositionEvent : public Event
{
public:
    RigidObject* obj;
    math::Vector3 position;

    ObjectPositionEvent(RigidObject* _Obj, math::Vector3 pos)
        :obj{ _Obj }, position{pos}
    {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ObjectVelocityEvent : public Event
{
public:
    RigidObject* obj;
    math::Vector3 velocity;

    ObjectVelocityEvent(RigidObject* _Obj, math::Vector3 vel)
        :obj{ _Obj }, velocity{vel}
    {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ObjectScaleEvent : public Event
{
public:
    RigidObject* obj;
    math::Vector3 GRID_SCALE;

    ObjectScaleEvent(RigidObject* _Obj, math::Vector3 scl)
        :obj{ _Obj }, GRID_SCALE{scl}
    {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ObjectMassEvent : public Event
{
public:
    RigidObject* obj;
    float value;

    ObjectMassEvent(RigidObject* _Obj, float _value)
        : obj(_Obj), value(_value) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct LeftMouseDragEvent : public Event
{
public:
    float xOffset, yOffset;

    LeftMouseDragEvent(float _xOffset, float _yOffset)
        : xOffset(_xOffset), yOffset(_yOffset) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct RightMouseDragEvent : public Event
{
public:
    float curX, curY;
    float prevX, prevY;

    RightMouseDragEvent(float _curX, float _curY, float _prevX, float _prevY)
        : curX(_curX), curY(_curY), prevX(_prevX), prevY(_prevY) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct MouseWheelEvent : public Event
{
public:
    float value;

    MouseWheelEvent(float _value)
        : value(_value) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct LeftMouseClickEvent : public Event
{
public:
    float screenX, screenY;
    bool isCtrlPressed;

    LeftMouseClickEvent(float _x, float _y, bool _isCtrlPressed)
        : screenX(_x), screenY(_y), isCtrlPressed(_isCtrlPressed) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ObjectFixPositionEvent : public Event
{
public:
    RigidObject* obj;
    bool shouldBeFixed;

    ObjectFixPositionEvent(RigidObject* Obj, bool ShouldBeFixed)
        : obj{Obj}, shouldBeFixed(ShouldBeFixed) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ToggleContactRenderEvent : public Event
{
public:
    bool flag;

    ToggleContactRenderEvent(bool _flag)
        : flag(_flag) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ClearAllObjectsEvent : public Event {
    virtual void Handle(Simulator& simulator) override final;
};

struct GroundRestitutionEvent : public Event
{
public:
    float value;

    GroundRestitutionEvent(float _value)
        : value(_value) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ObjectRestitutionEvent : public Event
{
public:
    float value;

    ObjectRestitutionEvent(float _value)
        : value(_value) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct GravityEvent : public Event
{
public:
    float value;

    GravityEvent(float _value)
        : value(_value) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ObjectRotateEvent : public Event
{
public:
    RigidObject* obj;
    float axisX;
    float axisY;
    float axisZ;
    float degree;

    ObjectRotateEvent(RigidObject* _Obj, float x, float y, float z, float _degree)
        : obj(_Obj), axisX(x), axisY(y), axisZ(z), degree(_degree) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct OrientationResetEvent : public Event
{
public:
    RigidObject* obj;

    OrientationResetEvent(RigidObject* _Obj)
        : obj(_Obj) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct ToggleWorldAxisRenderEvent : public Event
{
public:
    RigidObject* obj;
    int axisIdx;

    ToggleWorldAxisRenderEvent(RigidObject* _Obj, int _axisIdx)
        : obj(_Obj), axisIdx(_axisIdx) {}
    virtual void Handle(Simulator& simulator) override final;
};

struct RemoveFloatingObjectsEvent : public Event {
    virtual void Handle(Simulator& simulator) override final;
};

struct TimeStepChangeEvent : public Event
{
public:
    float value;

    TimeStepChangeEvent(float _value)
        : value(_value) {}
    virtual void Handle(Simulator& simulator) override final;
};