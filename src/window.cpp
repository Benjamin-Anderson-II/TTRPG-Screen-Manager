#include "window.h"
#include "GLFW/glfw3.h"
#include "callbacks.h"
#include "log.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <glad/gl.h>

std::optional<Window> Window::create(std::string winName){
    GLFWwindow *window;
    char *name = static_cast<char *>(std::malloc(winName.size() + 1));
    if(!name) return std::nullopt;
    std::memcpy(name, winName.data(), winName.size());
    name[winName.size()] = 0;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

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

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,             0, nullptr, GL_FALSE);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH,   0, nullptr, GL_TRUE);

    return Window(M{
        .window = std::move(window),
        .name   = std::move(name),
    });
}

const float vertex_data[] = {
    -.5f,  .5f, // top left
    -.5f, -.5f, // bottom left
     .5f, -.5f, // bottom right

    -.5f,  .5f, // top left
     .5f, -.5f, // bottom right
     .5f,  .5f, // top right
};

constexpr const char *VERTEX_SHADER_SRC = R"(
#version 460 core

layout (location = 0) in vec2 a_pos;

void main() {
    gl_Position = vec4(a_pos, 0.0, 1.0);
}
)";

constexpr const char *FRAGMENT_SHADER_SRC = R"(
#version 460 core

layout (location = 0) out vec4 frag_color;

void main() {
    frag_color = vec4(1.0);
}
)";

GLuint compile_shader(GLenum type, const char *src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // GLint success;
    // glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    // if(!success) {
    //     GLint length;
    //     glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    //     if(!length)
    //         log_message("Failed to compile shader. No info log.");
    //     else {
    //         std::string info_log(length, 0);
    //         glGetShaderInfoLog(shader, length, nullptr, info_log.data());
    //         log_message("Failed to compile Shader. Info Log:\n\t{}", info_log);
    //     }
    // }

    return shader;
}

/* A syntactic nightmare to be sure, but I'm too
 * annoyed at figuring out move semantics to care */
void Window::move(Window& other) {
    m.window = other.m.window;
    m.name = other.m.name;
    m.vbo = other.m.vbo;
    m.vao = other.m.vao;
    m.program = other.m.program;

    other.m.window = nullptr;
    other.m.name = nullptr;
    other.m.vbo = 0;
    other.m.vao = 0;
    other.m.program = 0;
}

void Window::stage() {
    glfwMakeContextCurrent(m.window);
    log_message("{}: Staging.", (void*)m.window);

    glCreateBuffers(1, &m.vbo);
    if(glIsBuffer(m.vbo) == GL_FALSE){
        std::cerr << "GL Error: " << glGetError() << std::endl;
        log_message("Failed to initialize Vertex Buffer.");
    }
    glNamedBufferStorage(m.vbo, sizeof(vertex_data), vertex_data, 0);

    glCreateVertexArrays(1, &m.vao);
    glVertexArrayVertexBuffer(m.vao, 0, m.vbo, 0, 2*sizeof(float));
    glVertexArrayBindingDivisor(m.vao, 0, 0);

    glVertexArrayAttribBinding(m.vao, 0, 0);
    glVertexArrayAttribFormat(m.vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(m.vao, 0);

    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SRC);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SRC);

    m.program = glCreateProgram();
    glAttachShader(m.program, vertex_shader);
    glAttachShader(m.program, fragment_shader);
    glLinkProgram(m.program);

    // GLint success;
    // glGetProgramiv(m.program, GL_LINK_STATUS, &success);
    // if(!success) {
    //     GLint length;
    //     glGetProgramiv(m.program, GL_INFO_LOG_LENGTH, &length);
    //     if(!length)
    //         log_message("Failed to link program. No info log.");
    //     else {
    //         std::string info_log(length, 0);
    //         glGetProgramInfoLog(m.program, length, nullptr, info_log.data());
    //         log_message("Failed to link program. Info Log:\n\t{}", info_log);
    //         std::cerr << "Failed to link program. Info Log:\n\t" << info_log << std::endl;
    //     }
    // }
    //
    // glDeleteShader(vertex_shader);
    // glDeleteShader(fragment_shader);

    // glDrawBuffer(GL_BACK);
    glClearColor(1,0,0,1);

    glUseProgram(m.program);
    glBindVertexArray(m.vao);
}

void Window::render() {
    glfwMakeContextCurrent(m.window);
    // TODO: figure out how big the quad should be HERE

    glViewport(0, 0, WIN_DEFAULT_HEIGHT, WIN_DEFAULT_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // TODO: potentially free the quad's vertex data HERE

    glfwSwapBuffers(m.window);
}

Window::~Window() {
    if(m.window)
        glfwDestroyWindow(m.window);
    if(m.name)
        free(m.name);
    if(m.vbo && glIsBuffer(m.vbo))
        glDeleteBuffers(1, &m.vbo);
    if(m.vao && glIsVertexArray(m.vao))
        glDeleteVertexArrays(1, &m.vao);
    if(m.program && glIsProgram(m.program))
        glDeleteProgram(m.program);

    // if(m.vertShader)
    //     glDeleteShader(*m.vertShader);
    // if(m.fragShader)
    //     glDeleteShader(*m.fragShader);
}
