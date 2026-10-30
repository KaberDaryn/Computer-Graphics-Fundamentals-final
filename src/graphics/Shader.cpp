#include "graphics/Shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Cannot open shader: " + path);
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compile(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* data = source.c_str();
    glShaderSource(shader, 1, &data, nullptr);
    glCompileShader(shader);
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(static_cast<size_t>(length > 1 ? length : 1), '\0');
        glGetShaderInfoLog(shader, length, nullptr, log.data());
        glDeleteShader(shader);
        throw std::runtime_error("GLSL compilation failed: " + std::string(log.data()));
    }
    return shader;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vertex = compile(GL_VERTEX_SHADER, readFile(vertexPath));
    GLuint fragment = 0;
    try {
        fragment = compile(GL_FRAGMENT_SHADER, readFile(fragmentPath));
        id_ = glCreateProgram();
        glAttachShader(id_, vertex);
        glAttachShader(id_, fragment);
        glLinkProgram(id_);
        GLint success = GL_FALSE;
        glGetProgramiv(id_, GL_LINK_STATUS, &success);
        if (success != GL_TRUE) {
            GLint length = 0;
            glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(static_cast<size_t>(length > 1 ? length : 1), '\0');
            glGetProgramInfoLog(id_, length, nullptr, log.data());
            throw std::runtime_error("GLSL linking failed: " + std::string(log.data()));
        }
    } catch (...) {
        glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);
        if (id_) glDeleteProgram(id_);
        throw;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() { if (id_) glDeleteProgram(id_); }
void Shader::use() const { glUseProgram(id_); }
GLint Shader::location(const char* name) const {
    const auto found = locations_.find(name);
    if (found != locations_.end()) return found->second;
    const GLint value = glGetUniformLocation(id_, name);
    locations_.emplace(name, value);
    return value;
}
void Shader::setBool(const char* n, bool v) const { glUniform1i(location(n), v ? 1 : 0); }
void Shader::setInt(const char* n, int v) const { glUniform1i(location(n), v); }
void Shader::setFloat(const char* n, float v) const { glUniform1f(location(n), v); }
void Shader::setVec2(const char* n, const glm::vec2& v) const {
    glUniform2fv(location(n), 1, glm::value_ptr(v));
}
void Shader::setVec3(const char* n, const glm::vec3& v) const {
    glUniform3fv(location(n), 1, glm::value_ptr(v));
}
void Shader::setMat4(const char* n, const glm::mat4& v) const {
    glUniformMatrix4fv(location(n), 1, GL_FALSE, glm::value_ptr(v));
}
