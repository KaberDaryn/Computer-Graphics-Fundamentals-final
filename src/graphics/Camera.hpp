#pragma once
#include <glm/glm.hpp>

class Camera final {
public:
    Camera();
    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix(float aspect) const;
    glm::vec3 position() const;
    void orbit(float deltaX, float deltaY);
    void zoom(float wheelOffset);
    void reset();

private:
    glm::vec3 target_{0.0f, 1.0f, 0.0f};
    float yaw_ = 0.0f;
    float pitch_ = 0.0f;
    float distance_ = 7.5f;
    float fov_ = 45.0f;
};
