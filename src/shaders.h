#ifndef SHADERS_H
#define SHADERS_H

#include <optional>
#include <glad/gl.h>

class Shaders {
    struct M{
        GLuint programID;
    } m;
    explicit Shaders(M m) : m(std::move(m)) {}
public:
    static std::optional<Shaders> create(const char *vertexShaderPath, const char *fragmentShaderPath);

    Shaders(const Shaders& other);
    Shaders& operator=(const Shaders& other);
    Shaders(Shaders&& other) noexcept;
    Shaders& operator=(Shaders&& other) noexcept;

    void use() noexcept;
    bool change(const char *vertexPath, const char *fragmentPath);

    ~Shaders();
};

#endif
