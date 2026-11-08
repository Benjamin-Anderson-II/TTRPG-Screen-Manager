#include "callbacks.h"
#include "glad/gl.h"
#include "log.h"

void GLFWErrorCallback(int err, const char *desc) {
    log_message("GLFW ERROR: {}: {}", err, desc);
}

void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userProgram){
    log_message("OpenGL Debug Message: {}", message);
}

// void GLFWRFrameBufferSizeCallback(GLFWwindow *window, int width, int height){
//     glViewport(0,0,width,height);
// }
