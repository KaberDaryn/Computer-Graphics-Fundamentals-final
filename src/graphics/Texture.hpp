#pragma once
#include <glad/glad.h>
#include <string>

class Texture final {
public:
    explicit Texture(const std::string& filename);
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;
    void bind(unsigned int slot = 0) const;
private:
    GLuint id_ = 0;
};
