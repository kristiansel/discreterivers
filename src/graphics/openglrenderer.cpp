#include "openglrenderer.h"

#include <iostream>
#include "gfxcommon.h"

namespace gfx
{

OpenGLRenderer::OpenGLRenderer(int w, int h)  :
    mWidth(w), mHeight(h),
    mGUITextShader(w, h)/*,
    mGUIFont("res/fonts/IMFePIrm28P.ttf", 24)*/
{
    // OpenGL context needs to be valid at this point

    // needs GLEW INIT!!!
    gfx::checkOpenGLErrors("stenc1");

    GLint stencilSize = 0;
    glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER,
        GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencilSize);
    std::cout << "OpenGL CORE stencil buffer bit depth: " << stencilSize << std::endl;

    GLint depthSize = 0;
    glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER,
        GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depthSize);
    std::cout << "OpenGL CORE depth buffer bit depth: " << depthSize << std::endl;
    //gfx::checkOpenGLErrors("stenc3");


    // depth
    glEnable(GL_DEPTH_TEST); // Is this necessary?
    glDepthFunc(GL_LESS); // Is this necessary?

    // stencil
    //glEnable(GL_STENCIL_TEST);
    //glStencilFunc(GL_EQUAL, 1, 0xFF);


    // Hi DPI
    //glLineWidth(2.2f*1.2f);
    //glPointSize(2.2f*2.6f);

    glLineWidth(1.2f);
    glPointSize(2.6f);

    glCullFace(GL_BACK);

    resize(w, h);

    // Check for errors:
    common:checkOpenGLErrors("OpenGLRenderer::OpenGLRenderer");

}

void OpenGLRenderer::resize(int w, int h)
{
    mWidth = w;
    mHeight = h;
    mGUITextShader.resize(w, h);
    glViewport(0, 0, w, h);
}

void OpenGLRenderer::preDrawUpdate()
{
    GFXTickJob job;
    bool popped = mGFXTickJobQueue.pop(job);
    if (popped)
    {
        std::cout << "executing GFX tick job" << std::endl;
        job();
    }
}

inline void OpenGLRenderer::drawScene(const Camera &camera, const SceneNode &scene_root) const
{
    glEnable(GL_DEPTH_TEST);
    // switch shader, (might be done later at material stage...)
    glUseProgram (mMainShader.getProgramID());

    // prepare lights
    /*mLightObjectsVector.clear();
    for (const auto &scene_node : mSceneRoot.mChildren)
    {
        for (const auto &light : scene_node.getLights())
        {
            vmath::Vector4 light_world_pos = scene_node.transform.getTransformMatrix() * light.mTransform.position;
            mLightObjectsVector.emplace_back(

                LightObject{light_world_pos, light.mColor}
            );
        }
    }*/

    vmath::Vector4 light_position_world;
    vmath::Vector4 light_color;
    if (mLightObjectsVector.size()>0)
    {
        // use first light
        light_position_world = mLightObjectsVector[0].mPosition;
        light_color = mLightObjectsVector[0].mColor;
    }
    else // default light
    {
        light_position_world = vmath::Vector4(10.0f, 10.0f, 10.0f, 0.0f);
        light_color = vmath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    vmath::Matrix4 vp_matrix = camera.getCamMatrixInverse();
    vmath::Vector4 light_position = vp_matrix * light_position_world;

    glUniform4fv(mMainShader.getUniforms().light_position, 1, (const GLfloat*)&light_position);
    glUniform4fv(mMainShader.getUniforms().light_color, 1, (const GLfloat*)&light_color);

    mMainShader.clearDrawObjects(); // could/does this need to be optimized?

    drawSceneRecursive(scene_root, vmath::Matrix4::identity());


    mMainShader.drawDrawObjects(camera);


    glDisable(GL_DEPTH_TEST);
}

inline void OpenGLRenderer::drawSceneRecursive(const SceneNode &scene_root, const vmath::Matrix4 parent_transform) const
{
    vmath::Matrix4 mv_matrix = parent_transform * scene_root.transform.getTransformMatrix();

    for (const auto &scene_object : scene_root.getSceneObjects())
    {
        RenderFlags so_rflags = scene_object.getRenderFlags();
        if (!so_rflags.checkFlag(RenderFlags::Hidden)) // woops branching in tight loop, optimize me please!
        {
            RenderFlags combined_flags = RenderFlags::combine(mRenderFlags, so_rflags);

            mMainShader.addDrawObject(vmath::Matrix4(mv_matrix), // make lvalue
                                      scene_object.mMaterial.getDrawData(),
                                      scene_object.mGeometry.getDrawData(),
                                      combined_flags);
        }
    }

    for (const auto &scene_node : scene_root.mChildren)
    {
        drawSceneRecursive(scene_node, mv_matrix);
    }
}


void OpenGLRenderer::draw(const Camera &camera, const gui::GUINode &gui_root) const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update mutable global uniforms.
    // blabla... update etc...
    // like camera
    // lights (forward renderer)
    // wind direction
    // all that stuff that can be shared by shader programs...

    drawScene(camera, mSceneRoot);

    drawGUI(gui_root);
}


