#include "camera.h"
#include <opengl/glm/gtc/quaternion.hpp>

using namespace graphics;

Camera::Camera()
{
    position = glm::vec3(-50.0f, 18.0f, 80.0f);
    target = glm::vec3(.0f, 15.0f, 0.f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    fov = 45.0f;
    panSensitivity = 0.035f;
    rotateSensitivity = 1.3f;
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const{
    return glm::lookAtRH(position, target, up);//OpenGL <- Right Handed
}

glm::vec3 Camera::GetViewPlaneNormal() const{
    glm::vec3 viewPlaneNormal = glm::cross(up, right);
    return glm::normalize(viewPlaneNormal);
}

void Camera::Pan(float xOffset, float yOffset, float zOffset){
    position += right * xOffset * panSensitivity;
    target += right * xOffset * panSensitivity;

    position += up * yOffset * panSensitivity;
    target += up * yOffset * panSensitivity;

    position += viewVector * zOffset * panSensitivity;
    target += viewVector * zOffset * panSensitivity;
}

void Camera::Rotate(glm::vec3 axis, float angle){
    angle *= rotateSensitivity;

    glm::quat rotateQuat(
        glm::cos(angle / 2),
        axis.x * glm::sin(angle / 2),
        axis.y * glm::sin(angle / 2),
        axis.z * glm::sin(angle / 2)
    );
    glm::mat4 rotateMatrix = glm::mat4_cast(rotateQuat);
    position = glm::mat3(rotateMatrix) * position;

    UpdateCameraVectors();
}

void Camera::Zoom(float yOffset){
    fov -= yOffset;
    if (fov < 1.0f) {
        fov = 1.0f;
    }
    else if (fov > 90.0f) {
        fov = 90.0f;
    }
}

glm::vec3 Camera::GetPosition() const{
    return position;
}

float Camera::GetFov() const{
    return fov;
}

void Camera::UpdateCameraVectors(){
    // Recalculate the target, right, and up vectors based on the new camera position.
    //target = glm::normalize(-position); // The target vector points towards the origin (0, 0, 0).
    viewVector = glm::normalize(target - position); // direction the camera is looking
    right = glm::normalize(glm::cross(viewVector, worldUp));
    up = glm::normalize(glm::cross(right, viewVector));
}
