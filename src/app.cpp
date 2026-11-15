#include <optional>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "log.h"
#include "app.h"
#include "callbacks.h"

void log_app_info(){
    log_message("App: OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

std::optional<App> App::create() {
    std::string err_msg = "";

    // Log File Stuff
    logFile.open(LOG_FILE_PATH);
    if(!logFile.is_open()){
        std::cerr << "Fatal Error: Unable to open log file" << std::endl;
        return std::nullopt;
    }
    log_message("Log File Opened!");

    glfwSetErrorCallback(GLFWErrorCallback);
    log_message("GLFW: Set Error Callback");

    if(!glfwInit()){
        err_msg = "Fatal Error: App: Failed to Initialize GLFW";
    }

    // Create & Init Player Window
    auto o_PCWin = Window::create("Player");
    if(o_PCWin == std::nullopt){
        log_message("Fatal Error::App: Failed to Create Player Window");
        return std::nullopt;
    }
    auto PCWin = std::move(o_PCWin.value());
    PCWin.addTexture("textures/container.jpg");
    PCWin.addRectangle_NDC(2,2,-1,-1);
    PCWin.rectangleAt(0).changeShaders("src/shaders/player_vertex.glsl", "src/shaders/player_fragment.glsl");

    // Create & Init Game Master Window
    auto o_GMWin = Window::create("Game Master");
    if(o_GMWin == std::nullopt){
        log_message("Fatal Error::App: Failed to Create Game Master Window");
        return std::nullopt;
    }
    auto GMWin = std::move(o_GMWin.value());

    log_app_info();

    return App(M {
        .PCWin = std::make_unique<Window>(std::move(PCWin)),
        .GMWin = std::make_unique<Window>(std::move(GMWin)),
    });
}
// Move Constructor
App::App(App&& other) noexcept {
    other.m.is_open = false;
    m.PCWin = std::move(other.m.PCWin);
    m.GMWin = std::move(other.m.GMWin);
}
// Move Assignment
App& App::operator=(App&& other) noexcept {
    std::swap(other.m.PCWin, m.PCWin);
    std::swap(other.m.GMWin, m.GMWin);
    other.m.is_open = false;
    return *this;
}

void App::launch(){
    log_message("Launching App");
    // m.PCWin->stage();

    while(!m.PCWin->shouldClose() && !m.GMWin->shouldClose()){
        m.GMWin->render();
        m.PCWin->render();
        glfwPollEvents();
    }
}

App::~App() {
    if(m.is_open){ // moved-from App should not terminate GLFW
        glfwTerminate();
        log_message("GLFW Terminated");
        if(logFile.is_open())
            logFile.close();
    }
}
