#ifndef GUITEXTSHADER_H
#define GUITEXTSHADER_H

#include "../gfxcommon.h"
//#include "../primitives.h"
#include "guinode.h"
#include "guifontrenderer.h"
#include "textelement/textelement.h"

namespace gfx {

namespace gui {

class GUITextShader {
public:
    GUITextShader();
    ~GUITextShader();

    GLuint getProgramID() const { return mShaderProgramID; }

    struct Uniforms;
    const Uniforms &getUniforms() const { return mUniforms; }

    struct Uniforms
    {
        GLint mv;
        GLint tex;
        GLint color;
    };

    inline void drawTextElement(const TextElement &text_element, const GUITransform::Position &pos) const;

private:
    GLuint mShaderProgramID;

    Uniforms mUniforms;

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;
};

inline void GUITextShader::drawTextElement(const TextElement &text_element, const GUITransform::Position &pos) const
{
    //std::cout << "drawing gui text" << std::endl;
    glUseProgram(mShaderProgramID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //vmath::Matrix4 mv = vmath::Matrix4::identity();
    vmath::Matrix4 mv = vmath::Matrix4::translation(vmath::Vector3(pos[0], pos[1], 0.0f));

    vmath::Vector4 color = text_element.getColor();

    glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
    glUniform4fv(mUniforms.color, 1, (const GLfloat*)&(color));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text_element.getFontAtlasTextureID());

    // Bind vertex array
    glBindVertexArray(text_element.getGUITextVertices().getVertexArrayObject());

    glDrawArrays(GL_TRIANGLES, 0, 6*text_element.getGUITextVertices().getNumCharacters());
}


} // namespace gui

} // namespace gfx

#endif // GUITEXTSHADER_H
