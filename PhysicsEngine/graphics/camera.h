#pragma once

#include "GLFW/glfw3.h"
#include "opengl/glm/glm.hpp"

namespace graphics
{
    class Camera
    {
        glm::vec3 viewVector;    // direction the camera is looking
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;

        float fov;
        float panSensitivity;
        float rotateSensitivity;

    public:
        Camera();

        glm::mat4 GetViewMatrix() const;
        glm::vec3 GetViewPlaneNormal() const;

        void Pan(float xOffset, float yOffset, float zOffset);
        void Rotate(glm::vec3 axis, float angle);
        void Zoom(float yOffset);

        glm::vec3 GetPosition() const;
        float GetFov() const;

    private:
        void UpdateCameraVectors();
    };    
}