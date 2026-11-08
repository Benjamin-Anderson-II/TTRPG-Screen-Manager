#ifndef CALLBACKS_H
#define CALLBACKS_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

void GLFWErrorCallback(int err, const char *desc);
void GLFWKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userProgram);
void GLFWRFrameBufferSizeCallback(GLFWwindow *window, int width, int height);

#endif
