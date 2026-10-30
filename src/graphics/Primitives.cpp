#include "graphics/Primitives.hpp"
#include <cmath>
#include <stdexcept>

namespace {
void appendQuad(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
                const glm::vec3& d, const glm::vec3& normal) {
    const unsigned int base = static_cast<unsigned int>(vertices.size());
    vertices.push_back({a, normal, {0.0f, 0.0f}});
    vertices.push_back({b, normal, {1.0f, 0.0f}});
    vertices.push_back({c, normal, {1.0f, 1.0f}});
    vertices.push_back({d, normal, {0.0f, 1.0f}});
    indices.insert(indices.end(), {base, base+1, base+2, base, base+2, base+3});
}
constexpr float pi = 3.14159265358979323846f;
}

std::unique_ptr<Mesh> Primitives::cube() {
    std::vector<Vertex> v;
    std::vector<unsigned int> i;
    const float h = 0.5f;
    appendQuad(v,i,{-h,-h,h},{h,-h,h},{h,h,h},{-h,h,h},{0,0,1});
    appendQuad(v,i,{h,-h,-h},{-h,-h,-h},{-h,h,-h},{h,h,-h},{0,0,-1});
    appendQuad(v,i,{h,-h,h},{h,-h,-h},{h,h,-h},{h,h,h},{1,0,0});
    appendQuad(v,i,{-h,-h,-h},{-h,-h,h},{-h,h,h},{-h,h,-h},{-1,0,0});
    appendQuad(v,i,{-h,h,h},{h,h,h},{h,h,-h},{-h,h,-h},{0,1,0});
    appendQuad(v,i,{-h,-h,-h},{h,-h,-h},{h,-h,h},{-h,-h,h},{0,-1,0});
    return std::make_unique<Mesh>(v,i);
}
std::unique_ptr<Mesh> Primitives::plane() {
    std::vector<Vertex> v;
    std::vector<unsigned int> i;
    appendQuad(v,i,{-0.5f,0,0.5f},{0.5f,0,0.5f},
               {0.5f,0,-0.5f},{-0.5f,0,-0.5f},{0,1,0});
    return std::make_unique<Mesh>(v,i);
}
// A single front-facing XY quad. UV (0,0) is bottom-left in OpenGL.
std::unique_ptr<Mesh> Primitives::screenQuad() {
    std::vector<Vertex> v;
    std::vector<unsigned int> i;
    appendQuad(v, i, {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f},
               {0.5f, 0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f});
    return std::make_unique<Mesh>(v, i);
}

std::unique_ptr<Mesh> Primitives::cylinder(unsigned int segments) {
    if (segments < 3) throw std::invalid_argument("Cylinder needs at least 3 segments");
    std::vector<Vertex> v;
    std::vector<unsigned int> i;
    for (unsigned int s=0; s<=segments; ++s) {
        const float t = 2.0f*pi*static_cast<float>(s)/static_cast<float>(segments);
        const float x = std::cos(t), z = std::sin(t);
        v.push_back({{0.5f*x,-0.5f,0.5f*z},{x,0,z},{float(s)/float(segments),0}});
        v.push_back({{0.5f*x, 0.5f,0.5f*z},{x,0,z},{float(s)/float(segments),1}});
        if (s<segments) {
            unsigned int b=2*s;
            i.insert(i.end(),{b,b+1,b+2, b+2,b+1,b+3});
        }
    }
    const unsigned int topCenter=static_cast<unsigned int>(v.size());
    v.push_back({{0,0.5f,0},{0,1,0},{0.5f,0.5f}});
    const unsigned int bottomCenter=static_cast<unsigned int>(v.size());
    v.push_back({{0,-0.5f,0},{0,-1,0},{0.5f,0.5f}});
    const unsigned int topStart=static_cast<unsigned int>(v.size());
    for (unsigned int s=0; s<=segments; ++s) {
        float t=2.0f*pi*float(s)/float(segments);
        float x=std::cos(t), z=std::sin(t);
        v.push_back({{0.5f*x,0.5f,0.5f*z},{0,1,0},{(x+1)*0.5f,(z+1)*0.5f}});
        v.push_back({{0.5f*x,-0.5f,0.5f*z},{0,-1,0},{(x+1)*0.5f,(z+1)*0.5f}});
        if (s<segments) {
            i.insert(i.end(),{topCenter,topStart+2*(s+1),topStart+2*s});
            i.insert(i.end(),{bottomCenter,topStart+2*s+1,topStart+2*(s+1)+1});
        }
    }
    return std::make_unique<Mesh>(v,i);
}
std::unique_ptr<Mesh> Primitives::sphere(unsigned int stacks, unsigned int slices) {
    if (stacks < 2 || slices < 3) throw std::invalid_argument("Sphere tessellation too low");
    std::vector<Vertex> v;
    std::vector<unsigned int> i;
    for (unsigned int a=0; a<=stacks; ++a) {
        float theta=pi*float(a)/float(stacks);
        for (unsigned int b=0; b<=slices; ++b) {
            float phi=2*pi*float(b)/float(slices);
            glm::vec3 n={std::sin(theta)*std::cos(phi), std::cos(theta),
                         std::sin(theta)*std::sin(phi)};
            v.push_back({n*0.5f,n,{float(b)/float(slices),1-float(a)/float(stacks)}});
        }
    }
    for (unsigned int a=0; a<stacks; ++a) {
        for (unsigned int b=0; b<slices; ++b) {
            unsigned int one=a*(slices+1)+b, next=(a+1)*(slices+1)+b;
            if (a>0) i.insert(i.end(),{one,one+1,next});
            if (a+1<stacks) i.insert(i.end(),{one+1,next+1,next});
        }
    }
    return std::make_unique<Mesh>(v,i);
}
