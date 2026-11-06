#include <optional>
#include <iostream>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "utils.h"
#include "window.h"
#include "callbacks.h"

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
