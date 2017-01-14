#ifndef GUISHADER_H
#define GUISHADER_H

#include "../../gfxcommon.h"
#include "bgelement.h"

namespace gfx {

namespace gui {

class GUIShader {
public:
    GUIShader();
    ~GUIShader();

    GLuint getProgramID() const {return mShaderProgramID;}

    struct Uniforms;
    const Uniforms &getUniforms() const {return mUniforms;}

    struct Uniforms
    {
        GLint mv;
        GLint color;

    };

    inline void drawBGElement(const BackgroundElement &bg_element, const vmath::Matrix4 &transform) const;
private:
    GLuint mShaderProgramID;

    Uniforms mUniforms;

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;

};

inline void GUIShader::drawBGElement(const BackgroundElement &bg_element, const vmath::Matrix4 &transform) const
{

    glUseProgram(mShaderProgramID);

    vmath::Matrix4 mv = transform;

    glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
    glUniform4fv(mUniforms.color, 1, (const GLfloat*)&(bg_element.getColor()));

    glBindVertexArray(mVertexArrayObject);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

} // namespace gui

} // namespace gfx

#endif // GUISHADER_H
