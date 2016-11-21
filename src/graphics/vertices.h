#ifndef VERTICES_H
#define VERTICES_H

#include "vector"
#include "../common/gfx_primitives.h"
#include "gfxcommon.h"

namespace gfx {

struct Vertices
{
    explicit Vertices(const std::vector<vmath::Vector4> &position_data,
                      const std::vector<vmath::Vector4> &normal_data);

    inline GLuint getVertexArrayObject() const       {return mVertexArrayObject;}
    inline GLuint getPositionArrayBuffer() const     {return mPositionArrayBuffer;}
    inline GLuint getNormalArrayBuffer() const       {return mNormalArrayBuffer;}


private:
    Vertices();

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;
    GLuint mNormalArrayBuffer;
};


inline Vertices::Vertices(const std::vector<vmath::Vector4> &position_data,
                   const std::vector<vmath::Vector4> &normal_data)
{
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
    mNormalArrayBuffer = 0;
    glGenBuffers(1, &mNormalArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, normal_data.size()*sizeof(vmath::Vector4), &normal_data[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);


    checkOpenGLErrors("Vertices::Vertices");
}

}

#endif // VERTICES_H
