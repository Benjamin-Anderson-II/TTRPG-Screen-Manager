#ifndef WINDOW_H
#define WINDOW_H

#include <optional>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>
#include "rectangle.h"

#define WIN_DEFAULT_WIDTH  800
#define WIN_DEFAULT_HEIGHT 600

class Window {
    struct M {
        GLFWwindow *window;
        std::string name = {};
        // TODO: pull out into State struct
        std::vector<Rectangle> rectangles;
        // TODO: Implement ability to have multiple textures
        // std::vector<std::pair<GLuint, std::string>> textures;
        GLuint vao;
    } m;
    explicit Window(M m) : m(std::move(m)) {}
public:
    static std::optional<Window> create(std::string);

    Window(const Window& other);
    Window& operator=(const Window& other);

    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    /* Member Functions */
    bool addRectangle_NDC(float width, float height, float x, float y);
    // bool addRectangle_Pixel(GLuint width, GLuint height, GLuint x, GLuint y);
    bool addTexture(const char *texturePath);
    bool setRectangleShaders(GLuint rectIdx, const char *vertexShaderPath, const char *fragmentShaderPath);
    void render();

    GLFWwindow *window() { return m.window; }
    std::string name() { return m.name; }

    ~Window();
};

#endif
