#ifndef GUITEXTPRIMITIVES_H
#define GUITEXTPRIMITIVES_H

#include <vector>
#include "../gfxcommon.h"
#include "../../common/stdext.h"
#include "../../common/gfx_primitives.h"
#include "../../common/resmanager/refcounted.h"

namespace gfx {

namespace gui {

class GUITextVertices : public Resource::RefCounted<GUITextVertices>
{
public:
    inline explicit GUITextVertices(const std::vector<vmath::Vector4> &position_data,
                                    const std::vector<gfx::TexCoords> &texcoord_data);

    inline GLuint getVertexArrayObject() const       {return mVertexArrayObject;}
    inline GLuint getPositionArrayBuffer() const     {return mPositionArrayBuffer;}
    inline GLuint getTexCoordArrayBuffer() const     {return mTexCoordArrayBuffer;}
    inline int getNumCharacters() const              {return mNumCharacters;}

    inline void updateTexCoordArrayBuffer( const gfx::TexCoords * tex_coords_ptr, unsigned int size );

// used by Resource::RefCounted<TextVertices>
    inline void resourceDestruct();

private:
    GUITextVertices();

    inline void init(const std::vector<vmath::Vector4> &position_data,
                     const std::vector<gfx::TexCoords> &texcoord_data);

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;
    GLuint mTexCoordArrayBuffer;

    int mNumCharacters;
};


inline GUITextVertices::GUITextVertices(const std::vector<vmath::Vector4> &position_data,
                   const std::vector<gfx::TexCoords> &texcoord_data) :
    mNumCharacters(position_data.size())
{
    init(position_data, texcoord_data);
}

inline void GUITextVertices::init(const std::vector<vmath::Vector4> &position_data,
            const std::vector<gfx::TexCoords> &texcoord_data)
{
    DEBUG_ASSERT(position_data.size()==texcoord_data.size());
    // potentially more optimizations:
    //
    // http://stackoverflow.com/questions/27027602/glvertexattribpointer-gl-invalid-operation-invalid-vao-vbo-pointer-usage
    //

    // Vertex Array Object
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    // Prepare buffer data
    GLsizeiptr num_vertices = position_data.size();

    const GLfloat *points = (GLfloat *)&position_data[0];
    GLsizeiptr num_points = num_vertices;
    GLsizeiptr point_buffer_size = num_points*sizeof(vmath::Vector4);

    // create position buffer
    mPositionArrayBuffer = 0;
    glGenBuffers(1, &mPositionArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mPositionArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, point_buffer_size, points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    // create normal buffer
    //std::cout << "first texture coordinate: " << texcoord_data[0][0] << ", " << texcoord_data[0][1] << std::endl;
    mTexCoordArrayBuffer = 0;
    glGenBuffers(1, &mTexCoordArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, texcoord_data.size()*sizeof(gfx::TexCoords), &texcoord_data[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, sizeof(gfx::TexCoords)/sizeof(float), GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);

    checkOpenGLErrors("TextVertices::TextVertices");
}

inline void GUITextVertices::resourceDestruct()
{
    //std::cout << "deleting text vertices: " << mVertexArrayObject << std::endl;
    glDeleteBuffers(1, &mTexCoordArrayBuffer);
    glDeleteBuffers(1, &mPositionArrayBuffer);
    glDeleteVertexArrays(1, &mVertexArrayObject);
}

inline void GUITextVertices::updateTexCoordArrayBuffer( const gfx::TexCoords * tex_coords_ptr, unsigned int size )
{
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(gfx::TexCoords), tex_coords_ptr, GL_DYNAMIC_DRAW);
}

} // namespace gui

} // namespace gfx


#endif // GUITEXTPRIMITIVES_H
