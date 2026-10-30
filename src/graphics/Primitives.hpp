#pragma once
#include "graphics/Mesh.hpp"
#include <memory>

namespace Primitives {
std::unique_ptr<Mesh> cube();
std::unique_ptr<Mesh> plane();
std::unique_ptr<Mesh> screenQuad();
std::unique_ptr<Mesh> cylinder(unsigned int segments = 32);
std::unique_ptr<Mesh> sphere(unsigned int stacks = 16, unsigned int slices = 32);
}
