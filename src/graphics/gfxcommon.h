#ifndef COMMON_H
#define COMMON_H

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
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

}

#endif // COMMON_H
