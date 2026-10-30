#include "scene/SceneFactory.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace SceneFactory {
SceneNode& group(SceneNode& parent, const std::string& name,
                 const glm::vec3& pos, float rotationZ) {
    auto node = std::make_unique<SceneNode>(name);
    node->localTransform = glm::translate(glm::mat4(1.0f), pos) *
                           glm::rotate(glm::mat4(1.0f),
                                       glm::radians(rotationZ), glm::vec3(0, 0, 1));
    return parent.addChild(std::move(node));
}
SceneNode& part(SceneNode& parent, const std::string& name, Mesh* mesh,
                const glm::vec3& pos, const glm::vec3& scale,
                const glm::vec3& color, Texture* texture,
                glm::vec2 uvScale, float shininess) {
    auto node = std::make_unique<SceneNode>(name);
    node->mesh = mesh;
    node->texture = texture;
    node->color = color;
    node->uvScale = uvScale;
    node->shininess = shininess;
    node->localTransform = glm::translate(glm::mat4(1.0f), pos) *
                           glm::scale(glm::mat4(1.0f), scale);
    return parent.addChild(std::move(node));
}
}
