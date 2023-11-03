#include "cameraManager.h"
#include <stdexcept>

CameraManager::CameraManager() {
    cameras.push_back(std::make_unique<Camera>());
    currentCamera = cameras[0].get();
}

Camera* CameraManager::GetCurrentCamera() const {
    return currentCamera;
}

void CameraManager::AddCamera(std::unique_ptr<Camera> cam) {
    cameras.push_back(std::move(cam));
}

void CameraManager::SetCurrentCamera(int index) {
    if (index >= cameras.size()) {
        throw std::runtime_error("CameraManager::setCurrentCamera, index out of range\n");
    }
    currentCamera = cameras[index].get();
}

void CameraManager::MoveCamera(glm::vec3 offset) {
    currentCamera->Pan(offset.x, offset.y, offset.z);
}

void CameraManager::RotateCamera(glm::vec3 axis, float angle) {
    currentCamera->Rotate(axis, angle);
}

void CameraManager::ZoomCamera(float degree) {
    currentCamera->Zoom(degree);
}

float CameraManager::GetFov() const { return currentCamera->GetFov(); }

glm::mat4 CameraManager::GetViewMatrix() const {
    return currentCamera->GetViewMatrix(); 
}

glm::vec3 CameraManager::GetCameraPosition() const {
    return currentCamera->GetPosition();
}

