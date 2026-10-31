#include "graphics/Camera.hpp"
#include <glm/geometric.hpp>
#include <cmath>
#include <stdexcept>
int main() {
    Camera a, b;
    a.orbit(80.0f, 0.0f);
    for (int i=0;i<100;++i) b.orbit(0.8f,0.0f);
    if (glm::distance(a.position(),b.position()) > 0.001f)
        throw std::runtime_error("Orbit depends on update count");
    a.zoom(100000.0f);
    a.orbit(0,100000.0f);
    const auto v=a.viewMatrix();
    for(int c=0;c<4;++c) for(int r=0;r<4;++r)
        if(!std::isfinite(v[c][r])) throw std::runtime_error("Invalid clamped view");
    a.reset(); Camera initial;
    if(glm::distance(a.position(),initial.position())>0.001f)
        throw std::runtime_error("Reset failed");
}
