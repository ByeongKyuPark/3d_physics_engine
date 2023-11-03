#pragma once

#include "graphics/renderer.h"
#include "gui/gui.h"
#include "object.h"
#include "event.h"
#include "graphics/textureImage.h"
#include "engine/physicsWorld.h"
#include "cameraManager.h"
#include <memory>//unique_ptr
#include <unordered_map>
#include <vector>
#include <queue>

class SphereBoxSpawner;

class Simulator
{
public:
    bool isRunning;
    bool shouldRenderContactInfo;
private:
    CameraManager cameraManager;
    physics::PhysicsWorld physicsWorld;
    graphics::Renderer renderer;
    gui::GUI userInterface;

    std::vector<RigidObject*> selectedObjects;
    std::queue<std::unique_ptr<Event>> eventQueue;

    float timeStepMultiplier;

public:
    Simulator();
    
    void Run();
    SphereObject* AddSphere(math::Vector3 pos = {0.f,1.f,0.f}, TextureID img = TextureID::FACE);
    BoxObject* AddBox(math::Vector3 pos = { 0.f,1.f,0.f }, TextureID img=TextureID::BALOONS);
    SphereBoxSpawner* AddSpawner(math::Vector3 pos = { 0.f,1.f,0.f }, TextureID img = TextureID::SPAWNER);
    std::vector<RigidObject*>::iterator RemoveObject(RigidObject* obj);

    void SetTimeStepMultiplier(float value) { timeStepMultiplier=value; }
    
    CameraManager& GetCameraManager() { return cameraManager; }
    graphics::Renderer& GetRenderer() { return renderer; }
    physics::PhysicsWorld& GetSimulator() { return physicsWorld; }
    std::queue<std::unique_ptr<Event>>& GetEventQueue() { return eventQueue; }
    std::vector<RigidObject*>& GetSelectedObjects() { return selectedObjects; }
    
    void HandleKeyboardInput();
    void ClearSelectedObjectIDs();

    static constexpr double Target_FPS = 60.0;
    static constexpr double Target_FPS_Inverse = 1 / 60.0;
};

