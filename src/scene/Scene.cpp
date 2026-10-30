#include "scene/Scene.hpp"
#include "scene/SceneFactory.hpp"
#include "graphics/Primitives.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Shader.hpp"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

Scene::Scene()
    : cube_(Primitives::cube()), plane_(Primitives::plane()),
      cylinder_(Primitives::cylinder()), sphere_(Primitives::sphere()),
      screen_(Primitives::screenQuad()),
      wood_(std::make_unique<Texture>("assets/textures/wood.png")),
      floor_(std::make_unique<Texture>("assets/textures/floor.png")) {
    SceneAssets assets{cube_.get(), plane_.get(), cylinder_.get(), sphere_.get(),
                       screen_.get(), wood_.get(), floor_.get()};
    SceneFactory::buildRoom(root_, assets);
    SceneNode& desk = SceneFactory::buildDesk(root_, assets);
    leftMonitor_ = desk.findByName("Left monitor");
    rightMonitor_ = desk.findByName("Right monitor");
    if (!leftMonitor_ || !rightMonitor_)
        throw std::runtime_error("Monitor nodes missing from desk hierarchy");
    bulb_ = &SceneFactory::buildLamp(desk, assets);
    SceneFactory::buildChair(root_, assets);
}
Scene::~Scene() = default;
void Scene::draw(const Shader& shader, RenderPass pass) const {
    root_.draw(shader, glm::mat4(1.0f), pass);
}
void Scene::toggleLamp() {
    if (wow_.active()) return;
    lampOn_ = !lampOn_;
    bulb_->emission = glm::vec3(0.90f, 0.65f, 0.30f) * lampBrightness();
}
glm::vec3 Scene::lampPosition() const { return bulb_->worldPosition(); }

void Scene::setMonitorLayout(int layout) {
    monitorLayout_ = std::clamp(layout, 0, 2);
    // Keep each monitor and its stand as a single rigid transform group.
    struct Pose { float x, z, yaw; };
    const Pose left[3] = {{-0.86f, -0.47f, 0.0f},
                          {-0.86f, -0.47f, 18.0f},
                          {-1.05f, -0.47f, 4.0f}};
    const Pose right[3] = {{0.86f, -0.47f, 0.0f},
                           {0.86f, -0.47f, -18.0f},
                           {1.05f, -0.47f, -4.0f}};
    auto transform = [](SceneNode& node, const Pose& pose) {
        node.localTransform = glm::translate(glm::mat4(1.0f),
                                              {pose.x, 0.0f, pose.z}) *
            glm::rotate(glm::mat4(1.0f), glm::radians(pose.yaw),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    };
    transform(*leftMonitor_, left[monitorLayout_]);
    transform(*rightMonitor_, right[monitorLayout_]);
}
void Scene::cycleMonitorLayout() { setMonitorLayout((monitorLayout_ + 1) % 3); }
void Scene::setRgbMode(int mode) {
    if (!wow_.active()) rgbMode_ = std::clamp(mode, 0, 2);
}
void Scene::cycleRgbMode() { setRgbMode((rgbMode_ + 1) % 3); }
void Scene::setRgbSpeed(float speed) {
    if (!wow_.active()) rgbSpeed_ = std::clamp(speed, 0.1f, 5.0f);
}
void Scene::setRgbColor(const glm::vec3& color) {
    if (wow_.active()) return;
    rgbColor_ = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
}
glm::vec3 Scene::rgbLightColor(float time) const {
    if (rgbMode() == 2) return glm::vec3(0.0f);
    if (rgbMode() == 1) return rgbColor_;
    // Same three-channel cosine palette and phase speed as rgb.frag.
    constexpr float tau = 6.28318530718f;
    const float phase = time * rgbSpeed() * 0.16f;
    return glm::vec3(0.5f + 0.5f * std::cos(tau * phase),
                     0.5f + 0.5f * std::cos(tau * (phase + 1.0f/3.0f)),
                     0.5f + 0.5f * std::cos(tau * (phase + 2.0f/3.0f)));
}

void Scene::setDayFactor(float factor) {
    if (wow_.active()) return;
    dayFactor_ = std::clamp(factor, 0.0f, 1.0f);
}
void Scene::toggleDayNight() {
    setDayFactor(dayFactor_ >= 0.5f ? 0.0f : 1.0f);
}

void Scene::toggleFinale() {
    if (!wow_.active()) {
        // The controller saves values, while original UI settings remain intact.
        wow_.start(dayFactor_, lampOn_ ? 1.0f : 0.0f);
    } else {
        wow_.restore(); // Also works during the initial fade.
    }
}
void Scene::update(float deltaTime) {
    wow_.update(deltaTime);
    // Only a material uniform changes; static vertex buffers never re-upload.
    bulb_->emission = glm::vec3(0.90f, 0.65f, 0.30f) * lampBrightness();
}
