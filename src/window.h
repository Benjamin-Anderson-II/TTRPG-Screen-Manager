#ifndef WINDOW_H
#define WINDOW_H

#include <optional>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define WIN_DEFAULT_WIDTH  800
#define WIN_DEFAULT_HEIGHT 600

struct Window {
    struct M {
        GLFWwindow *window;
    } m;
    explicit Window(M m) : m(std::move(m)) {}
    Window(const Window& other)
        : m(other.m)
    {}
    Window(Window&& other) noexcept
        : m(other.m)
    {
        other.m.window = nullptr;
    }

    /* Member Functions */
    static std::optional<Window> create(std::string);
    void render();

    ~Window();
};

#endif
