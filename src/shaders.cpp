#include <fstream>
#include <sstream>

#include "shaders.h"
#include "glad/gl.h"
#include "log.h"

std::string _readFile2String(const char *fname) {
    std::ifstream file(fname);
    if(!file.is_open()){
        log_message("Error: Failed to open shader file: {}", fname);
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    return ss.str();
}

GLuint _createShader(const char *path, GLenum type) {
    std::string str_src = _readFile2String(path);
    const char *src = str_src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // maxLength includes the null terminator
        std::string errorLog(maxLength, '\0');
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        log_message("Error::Shaders.create() - Failed to create {} shader.",
                    (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment");
        if(!maxLength)
            log_message(" -- Info Log Max Length is 0.");
        else
            log_message(" -- {}", errorLog);
        glDeleteShader(shader);
        return 0;
    } else
        log_message("Log::Shaders.create() - {} Shader at {} Compiled Successfully.",
                    (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment", path);
    return shader;
}
Shaders::Shaders(const Shaders& other)
    : m(other.m)
{}
Shaders& Shaders::operator=(const Shaders& other) {
    if(this == &other)
        return *this;
    m = other.m;
    return *this;
}
Shaders::Shaders(Shaders&& other) noexcept
    : m(other.m)
{
    other.m.programID = 0;
}
Shaders& Shaders::operator=(Shaders&& other) noexcept {
    m = other.m;
    other.m.programID = 0;
    return *this;
}

std::optional<Shaders> Shaders::create(const char *vertexShaderPath, const char *fragmentShaderPath) {
    auto vertex_shader = _createShader(vertexShaderPath, GL_VERTEX_SHADER);
    if(!vertex_shader)
        return std::nullopt;
    auto fragment_shader = _createShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
    if(!fragment_shader)
        return std::nullopt;

    auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    return std::move(Shaders(M {
        .programID = program,
    }));
}

void Shaders::use() noexcept {
    glUseProgram(m.programID);
}

bool Shaders::change(const char *vertexPath, const char *fragmentPath) {
    auto vertex_shader = _createShader(vertexPath, GL_VERTEX_SHADER);
    if(!vertex_shader) return false;
    auto fragment_shader = _createShader(fragmentPath, GL_FRAGMENT_SHADER);
    if(!fragment_shader) return false;
    glAttachShader(m.programID, vertex_shader);
    glAttachShader(m.programID, fragment_shader);
    glLinkProgram(m.programID);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDetachShader(m.programID, vertex_shader);
    glDetachShader(m.programID, fragment_shader);
    return true;
}

Shaders::~Shaders() {
    if(m.programID)
        glDeleteProgram(m.programID);
}
