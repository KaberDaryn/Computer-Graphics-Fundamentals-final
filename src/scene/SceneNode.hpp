#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

class Mesh;
class Texture;
class Shader;

enum class RenderPass { Lit, RGB, Sky, Screen };

// Scene graph node: ownership follows the tree; mesh and texture references do not.
class SceneNode final {
public:
    explicit SceneNode(std::string name);
    SceneNode& addChild(std::unique_ptr<SceneNode> child);
    SceneNode* findByName(const std::string& name);
    void draw(const Shader& shader, const glm::mat4& parent, RenderPass pass) const;
    glm::mat4 worldTransform() const;
    glm::vec3 worldPosition() const;
    const std::string& name() const { return name_; }

    glm::mat4 localTransform{1.0f};
    glm::vec3 color{1.0f};
    glm::vec3 emission{0.0f};
    glm::vec2 uvScale{1.0f};
    float shininess = 48.0f;
    RenderPass renderPass = RenderPass::Lit;
    float rgbPhase = 0.0f;
    Mesh* mesh = nullptr;
    Texture* texture = nullptr;
private:
    std::string name_;
    SceneNode* parent_ = nullptr;
    std::vector<std::unique_ptr<SceneNode>> children_;
};
