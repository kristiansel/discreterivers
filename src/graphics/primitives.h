#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "gfxcommon.h"
#include "../common/resmanager/refcounted.h"

namespace gfx {

struct primitive_type_tag{};
typedef decltype(GL_TRIANGLES) PRIMITIVE_GL_CODE;
typedef ID<primitive_type_tag, PRIMITIVE_GL_CODE, GL_TRIANGLES> gl_primitive_type;

class Primitives : public Resource::RefCounted<Primitives>
{
public:
    template<class PrimitiveType>
    explicit Primitives(const std::vector<PrimitiveType> &primitive_data);

    inline GLuint getElementArrayBuffer() const         { return mElementArrayBuffer; }
    inline GLsizeiptr getNumIndices() const             { return mNumIndices; }
    inline gl_primitive_type getPrimitiveType() const   { return mPrimitiveType; }

    // used by Resource::RefCounted<Primitives>
    inline void resourceDestruct();

private:
    Primitives();

    GLuint mElementArrayBuffer; // Pointer type, Primitives is not a POD
    GLsizeiptr mNumIndices;
    gl_primitive_type mPrimitiveType;
};

template<class PrimitiveType>
Primitives::Primitives(const std::vector<PrimitiveType> &primitives_data)
{
    const GLuint *indices = (GLuint *)&primitives_data[0];
    mNumIndices = primitives_data.size() * sizeof(PrimitiveType);

    glGenBuffers(1, &mElementArrayBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementArrayBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices, indices, GL_STATIC_DRAW);

    std::cout << "generating primitives: mElementArrayBuffer = " << mElementArrayBuffer << std::endl;

    gl_primitive_type gl_primitive =
        std::is_same<PrimitiveType, gfx::Line>::value   ?       // if
            gl_primitive_type(GL_LINES)     :
        std::is_same<PrimitiveType, gfx::Point>::value  ?       // else if
            gl_primitive_type(GL_POINTS)    :
     // std::is_same<PrimitiveType, gfx::Triangle>::value ?     // else
            gl_primitive_type(GL_TRIANGLES);

    mPrimitiveType = gl_primitive;
}

inline void Primitives::resourceDestruct()
{
    std::cout << "deleting primitives: " << mElementArrayBuffer << std::endl;
    glDeleteBuffers(1, &mElementArrayBuffer);
}

} // namespace gfx

#endif // PRIMITIVES_H
