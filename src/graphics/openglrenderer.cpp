#include "openglrenderer.h"

#include <iostream>
#include "gfxcommon.h"

namespace gfx
{

OpenGLRenderer::OpenGLRenderer(int w, int h, float scale_factor)  :
    mWidth(w), mHeight(h), mScaleFactor(scale_factor),
    mGUITextShader(w, h)
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
    glLineWidth(2.2f*1.2f);
    glPointSize(2.2f*2.6f);

    //glLineWidth(1.2f);
    //glPointSize(2.6f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glFrontFace(GL_CW);

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

/*void OpenGLRenderer::preDrawUpdate()
{
    GFXTickJob job;
    bool popped = mGFXTickJobQueue.pop(job);
    if (popped)
    {
        std::cout << "executing GFX tick job" << std::endl;
        job();
    }
}*/

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

    mMainShader.clearDrawObjects(); // could/does this need to be optimized?
    mMainShader.clearLightObjects();

    drawSceneRecursive(scene_root, vmath::Matrix4::identity());

    mMainShader.drawDrawObjects(camera);

    glDisable(GL_DEPTH_TEST);
}

inline void OpenGLRenderer::drawSceneRecursive(const SceneNode &scene_node, const vmath::Matrix4 parent_transform) const
{
    vmath::Matrix4 mv_matrix = parent_transform * scene_node.transform.getTransformMatrix();

    // add scene object to drawing buffer
    for (const auto &scene_object : scene_node.getSceneObjects())
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

    // add lights to light buffer
    for (const auto &light : scene_node.getLights())
    {
        mMainShader.addLightObject(light);
    }

    for (const auto &child : scene_node.mChildren)
    {
        drawSceneRecursive(child, mv_matrix);
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

    //std::cout << "pre draw gui" << std::endl;
    drawGUI(gui_root);
    //std::cout << "post draw gui" << std::endl;
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

    drawGUIRecursive(gui_root, screen_space, (float)(mWidth)/mScaleFactor, (float)(mHeight)/mScaleFactor);

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

inline void OpenGLRenderer::drawGUIRecursive(const gui::GUINode &gui_node, vmath::Matrix4 parent_transform, float w_abs, float h_abs) const
{
    //vmath::Matrix4 mv = mGUIShader.drawGUINode(gui_node, parent_transform);
    //mGUITextShader.drawGUINodeText(gui_node, parent_transform);
    if (gui_node.isVisible())
    {
        vmath::Matrix4 mv = parent_transform * gui_node.getTransform().getTransformMatrix(w_abs, h_abs);

        //vmath::Matrix4 mv = parent_transform * gui_node.getTransform().getTransformMatrix();

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
                    //std::cout << "draw TextElement" << std::endl;
                    mGUITextShader.drawTextElement(child_element.get_const<gui::TextElement>(), mv);
                }
                break;
            case (gui::GUIElement::is_a<gui::BackgroundElement>::value):
                {
                    //std::cout << "draw BackgroundElement" << std::endl;
                    mGUIShader.drawBGElement(child_element.get_const<gui::BackgroundElement>(), mv);
                }
                break;
            case (gui::GUIElement::is_a<gui::ImageElement>::value):
                {
                    //std::cout << "draw ImageElement" << std::endl;
                    mGUIImageShader.drawImageElement(child_element.get_const<gui::ImageElement>(), mv);
                }
                break;
            case (gui::GUIElement::is_a<gui::SceneElement>::value):
                {
                    //std::cout << "draw SceneElement" << std::endl;
                    vmath::Vector4 bl = mv * vmath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
                    vmath::Vector4 tr = mv * vmath::Vector4(1.0f, 1.0f, 0.0f, 1.0f);
                    bl = bl + vmath::Vector4(1.0, 1.0, 0.0, 1.0f);
                    tr = tr + vmath::Vector4(1.0, 1.0, 0.0, 1.0f);
                    bl = vmath::Matrix4::scale(vmath::Vector3(mWidth/2.0f, mHeight/2.0f, 1.0f)) * bl;
                    tr = vmath::Matrix4::scale(vmath::Vector3(mWidth/2.0f, mHeight/2.0f, 1.0f)) * tr;

                    glViewport(std::max((int)(bl[0]), 1),       std::max((int)(tr[1]), 1),
                               std::max((int)(tr[0]-bl[0]), 1), std::max((int)(bl[1]-tr[1]),1)); // top right and bottom left get muddled by screen space matrix
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

        float this_w_abs = gui_node.getTransform().getSize().x.getRelative(w_abs) * w_abs;
        float this_h_abs = gui_node.getTransform().getSize().y.getRelative(h_abs) * h_abs;

        for (const gui::GUINode &child_node : gui_node.getChildren())
        {
            drawGUIRecursive(child_node, mv, this_w_abs, this_h_abs);
        }
    }
}

} // namespace gfx
