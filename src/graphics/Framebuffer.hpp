#pragma once
#include <glad/glad.h>

// Offscreen render target: one RGB color texture and a depth renderbuffer.
// Allocate once, render each frame; no texture uploads inside the frame loop.
class Framebuffer final {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    void bind() const;
    static void unbind();
    void bindColorTexture(unsigned int unit) const;
    int width() const { return width_; }
    int height() const { return height_; }
private:
    GLuint fbo_ = 0;
    GLuint color_ = 0;
    GLuint depth_ = 0;
    int width_ = 0;
    int height_ = 0;
};
