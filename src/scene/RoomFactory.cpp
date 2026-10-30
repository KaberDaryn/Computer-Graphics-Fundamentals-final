#include "scene/SceneFactory.hpp"
#include "graphics/Mesh.hpp"

namespace SceneFactory {
void buildRoom(SceneNode& root, const SceneAssets& a) {
    // Room shell: all planar surfaces have deliberately separate depth coordinates.
    part(root, "Oak floor", a.plane, {0, -0.015f, 0}, {10, 1, 10},
         {0.75f, 0.80f, 0.86f}, a.floor, {9, 9}, 18);
    part(root, "Back wall", a.cube, {0, 2.5f, -3.45f}, {10, 5, 0.10f},
         {0.25f, 0.31f, 0.43f}, nullptr, {1, 1}, 16);
    part(root, "Side wall", a.cube, {-4.70f, 2.5f, 0}, {0.10f, 5, 9.0f},
         {0.22f, 0.28f, 0.38f}, nullptr, {1, 1}, 16);
    part(root, "Baseboard", a.cube, {0, 0.10f, -3.35f}, {9.3f, 0.17f, 0.06f},
         {0.12f, 0.16f, 0.22f});

    // Procedural sky behind the window frame; day/night is driven by GLSL.
    auto& window = group(root, "Window frame", {2.50f, 2.69f, -3.365f});
    auto& sky = part(window, "Window sky gradient", a.cube, {0, 0, 0.045f},
                     {1.95f, 1.35f, 0.026f}, {1, 1, 1});
    sky.renderPass = RenderPass::Sky;
    for (float side : {-1.0f, 1.0f}) {
        part(window, "Frame vertical", a.cube, {side * 1.01f, 0, 0.10f},
             {0.09f, 1.50f, 0.10f}, {0.08f, 0.11f, 0.15f});
        part(window, "Frame horizontal", a.cube, {0, side * 0.72f, 0.10f},
             {2.12f, 0.085f, 0.10f}, {0.08f, 0.11f, 0.15f});
    }
    part(window, "Window mullion", a.cube, {0, 0, 0.108f},
         {0.07f, 1.40f, 0.08f}, {0.08f, 0.11f, 0.15f});
    part(window, "Window crossbar", a.cube, {0, 0.0f, 0.108f},
         {2.0f, 0.055f, 0.08f}, {0.08f, 0.11f, 0.15f});
    part(window, "Window sill", a.cube, {0, -0.78f, 0.28f},
         {2.28f, 0.10f, 0.40f}, {0.69f, 0.66f, 0.66f});

    auto& shelf = group(root, "Wall shelves", {-2.55f, 2.65f, -3.10f});
    for (int r = 0; r < 2; ++r) {
        const float y = 0.30f - float(r) * 0.72f;
        part(shelf, "Wood shelf", a.cube, {0, y, 0}, {1.75f, 0.09f, 0.38f},
             {0.83f, 0.74f, 0.62f}, a.wood);
        for (int b = 0; b < 5; ++b) {
            float height = 0.23f + 0.04f * float(b % 3);
            part(shelf, "Book spine", a.cube,
                 {-0.64f + float(b) * 0.18f, y + height * 0.5f + 0.045f, 0},
                 {0.11f, height, 0.27f},
                 {0.18f + 0.09f * float(b % 3), 0.25f + 0.05f * float(b % 2),
                  0.36f + 0.04f * float(b % 2)});
        }
    }
    part(root, "Small rug", a.cube, {0, 0.018f, 2.25f}, {2.15f, 0.025f, 1.9f},
         {0.15f, 0.23f, 0.32f}, nullptr, {1, 1}, 8);
}
}
