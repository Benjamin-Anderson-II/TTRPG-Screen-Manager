#include "window.h"
#include "callbacks.h"

std::optional<Window> Window::create(std::string winName){
    GLFWwindow *window;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIN_DEFAULT_WIDTH, WIN_DEFAULT_HEIGHT, winName.c_str(), nullptr, nullptr);
    if(!window){
        glfwTerminate();
        return std::nullopt;
    }

    glfwSetKeyCallback(window, GLFWKeyCallback);

    glfwMakeContextCurrent(window);
    auto version = gladLoadGL(glfwGetProcAddress);
    if(!version){
        glfwTerminate();
        return std::nullopt;
    }
    glfwSwapInterval(1);

    return Window(M{
        .window = std::move(window),
    });
}

void Window::render() {
    glfwMakeContextCurrent(m.window);
    glfwSwapBuffers(m.window);
}

Window::~Window() {
    if(m.window)
        glfwDestroyWindow(m.window);
}
