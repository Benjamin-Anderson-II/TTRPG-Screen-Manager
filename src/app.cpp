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
    Window *PCWin;
    Window *GMWin;
    std::string err_msg = "";

    // Log File Stuff
    logFile.open(LOG_FILE_PATH);
    if(!logFile.is_open()){
        std::cerr << "Fatal Error: Unable to open log file" << std::endl;
        return std::nullopt;
    }
    log_message("Log File Opened!");

    // Allocate member variables
    PCWin = (Window*)malloc(sizeof(Window));
    if(!PCWin){
        err_msg = "Fatal Error: App: Failed to malloc Player Window";
        goto fail;
    }
    GMWin = (Window*)malloc(sizeof(Window));
    if(!GMWin){
        err_msg = "Fatal Error: App: Failed to malloc Game Master Window";
        goto fail;
    }

    glfwSetErrorCallback(GLFWErrorCallback);
    log_message("GLFW: Set Error Callback");

    if(!glfwInit()){
        err_msg = "Fatal Error: App: Failed to Initialize GLFW";
    }

    { // Create PCWin
        std::optional<Window> o_PCWin = Window::create("Player");
        if(o_PCWin == std::nullopt){
            err_msg = "Fatal Error: App: Failed to Create Player Window";
            goto fail;
        }
        *PCWin = std::move(o_PCWin.value());
        PCWin->addRectangle_NDC(2,2,-1,-1);
    }
    { // Create GMWin
        std::optional<Window> o_GMWin = Window::create("Game Master");
        if(o_GMWin == std::nullopt){
            err_msg = "Fatal Error: App: Failed to Create Game Master Window";
            goto fail;
        }
        *GMWin = std::move(o_GMWin.value());
    }

    log_app_info();

    return App(M {
        .PCWin = std::move(PCWin),
        .GMWin = std::move(GMWin),
    });
fail:
    if(PCWin) free(PCWin);
    if(GMWin) free(GMWin);
    log_message("{}", err_msg);
    return std::nullopt;
}
// Copy Constructor
App::App(const App& other) : m(other.m) {}
// Copy Assignment
App& App::operator=(const App& other) {
    if(this == &other)
        return *this;
    m = other.m;
    return *this;
}
// Move Constructor
App::App(App&& other) noexcept
    : m(other.m)
{
    other.m.PCWin = nullptr;
    other.m.GMWin = nullptr;
    other.m.is_open = false;
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

    while(!glfwWindowShouldClose(m.PCWin->window()) && !glfwWindowShouldClose(m.GMWin->window())){
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
    if(m.PCWin)
        free(m.PCWin);
    if(m.GMWin)
        free(m.GMWin);
}
