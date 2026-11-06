#ifndef CALLBACKS_H
#define CALLBACKS_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void GLFWErrorCallback(int err, const char *desc);
void GLFWKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

#endif
