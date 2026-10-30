#include "graphics/Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

Camera::Camera() { reset(); }
void Camera::reset() {
    yaw_ = glm::radians(42.0f);
    pitch_ = glm::radians(22.0f);
    distance_ = 7.5f;
    target_ = {0.0f, 1.0f, 0.0f};
}
void Camera::orbit(float deltaX, float deltaY) {
    yaw_ += glm::radians(deltaX);
    pitch_ = std::clamp(pitch_ + glm::radians(deltaY),
                        glm::radians(-3.0f), glm::radians(85.0f));
}
void Camera::zoom(float wheelOffset) {
    distance_ = std::clamp(distance_ - wheelOffset * 0.5f, 2.0f, 15.0f);
}
glm::vec3 Camera::position() const {
    const float c = std::cos(pitch_);
    return target_ + distance_ * glm::vec3(
        c * std::sin(yaw_), std::sin(pitch_), c * std::cos(yaw_));
}
glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(position(), target_, glm::vec3(0.0f, 1.0f, 0.0f));
}
glm::mat4 Camera::projectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(fov_), aspect, 0.1f, 100.0f);
}
