#ifndef GUISHADER_H
#define GUISHADER_H

#include "../gfxcommon.h"
#include "guinode.h"
#include "../primitives.h"

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
        GLint tex;
        GLint color;

    };

    inline void drawGUI(const std::vector<GUINode> &gui_nodes) const;

    inline vmath::Matrix4 drawGUINode(const GUINode &gui_node, vmath::Matrix4 parent_transform = vmath::Matrix4::identity()) const;

    inline void drawRecursive(const GUINode &gui_node, vmath::Matrix4 parent_transform = vmath::Matrix4::identity()) const;

private:
    GLuint mShaderProgramID;

    Uniforms mUniforms;

    //Primitives mRectPrimitives;

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;
    GLuint mTexCoordArrayBuffer;

};

inline vmath::Matrix4 GUIShader::drawGUINode(const GUINode &gui_node, vmath::Matrix4 parent_transform) const
{
    vmath::Matrix4 mv = parent_transform * gui_node.getTransform().getTransformMatrix();

    glUniformMatrix4fv(mUniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
    glUniform4fv(mUniforms.color, 1, (const GLfloat*)&(gui_node.getColor()));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gui_node.getTextureID());

    glBindVertexArray(mVertexArrayObject);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    return mv;
}

inline void GUIShader::drawRecursive(const GUINode &gui_node, vmath::Matrix4 parent_transform) const
{
    vmath::Matrix4 mv = drawGUINode(gui_node, parent_transform);

    for (const GUINode &child_node : gui_node.getChildren())
    {
        drawRecursive(child_node, mv);
    }
}

inline void GUIShader::drawGUI(const std::vector<GUINode> &gui_nodes) const
{
    glUseProgram(mShaderProgramID);
    glDisable(GL_DEPTH_TEST);

    vmath::Matrix4 screen_space = GUITransform::getScreenSpaceTransform();

    for (const auto &gui_node : gui_nodes)
    {
        drawRecursive(gui_node, screen_space);
    }

    glEnable(GL_DEPTH_TEST);
}

} // namespace gui

} // namespace gfx

#endif // GUISHADER_H
