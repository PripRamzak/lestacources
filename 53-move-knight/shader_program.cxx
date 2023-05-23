#include "shader_program.hxx"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "glad/glad.h"

void gl_check()
{
    const int error = static_cast<int>(glGetError());
    if (error != GL_NO_ERROR)
    {
        switch (error)
        {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM error" << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE error" << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION error" << std::endl;
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION error"
                          << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY error" << std::endl;
                break;
        }
        assert(false);
    }
}

bool shader_program::create()
{
    program = glCreateProgram();
    gl_check();
    if (program == 0)
    {
        std::cerr << "Create program error" << std::endl;
        return false;
    }

    glAttachShader(program, vertex_shader);
    gl_check();
    glAttachShader(program, fragment_shader);
    gl_check();

    glLinkProgram(program);
    gl_check();

    GLint linked_status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked_status);
    gl_check();
    if (linked_status == 0)
    {
        GLint info_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_length);
        gl_check();
        std::vector<char> info_log(static_cast<size_t>(info_length));
        glGetProgramInfoLog(program, info_length, nullptr, info_log.data());
        gl_check();

        std::cerr << "Error while linking program: " << info_log.data();

        glDeleteShader(vertex_shader);
        gl_check();
        glDeleteShader(fragment_shader);
        gl_check();
        glDeleteProgram(program);
        gl_check();
        return false;
    }

    glUseProgram(program);
    gl_check();

    glDeleteShader(vertex_shader);
    gl_check();
    glDeleteShader(fragment_shader);
    gl_check();

    glEnable(GL_BLEND);
    gl_check();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl_check();

    return true;
}

unsigned int shader_program::get_program()
{
    return program;
}

bool shader_program::create_shader(const char* file_path, shader_type type)
{
    GLuint shader;
    if (type == shader_type::vertex)
    {
        shader = glCreateShader(GL_VERTEX_SHADER);
        gl_check();
        vertex_shader = shader;
    }
    else if (type == shader_type::fragment)
    {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        gl_check();
        fragment_shader = shader;
    }

    std::stringstream ss;
    std::ifstream     vertex_shader_file;
    std::string       vertex_shader_src;
    const char*       src_code;

    vertex_shader_file.exceptions(std::ifstream::failbit);
    try
    {
        vertex_shader_file.open(file_path);
        ss << vertex_shader_file.rdbuf();
        vertex_shader_file.close();
    }
    catch (std::ifstream::failure)
    {
        std::cerr << "Exception opening/reading/closing file" << std::endl;
        return false;
    }
    vertex_shader_src = ss.str();
    src_code          = vertex_shader_src.c_str();

    glShaderSource(shader, 1, &src_code, nullptr);
    gl_check();
    glCompileShader(shader);
    gl_check();

    GLint compiled_status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled_status);
    gl_check();
    if (compiled_status == 0)
    {
        GLint info_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
        gl_check();
        std::vector<char> info_log(static_cast<size_t>(info_length));
        glGetShaderInfoLog(shader, info_length, nullptr, info_log.data());
        gl_check();
        glDeleteShader(shader);
        gl_check();

        std::cerr << "Error while compiling shader\nSourceCode:\n"
                  << vertex_shader_src << std::endl
                  << info_log.data();
        return false;
    }

    return true;
}

void shader_program::delete_program()
{
    glDeleteProgram(program);
    gl_check();
    glDeleteShader(vertex_shader);
    gl_check();
    glDeleteShader(fragment_shader);
    gl_check();
}