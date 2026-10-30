#include "scene/SceneFactory.hpp"
#include "graphics/Mesh.hpp"
#include <cmath>

namespace SceneFactory {
SceneNode& buildDesk(SceneNode& root, const SceneAssets& a) {
    auto& desk = group(root, "Desk assembly", {0, 0, 0});
    part(desk, "Textured wooden tabletop", a.cube, {0, 1.05f, 0},
         {3.75f, 0.14f, 1.75f}, {0.88f, 0.78f, 0.69f}, a.wood, {2, 1}, 22);
    for (float x : {-1.68f, 1.68f}) for (float z : {-0.70f, 0.70f}) {
        part(desk, "Steel desk leg", a.cube, {x, 0.49f, z},
             {0.105f, 0.98f, 0.105f}, {0.09f, 0.14f, 0.20f}, nullptr, {1, 1}, 74);
    }
    part(desk, "Cable tray", a.cube, {0, 0.93f, -0.65f},
         {2.30f, 0.07f, 0.22f}, {0.07f, 0.10f, 0.15f});

    // Two monitors; every monitor's screen follows its frame and stand.
    for (int m = 0; m < 2; ++m) {
        const float x = (m == 0) ? -0.86f : 0.86f;
        auto& monitor = group(desk, m == 0 ? "Left monitor" : "Right monitor",
                              {x, 0, -0.47f});
        part(monitor, "Stand base", a.cube, {0, 1.15f, 0.07f},
             {0.52f, 0.043f, 0.35f}, {0.10f, 0.13f, 0.19f}, nullptr, {1, 1}, 95);
        part(monitor, "Stand pole", a.cylinder, {0, 1.43f, 0},
             {0.085f, 0.55f, 0.085f}, {0.12f, 0.15f, 0.20f});
        part(monitor, "Screen housing", a.cube, {0, 1.89f, 0},
             {1.53f, 0.94f, 0.095f}, {0.045f, 0.058f, 0.080f}, nullptr, {1, 1}, 72);
        auto& display = part(monitor, "Screen display", a.cube, {0, 1.89f, 0.057f},
             {1.39f, 0.80f, 0.015f},
             m == 0 ? glm::vec3(0.09f, 0.29f, 0.42f)
                    : glm::vec3(0.11f, 0.19f, 0.34f));
        display.emission = m == 0 ? glm::vec3(0.024f, 0.09f, 0.15f)
                                  : glm::vec3(0.025f, 0.05f, 0.10f);
        if (m == 1) {
            // This single quad sits a few millimeters in front of the dark
            // screen backing; draw it only in the final, on-screen pass.
            // We do not draw it to the offscreen camera: no feedback loop.
            auto& feed = part(monitor, "Live second-camera screen", a.screen,
                              {0, 1.89f, 0.070f}, {1.34f, 0.75f, 1.0f},
                              {1.0f, 1.0f, 1.0f});
            feed.renderPass = RenderPass::Screen;
        }
        part(monitor, "Camera dot", a.sphere, {0, 2.33f, 0.063f},
             {0.035f, 0.035f, 0.025f}, {0.18f, 0.46f, 0.65f});
    }

    auto& pad = part(desk, "Desk pad", a.cube, {-0.12f, 1.128f, 0.40f},
                     {2.25f, 0.012f, 0.77f}, {0.09f, 0.15f, 0.23f}, nullptr, {1, 1}, 8);
    pad.shininess = 8.0f;
    auto& keyboard = group(desk, "Mechanical keyboard", {-0.30f, 0, 0.46f});
    part(keyboard, "Keyboard body", a.cube, {0, 1.174f, 0},
         {1.36f, 0.065f, 0.48f}, {0.045f, 0.07f, 0.105f});
    for (int row = 0; row < 4; ++row) for (int column = 0; column < 12; ++column) {
        float x = -0.593f + float(column) * 0.107f;
        float z = -0.175f + float(row) * 0.115f;
        // Dark keycaps with separate, shader-animated RGB luminous top inserts.
        auto& key = part(keyboard, "Keycap", a.cube, {x, 1.233f, z},
                         {0.087f, 0.034f, 0.088f},
                         {0.08f, 0.11f, 0.15f});
        key.shininess = 75.0f;
        auto& accent = part(keyboard, "Shader animated RGB key", a.cube,
                            {x, 1.260f, z}, {0.073f, 0.012f, 0.073f},
                            {1.0f, 1.0f, 1.0f});
        accent.renderPass = RenderPass::RGB;
        accent.rgbPhase = float(column) * 0.065f + float(row) * 0.025f;
    }
    part(desk, "Mouse", a.sphere, {1.21f, 1.19f, 0.47f},
         {0.23f, 0.11f, 0.34f}, {0.12f, 0.16f, 0.23f}, nullptr, {1, 1}, 83);
    part(desk, "Mouse wheel", a.cube, {1.21f, 1.247f, 0.395f},
         {0.031f, 0.017f, 0.06f}, {0.23f, 0.47f, 0.56f});

    auto& plant = group(desk, "Succulent plant", {-1.52f, 0, -0.40f});
    part(plant, "Clay pot", a.cylinder, {0, 1.25f, 0},
         {0.35f, 0.25f, 0.35f}, {0.60f, 0.33f, 0.23f}, nullptr, {1, 1}, 24);
    part(plant, "Pot soil", a.cylinder, {0, 1.385f, 0},
         {0.29f, 0.018f, 0.29f}, {0.18f, 0.15f, 0.12f});
    part(plant, "Stem", a.cylinder, {0, 1.52f, 0},
         {0.065f, 0.28f, 0.065f}, {0.12f, 0.34f, 0.17f});
    for (int n = 0; n < 7; ++n) {
        float t = float(n) * 6.28318530718f / 7.0f;
        part(plant, "Leaves", a.sphere,
             {0.14f * std::cos(t), 1.66f + 0.10f * float(n % 3),
              0.13f * std::sin(t)}, {0.21f, 0.30f, 0.18f},
             {0.10f, 0.34f + 0.025f * float(n % 3), 0.19f});
    }

    part(desk, "LED strip housing", a.cube, {0, 1.148f, -0.84f},
         {3.50f, 0.035f, 0.035f}, {0.12f, 0.20f, 0.28f});
    auto& rgbStrip = part(desk, "Shader animated LED strip", a.cube,
                          {0, 1.165f, -0.84f},
                          {3.42f, 0.012f, 0.013f}, {1.0f, 1.0f, 1.0f});
    rgbStrip.renderPass = RenderPass::RGB;
    rgbStrip.rgbPhase = 0.08f;
    return desk;
}

SceneNode& buildLamp(SceneNode& desk, const SceneAssets& a) {
    // A genuine hierarchy: the head and bulb are children of the rotated arm.
    auto& lamp = group(desk, "Desk lamp", {1.53f, 0, -0.29f});
    part(lamp, "Lamp foot", a.cylinder, {0, 1.17f, 0},
         {0.48f, 0.075f, 0.48f}, {0.13f, 0.16f, 0.19f}, nullptr, {1, 1}, 98);
    part(lamp, "Lower arm", a.cylinder, {0, 1.53f, 0},
         {0.07f, 0.65f, 0.07f}, {0.49f, 0.54f, 0.60f}, nullptr, {1, 1}, 108);
    part(lamp, "Hinge", a.sphere, {0, 1.84f, 0},
         {0.14f, 0.14f, 0.14f}, {0.16f, 0.20f, 0.25f});
    auto& upper = group(lamp, "Upper arm pivot", {0, 1.84f, 0}, 48.0f);
    part(upper, "Upper arm", a.cylinder, {0, 0.27f, 0},
         {0.060f, 0.55f, 0.060f}, {0.50f, 0.55f, 0.60f}, nullptr, {1, 1}, 108);
    auto& head = group(upper, "Lamp head", {0, 0.52f, 0});
    part(head, "Shade", a.cylinder, {0, 0, 0},
         {0.36f, 0.18f, 0.36f}, {0.17f, 0.20f, 0.24f}, nullptr, {1, 1}, 86);
    auto& bulb = part(head, "Warm glowing bulb", a.sphere, {0, -0.145f, 0},
                      {0.17f, 0.11f, 0.17f}, {1.0f, 0.87f, 0.64f},
                      nullptr, {1, 1}, 20);
    bulb.emission = {0.9f, 0.65f, 0.30f};
    return bulb;
}
void buildChair(SceneNode& root, const SceneAssets& a) {
    auto& chair = group(root, "Ergonomic chair", {0.10f, 0, 2.20f});
    part(chair, "Seat cushion", a.cube, {0, 0.57f, 0},
         {1.14f, 0.17f, 1.06f}, {0.14f, 0.20f, 0.29f}, nullptr, {1, 1}, 22);
    part(chair, "Chair back", a.cube, {0, 1.13f, 0.45f},
         {1.12f, 1.03f, 0.16f}, {0.15f, 0.21f, 0.30f}, nullptr, {1, 1}, 20);
    for (float x : {-0.53f, 0.53f}) {
        part(chair, "Armrest post", a.cube, {x, 0.78f, -0.10f},
             {0.07f, 0.40f, 0.09f}, {0.12f, 0.16f, 0.20f});
        part(chair, "Armrest pad", a.cube, {x, 0.98f, -0.08f},
             {0.13f, 0.065f, 0.60f}, {0.09f, 0.12f, 0.16f});
    }
    part(chair, "Chair pole", a.cylinder, {0, 0.32f, 0},
         {0.13f, 0.49f, 0.13f}, {0.10f, 0.13f, 0.18f}, nullptr, {1, 1}, 87);
    for (int i = 0; i < 5; ++i) {
        float t = float(i) * 6.28318530718f / 5.0f;
        float x = 0.30f * std::cos(t), z = 0.30f * std::sin(t);
        part(chair, "Chair wheel arm", a.cube, {x, 0.10f, z},
             {0.23f, 0.09f, 0.22f}, {0.09f, 0.13f, 0.19f});
        part(chair, "Wheel", a.sphere, {1.7f * x, 0.09f, 1.7f * z},
             {0.14f, 0.13f, 0.14f}, {0.08f, 0.09f, 0.11f});
    }
}
}