inline void OpenGLRenderer::drawGUI(const gui::GUINode &gui_root) const
{
    // render the gui
    // TODO: Complete this

    // drawing gui should not be affected by what is currently in the depth and stencil buffers
    glDisable(GL_DEPTH_TEST); // this should be enabled for own gui z buffer? Should perhaps just clear here? or sort nodes on same level by z?
    //glDisable(GL_STENCIL_TEST); // probably move this


    // -----PRE GUI DRAW STAGE-----------------------------------------------------
/*
    // disable writing to color and depth buffers
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    // set stencil writing stuff
    //glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
    //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //glStencilMask(0xFF); // Write to stencil buffer

    glStencilFunc(GL_NEVER, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)

    // draw stencil pattern
    glStencilMask(0xFF);
*/
    // -END-PRE GUI DRAW STAGE-----------------------------------------------------

    //glClear(GL_STENCIL_BUFFER_BIT);

    vmath::Matrix4 screen_space = gui::GUITransform::getScreenSpaceTransform();

    drawGUIRecursive(gui_root, screen_space);

    // -----POST GUI DRAW STAGE-----------------------------------------------------
/*
    // re-enable writing to color and depth buffers
    // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    // glDepthMask(GL_TRUE);

    // reset stencil writing stuff
    // glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
    // glStencilMask(0x00); // Don't write anything to stencil buffer


    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glStencilMask(0x00);
    // draw where stencil's value is 0
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    // (nothing to draw)
    // draw only where stencil's value is 1
    glStencilFunc(GL_EQUAL, 1, 0xFF);
*/
    // -END-POST GUI DRAW STAGE-----------------------------------------------------

    glEnable(GL_DEPTH_TEST); // probably move this
    //glEnable(GL_STENCIL_TEST); // probably move this
}

inline void OpenGLRenderer::drawGUIRecursive(const gui::GUINode &gui_node, vmath::Matrix4 parent_transform) const
{
    //vmath::Matrix4 mv = mGUIShader.drawGUINode(gui_node, parent_transform);
    //mGUITextShader.drawGUINodeText(gui_node, parent_transform);
    if (gui_node.isVisible())
    {
        vmath::Matrix4 mv = parent_transform * gui_node.getTransform().getTransformMatrix();

        for (const gui::GUIElement &child_element : gui_node.getElements())
        {
            /*std::cout << "drawing elements" << std::endl;

            std::cout << "type_index of element: " << child_element.get_type() << std::endl;
            std::cout << "type_index of TextElement: " << gui::GUIElement::is_a<gui::TextElement>::value << std::endl;
            std::cout << "type_index of BackGroundElement: " << gui::GUIElement::is_a<gui::BackGroundElement>::value << std::endl;*/

            switch (child_element.get_type())
            {
            case (gui::GUIElement::is_a<gui::TextElement>::value):
                // render text
                {
                    vmath::Vector3 transl = mv.getTranslation();
                    gui::GUITransform::Position pos = { float(transl[0]), float(transl[1]) };
                    mGUITextShader.drawTextElement(child_element.get_const<gui::TextElement>(), pos);
                }
                break;
            case (gui::GUIElement::is_a<gui::BackgroundElement>::value):
                {
                    mGUIShader.drawBGElement(child_element.get_const<gui::BackgroundElement>(), mv);
                }
                break;
            case (gui::GUIElement::is_a<gui::ImageElement>::value):
                {
                    mGUIImageShader.drawImageElement(child_element.get_const<gui::ImageElement>(), mv);
                }
                break;
            case (gui::GUIElement::is_a<gui::SceneElement>::value):
                {
                    vmath::Vector4 bl = mv * vmath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
                    vmath::Vector4 tr = mv * vmath::Vector4(1.0f, 1.0f, 0.0f, 1.0f);
                    bl = bl + vmath::Vector4(1.0, 1.0, 0.0, 1.0f);
                    tr = tr + vmath::Vector4(1.0, 1.0, 0.0, 1.0f);
                    bl = vmath::Matrix4::scale(vmath::Vector3(mWidth/2.0f, mHeight/2.0f, 1.0f)) * bl;
                    tr = vmath::Matrix4::scale(vmath::Vector3(mWidth/2.0f, mHeight/2.0f, 1.0f)) * tr;

                    glViewport(bl[0], tr[1], tr[0]-bl[0], bl[1]-tr[1]); // top right and bottom left get muddled by screen space matrix
                    //std::cout << "in SceneElement draw: " << std::endl;
                    //vmath::print(bl);
                    //vmath::print(tr);
                    //glViewport(400, 200, 200, 100);
                    const gui::SceneElement &se = child_element.get_const<gui::SceneElement>();
                    drawScene(se.getCameraConst(), se.getSceneRootConst());
                    glViewport(0, 0, mWidth, mHeight);
                }
                break;
            default:
                break;
                // do nothing
            }
        }

        for (const gui::GUINode &child_node : gui_node.getChildren())
        {
            drawGUIRecursive(child_node, mv);
        }
    }
}

} // namespace gfx
