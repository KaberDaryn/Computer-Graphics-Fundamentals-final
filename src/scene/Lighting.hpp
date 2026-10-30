#pragma once
class Shader;
class Scene;

namespace Lighting {
// Uploads fixed ceiling / window lights and one toggleable desk lamp.
void upload(const Shader& shader, const Scene& scene, float time);
}
