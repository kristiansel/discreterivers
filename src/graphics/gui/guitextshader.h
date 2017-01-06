#ifndef GUITEXTSHADER_H
#define GUITEXTSHADER_H

#include "../gfxcommon.h"
//#include "../primitives.h"
#include "guinode.h"
#include "guifontrenderer.h"

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

    inline void drawGUINodeText(const GUINode &gui_node, const vmath::Matrix4 &parent_transform) const;
    //inline void drawGUI(const std::vector<GUINode> &gui_nodes) const;

private:
    GLuint mShaderProgramID;

    Uniforms mUniforms;

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;
};

inline void GUITextShader::drawGUINodeText(const GUINode &gui_node, const vmath::Matrix4 &parent_transform) const
{
    //std::cout << "drawing gui text" << std::endl;
    glUseProgram(mShaderProgramID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    vmath::Matrix4 mv = vmath::Matrix4::identity();
    //vmath::Matrix4 mv = parent_transform;
    //vmath::print(parent_transform);

    vmath::Vector4 color = vmath::Vector4{0.0, 0.0, 0.0, 1.0};

    glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
    glUniform4fv(mUniforms.color, 1, (const GLfloat*)&(color));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gui_node.getFontAtlasTextureID());

    // Bind vertex array
    glBindVertexArray(gui_node.getGUITextVertices().getVertexArrayObject());

    glDrawArrays(GL_TRIANGLES, 0, 6*gui_node.getGUITextVertices().getNumCharacters());
}

} // namespace gui

} // namespace gfx

#endif // GUITEXTSHADER_H
