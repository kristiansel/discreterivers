#ifndef COMMON_H
#define COMMON_H

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <vector>
#include <array>
#include "../common/typetag.h"

namespace gfx {

inline void checkOpenGLErrors(const std::string &error_check_label)
{
    GLenum gl_err = GL_NO_ERROR;
    while((gl_err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error (" << error_check_label << "): " << gl_err << std::endl;
    }
}

inline bool checkShaderCompiled(GLuint shader)
{
    GLint shader_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);
    if(shader_compiled == GL_FALSE)
    {
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        std::vector<GLchar> info_log(max_length);
        glGetShaderInfoLog(shader, max_length, &max_length, &info_log[0]);

        for (auto err_char : info_log) std::cout << err_char;

        // glDeleteShader(shader); // Don't leak the shader.
        return false;
    }
    else
    {
        return true;
    }
}


inline bool checkProgramLinked(GLuint program)
{
    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if(is_linked == GL_FALSE)
    {
        GLint max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        //The maxLength includes the NULL character
        std::vector<GLchar> info_log(max_length);
        glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);

        for (auto info_char : info_log) std::cout << info_char;

        return false;
    }
    else
    {
        return true;
    }
}


inline GLuint createProgramFromShaders(const char * vertex_shader_src, const char * fragment_shader_src)
{
    // compile vertex shader
    GLuint vertex_shader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader (vertex_shader);

    checkShaderCompiled(vertex_shader);

    // compile fragment shader
    GLuint fragment_shader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader (fragment_shader);

    checkShaderCompiled(fragment_shader);

    std::cout << "linking shaders" << std::endl;

    GLuint shader_program = glCreateProgram ();
    glAttachShader (shader_program, fragment_shader);
    glAttachShader (shader_program, vertex_shader);
    glLinkProgram (shader_program);

    // should check linker error
    checkProgramLinked(shader_program);

    // shaders are copied into the program, so time to
    // clean up shaders
    glDetachShader(shader_program, vertex_shader);
    glDetachShader(shader_program, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}


}

#endif // COMMON_H
