#include "window.h"
#include "GLFW/glfw3.h"
#include "callbacks.h"
#include "log.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <glad/gl.h>
#include <stb_image.h>

/* Constructor */
std::optional<Window> Window::create(std::string winName){
    GLFWwindow *window;
    GLuint vao;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIN_DEFAULT_WIDTH, WIN_DEFAULT_HEIGHT, winName.c_str(), nullptr, nullptr);
    if(!window){
        glfwTerminate();
        return std::nullopt;
    }

    glfwSetKeyCallback(window, GLFWKeyCallback);
    glfwSetFramebufferSizeCallback(window, GLFWFrameBufferSizeCallback);

    glfwMakeContextCurrent(window);
    auto version = gladLoadGL(glfwGetProcAddress);
    if(!version){
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

    // TODO: Create State structure
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
Window::Window(Window&& other) noexcept {
    m.window     = other.m.window;
    m.vao        = other.m.vao;
    m.name       = std::move(other.m.name);
    m.rectangles = std::move(other.m.rectangles);
    m.shaders    = std::move(other.m.shaders);

    other.m.window     = nullptr;
    other.m.vao        = 0;
}
/* Move Assignment Operator */
Window& Window::operator=(Window&& other) noexcept {
    m.window     = other.m.window;
    m.vao        = other.m.vao;
    m.name       = std::move(other.m.name);
    m.rectangles = std::move(other.m.rectangles);
    m.shaders    = std::move(other.m.shaders);

    other.m.window     = nullptr;
    other.m.vao        = 0;
    return *this;
}

bool Window::addRectangle_NDC(float width, float height, float x, float y, Shader *shader) {
    auto rect = Rectangle::create_ndc(m.vao, width, height, x, y, shader);
    if(rect == std::nullopt)
        return false;
    m.rectangles.push_back(std::move(rect.value()));
    return true;
}
// bool Window::addRectangle_Pixel(GLuint width, GLuint height, GLuint x, GLuint y) {
//     auto rect = Rectangle::create_pixels(m.vao, width, height, x, y);
//     if(rect == std::nullopt)
//         return false;
//     m.rectangles.push_back(std::move(rect.value()));
//     return true;
// }

bool Window::addShader(const char *vertPath, const char *fragPath) {
    auto shader = Shader::create(vertPath, fragPath);
    if(shader == std::nullopt)
        return false;
    m.shaders.push_back(std::move(shader.value()));
    return true;
}

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

Rectangle& Window::rectangleAt(GLuint idx) {
    // TODO: type checking
    return m.rectangles[idx];
}
Shader& Window::shaderAt(GLuint idx) {
    // TODO: type checking
    return m.shaders[idx];
}

void Window::render() {
    glfwMakeContextCurrent(m.window);

    glClear(GL_COLOR_BUFFER_BIT);
    for(Rectangle& rect : m.rectangles) {
        rect.draw(m.vao);
    }

    glfwSwapBuffers(m.window);
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(m.window);
}

Window::~Window() {
    if(m.window)
        glfwDestroyWindow(m.window);
    if(m.vao && glIsVertexArray(m.vao))
        glDeleteVertexArrays(1, &m.vao);
}
