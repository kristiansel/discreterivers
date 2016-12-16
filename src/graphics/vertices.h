#ifndef VERTICES_H
#define VERTICES_H

#include <vector>
#include "../common/stdext.h"
#include "../common/gfx_primitives.h"
#include "gfxcommon.h"

namespace gfx {

struct Vertices
{
    inline explicit Vertices(const std::vector<vmath::Vector4> &position_data,
                      const std::vector<vmath::Vector4> &normal_data);

    inline explicit Vertices(const std::vector<vmath::Vector4> &position_data,
                      const std::vector<vmath::Vector4> &normal_data,
                      const std::vector<gfx::TexCoords> &texcoord_data);

    inline GLuint getVertexArrayObject() const       {return mVertexArrayObject;}
    inline GLuint getPositionArrayBuffer() const     {return mPositionArrayBuffer;}
    inline GLuint getNormalArrayBuffer() const       {return mNormalArrayBuffer;}
    inline GLuint getTexCoordArrayBuffer() const     {return mTexCoordArrayBuffer;}


private:
    Vertices();

    inline void init(const std::vector<vmath::Vector4> &position_data,
                const std::vector<vmath::Vector4> &normal_data,
                const std::vector<gfx::TexCoords> &texcoord_data);

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;
    GLuint mNormalArrayBuffer;
    GLuint mTexCoordArrayBuffer;
};

inline Vertices::Vertices(const std::vector<vmath::Vector4> &position_data,
                   const std::vector<vmath::Vector4> &normal_data)
{
    auto to_zero = [](const vmath::Vector4 &v) { return gfx::TexCoords{1.0f, 1.0f}; };
    init(position_data, normal_data, StdExt::vector_map<vmath::Vector4, gfx::TexCoords>(position_data, to_zero));
}

inline Vertices::Vertices(const std::vector<vmath::Vector4> &position_data,
                   const std::vector<vmath::Vector4> &normal_data,
                   const std::vector<gfx::TexCoords> &texcoord_data)
{
    init(position_data, normal_data, texcoord_data);
}

inline void Vertices::init(const std::vector<vmath::Vector4> &position_data,
            const std::vector<vmath::Vector4> &normal_data,
            const std::vector<gfx::TexCoords> &texcoord_data)
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

    // create normal buffer
    std::cout << "first texture coordinate: " << texcoord_data[0][0] << ", " << texcoord_data[0][1] << std::endl;
    mTexCoordArrayBuffer = 0;
    glGenBuffers(1, &mTexCoordArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, texcoord_data.size()*sizeof(gfx::TexCoords), &texcoord_data[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, sizeof(gfx::TexCoords)/sizeof(float), GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);


    checkOpenGLErrors("Vertices::Vertices");
}

}

#endif // VERTICES_H
