#include "graphics/Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

Texture::Texture(const std::string& filename) {
    stbi_set_flip_vertically_on_load(true);
    int width=0, height=0, components=0;
    unsigned char* pixels=stbi_load(filename.c_str(), &width, &height, &components, 0);
    if (!pixels) throw std::runtime_error("Cannot load texture: " + filename);
    GLenum format = GL_RGB;
    if (components == 1) format = GL_RED;
    else if (components == 3) format = GL_RGB;
    else if (components == 4) format = GL_RGBA;
    else {
        stbi_image_free(pixels);
        throw std::runtime_error("Unsupported texture components: " + filename);
    }
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,0,static_cast<GLint>(format),width,height,0,
                 format,GL_UNSIGNED_BYTE,pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);
}
Texture::~Texture() { if (id_) glDeleteTextures(1, &id_); }
void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id_);
}
