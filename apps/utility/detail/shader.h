#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

namespace detail
{
    void compile(GLuint handle, const char* source, const int size)
    {
        GLint compileResult = 0;

        glShaderSource(handle, 1, &source, &size);
        glCompileShader(handle);
        glGetShaderiv(handle, GL_COMPILE_STATUS, &compileResult);

        if(compileResult == GL_FALSE)
        {
            GLint infoLength = 0;
            std::string shaderLog;

            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLength);
            shaderLog.resize(static_cast<std::size_t>(infoLength));
            glGetShaderInfoLog(handle, infoLength, nullptr, &shaderLog[0]);

            std::cerr << shaderLog << std::endl;
            std::cerr.flush();

            throw std::runtime_error("[Shader] ERROR compiling shader:\n" + shaderLog);
        }
    }

    void link(GLuint handle)
    {
        glLinkProgram(handle);

        GLint result;
        glGetProgramiv(handle, GL_LINK_STATUS, &result);

        if(result == GL_FALSE)
        {
            GLint messageLength = 0;
            std::string programLog;

            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &messageLength);

            if(messageLength > 0)
            {
                programLog.resize(static_cast<std::size_t>(messageLength));
                glGetProgramInfoLog(handle, messageLength, nullptr, &programLog[0]);

                std::cerr << programLog << std::endl;
                std::cerr.flush();
            }

            throw std::runtime_error((std::string("[Shader] ERROR link shaderprogram: \n") + programLog));
        }
    }
}

GLuint shaderCreate(const std::string &vertexSource, const std::string &fragmentSource)
{
    GLuint program = glCreateProgram();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    if(!program || !vertex_shader || !frag_shader)
    {
        throw std::runtime_error("[Shader] Couldn't create shader program!");
    }

    detail::compile(vertex_shader, vertexSource.c_str(), vertexSource.size());
    glAttachShader(program, vertex_shader);

    detail::compile(frag_shader, fragmentSource.c_str(), fragmentSource.size());
    glAttachShader(program, frag_shader);

    detail::link(program);

    glDetachShader(program, vertex_shader);
    glDetachShader(program, frag_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);

    return program;
}
