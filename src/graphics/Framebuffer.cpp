#include "graphics/Framebuffer.hpp"
#include <stdexcept>

Framebuffer::Framebuffer(int width, int height) : width_(width), height_(height) {
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("Framebuffer dimensions must be positive");

    // Based on LearnOpenGL: Framebuffers (https://learnopengl.com/Advanced-OpenGL/Framebuffers).
    // Adapted as a reusable RAII class with depth buffer and validation.
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glGenTextures(1, &color_);
    glBindTexture(GL_TEXTURE_2D, color_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width_, height_, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, color_, 0);

    glGenRenderbuffers(1, &depth_);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depth_);

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteRenderbuffers(1, &depth_);
        glDeleteTextures(1, &color_);
        glDeleteFramebuffers(1, &fbo_);
        depth_ = color_ = fbo_ = 0;
        throw std::runtime_error("Offscreen framebuffer is incomplete (status " +
                                 std::to_string(static_cast<unsigned>(status)) + ")");
    }
}
Framebuffer::~Framebuffer() {
    if (depth_) glDeleteRenderbuffers(1, &depth_);
    if (color_) glDeleteTextures(1, &color_);
    if (fbo_) glDeleteFramebuffers(1, &fbo_);
}
void Framebuffer::bind() const { glBindFramebuffer(GL_FRAMEBUFFER, fbo_); }
void Framebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void Framebuffer::bindColorTexture(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, color_);
}
