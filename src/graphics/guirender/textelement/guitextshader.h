#ifndef GUITEXTSHADER_H
#define GUITEXTSHADER_H

#include "../../gfxcommon.h"
#include "../guitransform.h"
#include "../guifont.h"
#include "textelement.h"

namespace gfx {

namespace gui {

class GUITextShader {
public:
    GUITextShader(int w, int h);
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

    inline void drawTextElement(const TextElement &text_element, const vmath::Matrix4 &pos) const;

    inline void resize(int w, int h);

private:
    /*struct {
        int width;
        int height;
    } mOriginalResolution;*/

    vmath::Matrix4 mRescaleMatrix;
    GLuint mShaderProgramID;
    Uniforms mUniforms;
};

inline void GUITextShader::drawTextElement(const TextElement &text_element, const vmath::Matrix4 &mv_in) const
{
    //std::cout << "drawing gui text" << std::endl;
    glUseProgram(mShaderProgramID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //vmath::Matrix4 mv = vmath::Matrix4::identity();
    vmath::Vector3 transl = mv_in.getTranslation();
    vmath::Matrix4 mv = vmath::Matrix4::translation(vmath::Vector3(transl[0], transl[1], 0.0f)) * mRescaleMatrix;

    vmath::Vector4 color = text_element.getColor();

    glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
    glUniform4fv(mUniforms.color, 1, (const GLfloat*)&(color));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text_element.getFontAtlasTextureID());

    // Bind vertex array
    glBindVertexArray(text_element.getGUITextVertices().getVertexArrayObject());

    glDrawArrays(GL_TRIANGLES, 0, text_element.getGUITextVertices().getNumVertices());
}


inline void GUITextShader::resize(int w, int h)
{
    float inv_width_scale = float(GUIFont::StdResolution::width)/float(w);
    float inv_height_scale = float(GUIFont::StdResolution::height)/float(h);
    mRescaleMatrix = vmath::Matrix4::scale(vmath::Vector3{inv_width_scale, inv_height_scale, 1.0f});
}


} // namespace gui

} // namespace gfx

#endif // GUITEXTSHADER_H
