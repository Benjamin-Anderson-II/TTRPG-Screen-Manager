#include <optional>
#include <iostream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "window.h"
#include "callbacks.h"
#include "log.h"

/* Have to define the extern variable somewhere */
std::ofstream logFile;

/* 
 * I can't define App like I do with all of my other classes,
 * since a call to the destructor would terminate glfw.
 * As such i'm adopting the C++ standard way of initializing a
 * class, even if it technically comes at the cost of weaker
 * invariances.
 */
class App {
    Window *PCWin;
    Window *GMWin;
public:
    App();
    void launch();
    ~App();
};

App::App() {
    logFile.open(LOG_FILE_PATH);
    if(!logFile.is_open()) {
        std::cerr << "Fatal Error: Unable to open log file: " << LOG_FILE_PATH << std::endl;
        exit(1);
    }
    log_message("Log File Opened!");

    glfwSetErrorCallback(GLFWErrorCallback);
    logFile << "GLFW: Set Error Callback\n";

    if(!glfwInit())
        exit(1);

    /* Try as I might I can't get normal move semantics to work here,
     * so I'm making my own move function... */

    std::optional<Window> o_PCWin = Window::create("Player");
    if(o_PCWin == std::nullopt){
        log_message("Failed to Create Player Window");
        exit(1);
    }
    PCWin = (Window *)malloc(sizeof(Window));
    PCWin->move(o_PCWin);

    std::optional<Window> o_GMWin = Window::create("Game Master");
    if(o_GMWin == std::nullopt){
        log_message("Failed to Create Game Master Window");
        exit(1);
    }
    GMWin = (Window *)malloc(sizeof(Window));
    GMWin->move(o_GMWin);

    log_message("App: OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

void App::launch(){
    log_message("Launching App");
    PCWin->stage();

    while(!glfwWindowShouldClose(PCWin->window()) && !glfwWindowShouldClose(GMWin->window())){
        GMWin->render();
        PCWin->render();
        glfwPollEvents();
    }
}

App::~App() {
    glfwTerminate();
    logFile.close();
}

int main(int argc, const char *argv[]) {
    std::cout << "Starting Application\n";
    App app;
    app.launch();
    return 0;
}
