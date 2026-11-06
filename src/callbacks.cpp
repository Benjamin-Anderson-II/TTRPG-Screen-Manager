#include "utils.h"
#include "callbacks.h"

void GLFWErrorCallback(int err, const char *desc) {
    logFile << "GLFW ERROR = " << err << ": '" << desc << "'\n";
}

void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
