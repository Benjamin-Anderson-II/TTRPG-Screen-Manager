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

class Window {
    struct M {
        GLFWwindow *window;
        char *name = nullptr;
        GLuint vbo;
        GLuint ebo;
        GLuint vao;
        GLuint program;
        // GLint  vertShader;
        // GLint  fragShader;
    } m;
    explicit Window(M m) : m(std::move(m)) {}
public:
    Window(const Window& other)
        : m(other.m)
    {}
    Window(Window&& other) noexcept
        : m(other.m)
    {
        other.m.window  = nullptr;
        other.m.name    = nullptr;
        other.m.vbo     = 0;
        other.m.ebo     = 0;
        other.m.vao     = 0;
        other.m.program = 0;
        // other.m.vertShader = nullptr;
        // other.m.fragShader = nullptr;
    }
    Window& operator=(const Window& other) {
        if(this == &other)
            return *this;
        // m = other.m;
        m.window = other.m.window;
        m.name = other.m.name;
        m.vbo = other.m.vbo;
        m.ebo = other.m.ebo;
        m.vao = other.m.vao;
        m.program = other.m.program;
        return *this;
    }

    /* Member Functions */
    static std::optional<Window> create(std::string);

    void move(Window&);
    void move(std::optional<Window>&);
    void stage();
    void render();

    GLFWwindow *window() { return m.window; }
    char *name() { return m.name; }

    ~Window();
};

#endif
