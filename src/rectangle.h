#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <optional>
#include <glad/gl.h>

#include "shader.h"

class Rectangle {
    struct M{
        GLuint vbo;
        GLuint bindingIdx;
        bool isNDC;
        // NOTE: might swap with a vertex array if it ends up being easier
        float width, height;
        float x, y;
        Shader *shader;
    } m;
    explicit Rectangle(M m) : m(std::move(m)) {}
public:
    // x and y represent the bottom left corner of the rectangle
    static std::optional<Rectangle> create_ndc(GLuint vao, float width, float height, float x, float y, Shader *shader);
    // static std::optional<Rectangle> create_pixels(GLuint vao, GLuint width, GLuint height, GLuint x, GLuint y);
    Rectangle(const Rectangle&);
    Rectangle& operator=(const Rectangle&);
    Rectangle(Rectangle&&) noexcept;
    Rectangle& operator=(Rectangle&&) noexcept;

    bool changeShaders(const char *vertexShaderPath, const char *fragmentShaderPath);
    bool draw(GLuint vao);
};

#endif
