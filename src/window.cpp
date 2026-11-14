#include "window.h"
#include "GLFW/glfw3.h"
#include "callbacks.h"
#include "log.h"
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <glad/gl.h>
#include <stb_image.h>

/* Constructor */
std::optional<Window> Window::create(std::string winName){
    GLFWwindow *window;
    std::vector<Rectangle> rectangles;
    GLuint vao;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIN_DEFAULT_WIDTH, WIN_DEFAULT_HEIGHT, winName.c_str(), nullptr, nullptr);
    if(!window){
        // free(name);
        glfwTerminate();
        return std::nullopt;
    }

    glfwSetKeyCallback(window, GLFWKeyCallback);
    // glfwSetFramebufferSizeCallback(window, GLFWRFrameBufferSizeCallback);

    glfwMakeContextCurrent(window);
    auto version = gladLoadGL(glfwGetProcAddress);
    if(!version){
        // free(name);
        glfwTerminate();
        return std::nullopt;
    }
    glfwSwapInterval(1);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,             0, nullptr, GL_FALSE);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH,   0, nullptr, GL_TRUE);

    // State state = State::create();

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint ebo;
    GLuint indicies[6] = { 0,1,2, 3,2,1 };
    glCreateBuffers(1, &ebo);
    glNamedBufferStorage(ebo, sizeof(indicies), indicies, 0);
    glVertexArrayElementBuffer(vao, ebo);

    glDrawBuffer(GL_BACK);
    glClearColor(1,0,0,1);

    return Window(M{
        .window     = std::move(window),
        .name       = std::move(winName),
        .rectangles = std::move(rectangles),
        .vao        = vao,
    });
}
/* Copy Constructor */
Window::Window(const Window& other) : m(other.m) {}
/* Copy Assignment Operator */
Window& Window::operator=(const Window& other) {
    if(this == &other)
        return *this;
    m = other.m;
    return *this;
}
/* Move Constructor */
Window::Window(Window&& other) noexcept
    : m(other.m)
{
    other.m.window  = nullptr;
    other.m.name    = {};
    // other.m.vbo     = 0;
    // other.m.ebo     = 0;
    other.m.vao     = 0;
    // other.m.program = 0;
}
/* Move Assignment Operator */
Window& Window::operator=(Window&& other) noexcept {
    m = other.m;

    other.m.window  = nullptr;
    other.m.name    = {};
    // other.m.vbo     = 0;
    // other.m.ebo     = 0;
    other.m.vao     = 0;
    // other.m.program = 0;
    return *this;
}

bool Window::addRectangle_NDC(float width, float height, float x, float y) {
    auto rect = Rectangle::create_ndc(m.vao, width, height, x, y);
    if(rect == std::nullopt)
        return false;
    m.rectangles.push_back(rect.value());
    return true;
}
// bool Window::addRectangle_Pixel(GLuint width, GLuint height, GLuint x, GLuint y) {
//     auto rect = Rectangle::create_pixels(m.vao, width, height, x, y);
//     if(rect == std::nullopt)
//         return false;
//     m.rectangles.push_back(rect.value());
//     return true;
// }

bool Window::addTexture(const char *texturePath) {
    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int width, height, numChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &numChannels, 0);
    if(data){
        GLenum dataFormat, internalFormat;
        switch(numChannels) {
            case 4:
                dataFormat = GL_RGBA;
                internalFormat = GL_RGBA8;
            break;
            case 3:
                dataFormat = GL_RGB;
                internalFormat = GL_RGB8;
            break;
            default:
                log_message("Error: Texture has too few color channels: {}", numChannels);
                return false;
            break;
        }
        glTextureStorage2D(texture, 1, internalFormat, width, height);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureSubImage2D(texture, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
        glBindTextureUnit(0, texture);
    } else {
        log_message("Error: Failed to load texture: {}", texturePath);
        return false;
    }
    stbi_image_free(data);
    return true;
}

bool Window::setRectangleShaders(GLuint rectIdx, const char *vertexShaderPath, const char *fragmentShaderPath) {
    m.rectangles[rectIdx].changeShaders(vertexShaderPath, fragmentShaderPath);
    return true;
}

const float vertex_data[] = {
     1.f,  1.f, 1.f, 1.f, // top right
     1.f, -1.f, 1.f, 0.f, // bottom right
    -1.f, -1.f, 0.f, 0.f, // bottom left
    -1.f,  1.f, 0.f, 1.f, // top left
};

GLuint index_data[] = { 0, 1, 2, 0, 2, 3 };

const float border_color[] = {1.f,1.f,0.f,1.f};

