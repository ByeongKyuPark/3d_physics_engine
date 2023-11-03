#pragma once
#include <memory>//unique_ptr
#include <vector>
#include "graphics/camera.h"

using graphics::Camera;
class CameraManager {
private:
    std::vector<std::unique_ptr<Camera>> cameras;  //why pointer? 
                                                   // dynamic management, add/remove cameras at runtime
    Camera* currentCamera{ nullptr };

public:
    CameraManager();

    Camera* GetCurrentCamera() const;

    void AddCamera(std::unique_ptr<Camera> cam);
    void SetCurrentCamera(int index);
    void MoveCamera(glm::vec3 offset);
    void RotateCamera(glm::vec3 axis, float angle);
    void ZoomCamera(float degree);

    float GetFov()const;

    glm::mat4 GetViewMatrix() const;
    glm::vec3 GetCameraPosition() const;
};
