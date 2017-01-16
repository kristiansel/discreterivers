#ifndef GUIImageShader_H
#define GUIImageShader_H

#include "../../gfxcommon.h"
#include "imageelement.h"

namespace gfx {

namespace gui {

class GUIImageShader {
public:
    GUIImageShader();
    ~GUIImageShader();

    GLuint getProgramID() const {return mShaderProgramID;}

    struct Uniforms;
    const Uniforms &getUniforms() const {return mUniforms;}

    struct Uniforms
    {
        GLint mv;
        GLint tex;
    };

    inline void drawImageElement(const ImageElement &img_element, const vmath::Matrix4 &transform) const;
private:
    GLuint mShaderProgramID;

    Uniforms mUniforms;

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;
    GLuint mTexCoordArrayBuffer;

};

inline void GUIImageShader::drawImageElement(const ImageElement &img_element, const vmath::Matrix4 &transform) const
{

    glUseProgram(mShaderProgramID);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    vmath::Matrix4 mv = transform;
    glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, img_element.getTextureID());

    glBindVertexArray(mVertexArrayObject);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

} // namespace gui

} // namespace gfx

#endif // GUIImageShader_H