std::string readFileToString(const std::string& fname) {
    std::ifstream file(fname);
    if(!file.is_open()){
        log_message("Error: Failed to load shader file: {}", fname);
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    return ss.str();
}

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

// void Window::stage() {
//     glfwMakeContextCurrent(m.window);
//     log_message("{}: Staging.", m.name);
//
//     glCreateBuffers(1, &m.vbo);
//     if(glIsBuffer(m.vbo) == GL_FALSE){
//         std::cerr << "GL Error: " << glGetError() << std::endl;
//         log_message("Failed to initialize Vertex Buffer.");
//     }
//     glNamedBufferStorage(m.vbo, sizeof(vertex_data), vertex_data, 0);
//
//     glCreateBuffers(1, &m.ebo);
//     glNamedBufferStorage(m.ebo, sizeof(index_data), index_data, 0);
//
//     glCreateVertexArrays(1, &m.vao);
//     glVertexArrayElementBuffer(m.vao, m.ebo);
//     glVertexArrayVertexBuffer(m.vao, 0, m.vbo, 0, 4*sizeof(float));
//     glVertexArrayBindingDivisor(m.vao, 0, 0);
//
//     glVertexArrayAttribBinding(m.vao, 0, 0);
//     glVertexArrayAttribFormat(m.vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
//     glEnableVertexArrayAttrib(m.vao, 0);
//
//     glVertexArrayAttribBinding(m.vao, 1, 0);
//     glVertexArrayAttribFormat(m.vao, 1, 2, GL_FLOAT, GL_TRUE, 2*sizeof(float));
//     glEnableVertexArrayAttrib(m.vao, 1);
//
//     GLuint texture;
//     glCreateTextures(GL_TEXTURE_2D, 1, &texture);
//
//     glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//     glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//     int width, height, nrChannels;
//     stbi_set_flip_vertically_on_load(true);
//     unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
//     GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
//     if(data) {
//         glTextureStorage2D(texture, 1, GL_RGBA8, width, height);
//         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//         glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
//         glBindTextureUnit(0, texture);
//     } else {
//         log_message("Error: Failed to load texture: {}", "textures/container.jpg");
//     }
//     stbi_image_free(data);
//
//     GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, readFileToString("src/shaders/player_vertex.glsl").c_str());
//     GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, readFileToString("src/shaders/player_fragment.glsl").c_str());
//
//     m.program = glCreateProgram();
//     glAttachShader(m.program, vertex_shader);
//     glAttachShader(m.program, fragment_shader);
//     glLinkProgram(m.program);
//
//     // GLint success;
//     // glGetProgramiv(m.program, GL_LINK_STATUS, &success);
//     // if(!success) {
//     //     GLint length;
//     //     glGetProgramiv(m.program, GL_INFO_LOG_LENGTH, &length);
//     //     if(!length)
//     //         log_message("Failed to link program. No info log.");
//     //     else {
//     //         std::string info_log(length, 0);
//     //         glGetProgramInfoLog(m.program, length, nullptr, info_log.data());
//     //         log_message("Failed to link program. Info Log:\n\t{}", info_log);
//     //         std::cerr << "Failed to link program. Info Log:\n\t" << info_log << std::endl;
//     //     }
//     // }
//     //
//     // glDeleteShader(vertex_shader);
//     // glDeleteShader(fragment_shader);
//
//     glDrawBuffer(GL_BACK);
//     glClearColor(1,0,0,1);
//
//     glUseProgram(m.program);
//     glBindVertexArray(m.vao);
// }

void Window::render() {
    glfwMakeContextCurrent(m.window);

    int vx, vy;
    glfwGetFramebufferSize(m.window, &vx, &vy);
    GLsizei v = vx < vy ? vx : vy;
    GLint xl = (vx-v)/2;
    GLint yb = (vy-v)/2;
    glViewport(xl,yb,v,v);

    glClear(GL_COLOR_BUFFER_BIT);
    for(auto rect : m.rectangles) {
        rect.draw(m.vao);
    }
    // if(m.vao)
    //     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(m.window);
}

Window::~Window() {
    if(m.window)
        glfwDestroyWindow(m.window);
    // if(m.vbo && glIsBuffer(m.vbo))
    //     glDeleteBuffers(1, &m.vbo);
    // if(m.ebo && glIsBuffer(m.ebo))
    //     glDeleteBuffers(1, &m.ebo);
    if(m.vao && glIsVertexArray(m.vao))
        glDeleteVertexArrays(1, &m.vao);
    // if(m.program && glIsProgram(m.program))
    //     glDeleteProgram(m.program);
}
