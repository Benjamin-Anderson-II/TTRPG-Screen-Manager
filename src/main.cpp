#include <iostream>
#include <fstream>
#include <format>
#include <string>
#include <optional>
#include <utility>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define LOG_FILE_NAME "debug/tabletop.log"
std::ofstream logFile;

#define WIN_DEFAULT_WIDTH 800
#define WIN_DEFAULT_HEIGHT 600

static void GLFWErrorCallback(int err, const char *desc) {
    logFile << "GLFW ERROR = " << err << ": '" << desc << "'\n";
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

template <typename... Args>
void log_message(std::format_string<Args...> fmt, Args&&... args){
    std::string msg = std::format(fmt, std::forward<Args>(args)...);
    logFile << msg << std::endl;
}



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

    glfwSetKeyCallback(window, key_callback);

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



bool app_init() {
    logFile.open(LOG_FILE_NAME);
    if(!logFile.is_open()){
        std::cerr << "Could not open Log File. Aborting.\n";
        return 1;
    }

    glfwSetErrorCallback(GLFWErrorCallback);

    if(!glfwInit())
        return false;

    return true;
}
void app_terminate() {
    glfwTerminate();
    logFile.close();
}

int main(int argc, const char *argv[]) {
    app_init();

    std::optional<Window> o_PCWin = Window::create("Players");
    if(o_PCWin == std::nullopt) return 1;
    Window PCWin = o_PCWin.value();

    std::optional<Window> o_GMWin = Window::create("Game Master");
    if(o_GMWin == std::nullopt) return 1;
    Window GMWin = o_GMWin.value();

    while(!glfwWindowShouldClose(PCWin.m.window) && !glfwWindowShouldClose(GMWin.m.window)){
        GMWin.render();
        PCWin.render();
        glfwPollEvents();
    }

    app_terminate();
    return 0;
}
