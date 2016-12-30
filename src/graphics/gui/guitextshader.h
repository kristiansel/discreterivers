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
    };

    inline void drawGUINodeText(const GUINode &gui_node, const vmath::Matrix4 &parent_transform) const;
    //inline void drawGUI(const std::vector<GUINode> &gui_nodes) const;

    inline GUITextVertices bakeGUIText(std::string &&text) { return mGUIFontRenderer.bakeGUIText(std::move(text)); }

private:
    GLuint mShaderProgramID;

    Uniforms mUniforms;

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;

    GUIFontRenderer mGUIFontRenderer;
};

/*inline void GUITextShader::drawGUI(const std::vector<GUINode> &gui_nodes) const
{
    glUseProgram(mShaderProgramID);
    glDisable(GL_DEPTH_TEST);

    for (const auto &gui_node : gui_nodes)
    {
        vmath::Matrix4 mv = gui_node.getTransform().getTransformMatrix();

        glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
        glUniform4fv(mUniforms.color, 1, (const GLfloat*)&(gui_node.getColor()));

        glBindVertexArray(mVertexArrayObject);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glEnable(GL_DEPTH_TEST);
}*/

inline void GUITextShader::drawGUINodeText(const GUINode &gui_node, const vmath::Matrix4 &parent_transform) const
{
    //std::cout << "drawing gui text" << std::endl;
}

} // namespace gui

} // namespace gfx

#endif // GUITEXTSHADER_H
