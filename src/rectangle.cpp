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

    // Vertex Shader
    std::string str_vertex_src = _readFile2String("src/shaders/rectangle_default_vertex.glsl");
    const char *vertex_src = str_vertex_src.c_str();
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_src, nullptr);
    glCompileShader(vertex_shader);

    // Fragment Shader
    std::string str_fragment_src = _readFile2String("src/shaders/rectangle_default_fragment.glsl");
    const char *fragment_src = str_fragment_src.c_str();
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_src, nullptr);
    glCompileShader(fragment_shader);

    // Link Shaders to Program
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return Rectangle(M {
        .vbo = vbo,
        .bindingIdx = bindingIdx,
        .isNDC = true,
        .width = width,
        .height = height,
        .x = x,
        .y = y,
        .program = program,
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
    });
}

bool Rectangle::changeShaders(const char *vertexShaderPath, const char *fragmentShaderPath) {
    std::string str_vertex_src = _readFile2String(vertexShaderPath);
    const char *vertex_src = str_vertex_src.c_str();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_src, nullptr);
    glCompileShader(vertex_shader);

    std::string str_fragment_src = _readFile2String(fragmentShaderPath);
    const char *fragment_src = str_fragment_src.c_str();
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_src, nullptr);
    glCompileShader(fragment_shader);
    glDetachShader(m.program, m.vertex_shader);
    glDetachShader(m.program, m.fragment_shader);
    glAttachShader(m.program, vertex_shader);
    glAttachShader(m.program, fragment_shader);
    glLinkProgram(m.program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return true;
}

bool Rectangle::draw(GLuint vao) {
    glUseProgram(m.program);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    return true;
}
