#include "openglrenderer.h"

#include <iostream>
#include "gfxcommon.h"

namespace gfx
{

Camera::Camera(int width, int height)
{
    // set up camera
    // camera
    // projection matrix
    float aspect_ratio = (float)(width)/(float)(height);
//    mCamera.mProjectionMatrix = vmath::Matrix4::perspective(
//        M_PI_4,                         // field of view (radians)
//        aspect_ratio,                   // aspect ratio
//        0.1f,                           // znear
//        100.0f                          // zfar
//    );

    float half_height = 3.5f;


    mProjectionMatrix = vmath::Matrix4::orthographic(
        -aspect_ratio*half_height, aspect_ratio*half_height, -half_height, half_height, 0.1f, 100.0f
    );



    /*vmath::Matrix4 camera_matrix = vmath::Matrix4::translation(vmath::Vector3(0.0f, 0.0f, 10.0f));
    mCamMatrixInverse = vmath::inverse(camera_matrix);*/
}

OpenGLRenderer::OpenGLRenderer(int w, int h)  :
    mGUIFontRenderer("res/fonts/IMFePIrm28P.ttf", 24)
{
    // OpenGL context needs to be valid at this point

    glEnable(GL_DEPTH_TEST); // Is this necessary?
    glDepthFunc(GL_LESS); // Is this necessary?

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
    glViewport(0, 0, w, h);
}

SceneNodeHandle OpenGLRenderer::addSceneNode()
{
    scenenode_id id_out(mSceneNodesVector.size());
    mSceneNodesVector.emplace_back(SceneNode());

    return SceneNodeHandle(this, id_out);
}

SceneNode * OpenGLRenderer::getSceneNodePtr(scenenode_id id)
{
    return &mSceneNodesVector[int(id)];
}

SceneObjectHandle SceneNode::addSceneObject(const Geometry &geometry,
                                         const Material &material,
                                         const Transform &transform)
{
    sceneobject_id id_out(mSceneObjects.size());
    mSceneObjects.emplace_back(material, geometry);
    return SceneObjectHandle(this, id_out);
}

SceneObject * SceneNode::getSceneObjectPtr(sceneobject_id id)
{
    return &mSceneObjects[int(id)];
}


LightHandle SceneNode::addLight(const vmath::Vector4 &color,
                     const Transform &transform)
{
    light_id id_out(mLights.size());
    mLights.emplace_back(transform, color);

    return LightHandle(this, id_out);

}

void OpenGLRenderer::draw(const Camera &camera) const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update mutable global uniforms.
    // blabla... update etc...
    // like camera
    // lights (forward renderer)
    // wind direction
    // all that stuff that can be shared by shader programs...

    // switch shader, (might be done later at material stage...)
    glUseProgram (mMainShader.getProgramID());

    // prepare lights
    mLightObjectsVector.clear();
    for (const auto &scene_node : mSceneNodesVector)
    {
        for (const auto &light : scene_node.getLights())
        {
            vmath::Vector4 light_world_pos = scene_node.transform.getTransformMatrix() * light.mTransform.position;
            mLightObjectsVector.emplace_back(

                LightObject{light_world_pos, light.mColor}
            );
        }
    }

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

    // prepare the drawobjects
    mMainShader.clearDrawObjects(); // could/does this need to be optimized?
    for (const auto &scene_node : mSceneNodesVector)
    {
        for (const auto &scene_object : scene_node.getSceneObjects())
        {
            RenderFlags so_rflags = scene_object.getRenderFlags();
            if (!so_rflags.checkFlag(RenderFlags::Hidden)) // woops branching in tight loop, optimize me please!
            {
                RenderFlags combined_flags = RenderFlags::combine(mRenderFlags, so_rflags);

                mMainShader.addDrawObject(scene_node.transform.getTransformMatrix(),
                                          scene_object.mMaterial.getDrawData(),
                                          scene_object.mGeometry.getDrawData(),
                                          combined_flags);
            }
        }
    }

    mMainShader.drawDrawObjects(camera);

    // render the gui
    // TODO: Complete this

    glDisable(GL_DEPTH_TEST);

    vmath::Matrix4 screen_space = gui::GUITransform::getScreenSpaceTransform();

    for (const auto &gui_node : mGUINodesVector)
    {
        drawGUIRecursive(gui_node, screen_space);
    }

    glEnable(GL_DEPTH_TEST);
}


inline void OpenGLRenderer::drawGUIRecursive(const gui::GUINode &gui_node, vmath::Matrix4 parent_transform) const
{
    vmath::Matrix4 mv = mGUIShader.drawGUINode(gui_node, parent_transform);
    mGUITextShader.drawGUINodeText(gui_node, parent_transform);

    for (const gui::GUINode &child_node : gui_node.getChildren())
    {
        drawGUIRecursive(child_node, mv);
    }
}

} // namespace gfx
