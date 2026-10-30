#pragma once
#include "scene/SceneNode.hpp"
#include <glm/glm.hpp>
#include <string>

class Mesh;
class Texture;

struct SceneAssets {
    Mesh* cube;
    Mesh* plane;
    Mesh* cylinder;
    Mesh* sphere;
    Mesh* screen;
    Texture* wood;
    Texture* floor;
};

namespace SceneFactory {
SceneNode& group(SceneNode& parent, const std::string& name,
                 const glm::vec3& pos, float rotationZ = 0.0f);
SceneNode& part(SceneNode& parent, const std::string& name, Mesh* mesh,
                const glm::vec3& pos, const glm::vec3& scale,
                const glm::vec3& color, Texture* texture = nullptr,
                glm::vec2 uvScale = glm::vec2(1.0f),
                float shininess = 48.0f);
void buildRoom(SceneNode& root, const SceneAssets& assets);
SceneNode& buildDesk(SceneNode& root, const SceneAssets& assets);
SceneNode& buildLamp(SceneNode& desk, const SceneAssets& assets);
void buildChair(SceneNode& root, const SceneAssets& assets);
}
