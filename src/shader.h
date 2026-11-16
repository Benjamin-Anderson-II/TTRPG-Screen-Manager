#ifndef SHADER_H
#define SHADER_H

#include <optional>
#include <glad/gl.h>

class Shader {
    struct M{
        GLuint programID;
    } m;
    explicit Shader(M m) : m(std::move(m)) {}
public:
    static std::optional<Shader> create(const char *vertexShaderPath, const char *fragmentShaderPath);

    // A copied shader means duplicated programID, which
    // if destructed will delete the opengl program
    /* NEVER CALL THESE COPIERS
     *  (I have to define them because std::vector won't compile otherwise...) */
    Shader(const Shader& other) = default;
    Shader& operator=(const Shader& other) = default;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void use() noexcept;
    bool change(const char *vertexPath, const char *fragmentPath);

    ~Shader();
};

#endif
