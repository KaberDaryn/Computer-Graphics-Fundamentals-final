#pragma once
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader final {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

    void use() const;
    void setBool(const char* name, bool value) const;
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVec2(const char* name, const glm::vec2& value) const;
    void setVec3(const char* name, const glm::vec3& value) const;
    void setMat4(const char* name, const glm::mat4& value) const;

private:
    GLuint id_ = 0;
    mutable std::unordered_map<std::string, GLint> locations_;
    GLint location(const char* name) const;
    static std::string readFile(const std::string& path);
    static GLuint compile(GLenum type, const std::string& source);
};
