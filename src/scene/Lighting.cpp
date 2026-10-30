#include "scene/Lighting.hpp"
#include "scene/Scene.hpp"
#include "graphics/Shader.hpp"
#include <glm/common.hpp>

namespace Lighting {
void upload(const Shader& shader, const Scene& scene, float time) {
    // Daylight changes the room's ambient level and window contribution.
    // RGB and desk lamp remain independent, so the night scene is interactive.
    const float day = scene.dayFactor();
    const float room = scene.roomLightMultiplier();
    shader.setVec3("uAmbientColor", room * glm::mix(
        glm::vec3(0.009f, 0.013f, 0.025f),
        glm::vec3(0.105f, 0.125f, 0.165f), day));
    shader.setVec3("uLights[0].position", {-1.0f, 4.0f, 2.5f});
    shader.setVec3("uLights[0].color", {0.76f, 0.86f, 1.0f});
    shader.setFloat("uLights[0].intensity", room * (0.05f + day * 0.79f));
    shader.setFloat("uLights[0].linear", 0.07f);
    shader.setFloat("uLights[0].quadratic", 0.018f);

    shader.setVec3("uLights[1].position", {2.75f, 3.05f, -2.75f});
    shader.setVec3("uLights[1].color", glm::mix(
        glm::vec3(0.24f, 0.37f, 0.70f),
        glm::vec3(0.98f, 0.96f, 0.88f), day));
    shader.setFloat("uLights[1].intensity", room * (0.10f + 0.73f * day));
    shader.setFloat("uLights[1].linear", 0.11f);
    shader.setFloat("uLights[1].quadratic", 0.042f);

    // Same world-space location as bulb mesh (computed from the scene graph).
    shader.setVec3("uLights[2].position", scene.lampPosition());
    shader.setVec3("uLights[2].color", {1.0f, 0.76f, 0.43f});
    shader.setFloat("uLights[2].intensity", scene.lampBrightness() * 2.50f);
    shader.setFloat("uLights[2].linear", 0.34f);
    shader.setFloat("uLights[2].quadratic", 0.22f);

    // A nearby, low-cost colored point light gives the RGB strip a visible
    // spill on surrounding surfaces. Its palette matches the RGB GLSL shader.
    shader.setVec3("uLights[3].position", {0.0f, 1.22f, -0.77f});
    shader.setVec3("uLights[3].color", scene.rgbLightColor(time));
    shader.setFloat("uLights[3].intensity",
                    scene.rgbMode() == 2 ? 0.0f :
                        (1.15f + (1.0f - day) * 1.30f) * scene.rgbBoost());
    shader.setFloat("uLights[3].linear", 0.22f);
    shader.setFloat("uLights[3].quadratic", 0.35f);
}
}
