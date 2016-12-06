#ifndef SHADER_H
#define SHADER_H

#include "gfxcommon.h"

#include "material.h"
#include "geometry.h"
#include "transform.h"

#include "camera.h"
#include "renderflags.h"

namespace gfx {

class Shader {
public:
    Shader();

    GLuint getProgramID() const {return mShaderProgramID;}

    struct Uniforms;
    const Uniforms &getUniforms() const {return mUniforms;}

    struct Uniforms
    {
        GLint mv;
        GLint p;
        GLint tex;
        GLint color;
        GLint light_position;
        GLint light_color;
    };

    inline void clearDrawObjects() const { mDrawObjectsVector.clear(); }

    inline void addDrawObject(const vmath::Matrix4 &world_matrix, const Material &material,
                              const Geometry &geometry, RenderFlags render_flags) const
    {
         mDrawObjectsVector.emplace_back(world_matrix, material, geometry, render_flags);
    }

    inline void drawDrawObjects( const Camera &camera ) const
    {
        // actually draw the batched draw objects
        for (const auto &draw_object : mDrawObjectsVector)
        {
            drawDrawObject(draw_object, camera, false);
        }
    }

private:
    class DrawObject
    {
    public:
        DrawObject(const vmath::Matrix4 &matrix, const Material &material,
                   const Geometry &geometry, RenderFlags render_flags) :
            mMatrix(matrix), mMaterial(material), mGeometry(geometry), mRenderFlags(render_flags) {}

        DrawObject(vmath::Matrix4 &&matrix, Material &&material, Geometry &&geometry,
                   RenderFlags &&render_flags) :
            mMatrix(std::move(matrix)), mMaterial(std::move(material)), mGeometry(std::move(geometry)),
            mRenderFlags(render_flags) {}

        vmath::Matrix4 mMatrix;
        Material mMaterial;
        Geometry mGeometry;
        RenderFlags mRenderFlags;

    private:
        DrawObject();
    };

    inline void drawDrawObject(const DrawObject &draw_object, const Camera &camera, bool global_wireframe = false) const;

    GLuint mShaderProgramID;
    Uniforms mUniforms;
    mutable std::vector<DrawObject> mDrawObjectsVector;
};

// inline functions

inline void Shader::drawDrawObject(const DrawObject &draw_object, const Camera &camera, bool global_wireframe) const
{
    // deconstruct scene_object
    // wouldn't it be nice to write for ({transform, {vertices, primitives}, material} : mSceneObjectsVector)... std::tie?
    const auto &model_matrix = draw_object.mMatrix;
    const auto &geometry = draw_object.mGeometry;
    const auto &material = draw_object.mMaterial;

    // deconstruct geometry
    const auto &vertices = geometry.getVertices();
    const auto &primitives = geometry.getPrimitives();

    vmath::Matrix4 mv = camera.getCamMatrixInverse() * model_matrix;

    // combined model view projection matrix
    vmath::Matrix4 p = camera.mProjectionMatrix;

    glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
    glUniformMatrix4fv(mUniforms.p, 1, false, (const GLfloat*)&(p[0]));
    glUniform4fv(mUniforms.color, 1, (const GLfloat*)&material.getColor());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.getTexture().getTextureID());

    // Bind vertex array
    glBindVertexArray(vertices.getVertexArrayObject());

    // Bind element array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitives.getElementArrayBuffer());

    // wireframe !
    if (draw_object.mRenderFlags.checkFlag(RenderFlags::Wireframe)) // woops branching in tight loop, should fix...
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

    checkOpenGLErrors("Before draw elements");
    //                                                  | num indices | type of index | wtf is this for?
    glDrawElements(PRIMITIVE_GL_CODE(primitives.getPrimitiveType()), primitives.getNumIndices(), GL_UNSIGNED_INT, (void*)0 );

    checkOpenGLErrors("After draw elements");
}


}

#endif // SHADER_H
