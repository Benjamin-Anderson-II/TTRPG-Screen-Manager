#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <optional>
#include <glad/gl.h>

class Rectangle {
    struct M{
        GLuint vbo;
        GLuint bindingIdx;
        bool isNDC;
        // NOTE: might swap with a vertex array if it ends up being easier
        float width, height;
        float x, y;
        GLuint program;
        GLuint vertex_shader;
        GLuint fragment_shader;
    } m;
    explicit Rectangle(M m) : m(std::move(m)) {}
public:
    // x and y represent the bottom left corner of the rectangle
    static std::optional<Rectangle> create_ndc(GLuint vao, float width, float height, float x, float y);
    // static std::optional<Rectangle> create_pixels(GLuint vao, GLuint width, GLuint height, GLuint x, GLuint y);
    // TODO: implement rule of 5

    bool changeShaders(const char *vertexShaderPath, const char *fragmentShaderPath);
    bool draw(GLuint vao);
};

#endif
