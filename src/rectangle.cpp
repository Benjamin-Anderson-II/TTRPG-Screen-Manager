#include "rectangle.h"
#include "glad/gl.h"
#include <string>
#include <sstream>
#include <fstream>
#include "log.h"

std::string _readFile2String(std::string fname) {
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

std::optional<Rectangle> Rectangle::create_ndc(GLuint vao, float width, float height, float x, float y) {
    GLuint vbo;
    GLuint bindingIdx = 0;
    GLuint vertex_shader, fragment_shader;
    GLuint program;
    glCreateBuffers(1, &vbo);
    float vertices[16] = {
        x,       y,        0, 0,
        x+width, y,        1, 0,
        x,       y+height, 0, 1,
        x+width, y+height, 1, 1,
    };
    // bind vbo to vao
    glNamedBufferStorage(vbo, sizeof(vertices), vertices, 0);
    glVertexArrayVertexBuffer(vao, bindingIdx, vbo, 0, 4*sizeof(float));
    glVertexArrayBindingDivisor(vao, bindingIdx, 0);

    // enable vertex indicies
    glVertexArrayAttribBinding(vao, 0, bindingIdx);
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(vao, 0);

    // enable texture indicies
    glVertexArrayAttribBinding(vao, 1, bindingIdx);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_TRUE, 2*sizeof(float));
    glEnableVertexArrayAttrib(vao, 1);

    const char *vertexShaderPath = "src/shaders/rectangle_default_vertex.glsl";
    const char *fragmentShaderPath = "src/shaders/rectangle_default_fragment.glsl";
    auto shaders = Shaders::create(vertexShaderPath, fragmentShaderPath);
    if(shaders == std::nullopt) {
        log_message("Error::Rectangle.create() - Unable to create_shaders");
        return std::nullopt;
    }

    return std::move(Rectangle(M {
        .vbo = vbo,
        .bindingIdx = bindingIdx,
        .isNDC = true,
        .width = width,
        .height = height,
        .x = x,
        .y = y,
        .shaders = std::move(shaders.value()),
    }));
}
Rectangle::Rectangle(const Rectangle& other)
    : m(other.m)
{}
Rectangle& Rectangle::operator=(const Rectangle& other) {
    m = other.m;
    return *this;
}
Rectangle::Rectangle(Rectangle&& other) noexcept
    : m(std::move(other.m))
{
    other.m.vbo = 0;
    other.m.bindingIdx = 0;
    other.m.x = 0;
    other.m.y = 0;
    other.m.width = 0;
    other.m.height = 0;
    other.m.isNDC = false;
}
Rectangle& Rectangle::operator=(Rectangle&& other) noexcept {
    m = std::move(other.m);

    other.m.vbo = 0;
    other.m.bindingIdx = 0;
    other.m.x = 0;
    other.m.y = 0;
    other.m.width = 0;
    other.m.height = 0;
    other.m.isNDC = false;
    return *this;
}

bool Rectangle::changeShaders(const char *vertexShaderPath, const char *fragmentShaderPath) {
    if(!m.shaders.change(vertexShaderPath, fragmentShaderPath))
        return false;
    return true;
}

bool Rectangle::draw(GLuint vao) {
    m.shaders.use();
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    return true;
}
