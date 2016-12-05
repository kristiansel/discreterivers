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

OpenGLRenderer::OpenGLRenderer() : mGlobalWireframe(false)
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

    // Check for errors:
    common:checkOpenGLErrors("OpenGLRenderer::OpenGLRenderer");

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
    mSceneObjects.emplace_back(transform, material, geometry);

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


inline void OpenGLRenderer::drawDrawObject(const DrawObject &draw_object, const Camera &camera, const Shader::Uniforms &uniforms, bool global_wireframe)
{
    // deconstruct scene_object
    // wouldn't it be nice to write for ({transform, {vertices, primitives}, material} : mSceneObjectsVector)... std::tie?
    const auto &model_matrix = draw_object.mMatrix;
    const auto &geometry = draw_object.mGeometry;
    const auto &material = draw_object.mMaterial;

    // deconstruct geometry
    const auto &vertices = geometry.getVertices();
    const auto &primitives = geometry.getPrimitives();

    vmath::Matrix4 mv = camera.getCamMatrixInverse() * model_matrix;

#ifdef _VECTORMATH_DEBUG
    //vmath::print(model_matrix, ("model_matrix"+std::to_string(debug_counter)).c_str());
#endif

    // combined model view projection matrix
    vmath::Matrix4 p = camera.mProjectionMatrix;

//#ifdef _VECTORMATH_DEBUG
//    vmath::print(mvp, "mvp");
//#endif

    glUniformMatrix4fv(uniforms.mv, 1, false, (const GLfloat*)&(mv[0]));
    glUniformMatrix4fv(uniforms.p, 1, false, (const GLfloat*)&(p[0]));
    glUniform4fv(uniforms.color, 1, (const GLfloat*)&material.getColor());

    checkOpenGLErrors("Draw before texture");

    glActiveTexture(GL_TEXTURE0);

    std::cout << "texture id: " << material.getTexture().getTextureID() << std::endl;

    glBindTexture(GL_TEXTURE_2D, material.getTexture().getTextureID());

    checkOpenGLErrors("Draw after texture");


    // Bind vertex array
    glBindVertexArray(vertices.getVertexArrayObject());

    // Bind element array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitives.getElementArrayBuffer());

    // wireframe !
    if (material.getWireframe()==true || global_wireframe) // woops branching in tight loop, should fix...
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

    checkOpenGLErrors("Before draw elements");

    //                                                  | num indices | type of index | wtf is this for?
    glDrawElements(PRIMITIVE_GL_CODE(primitives.getPrimitiveType()), primitives.getNumIndices(), GL_UNSIGNED_INT, (void*)0 );


    checkOpenGLErrors("After draw elements");
    //assert(false);
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
    glUseProgram (mShaderProgram.getProgramID());

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

    glUniform4fv(mShaderProgram.getUniforms().light_position, 1, (const GLfloat*)&light_position);
    glUniform4fv(mShaderProgram.getUniforms().light_color, 1, (const GLfloat*)&light_color);

    // prepare the drawobjects
    mDrawObjectsVector.clear(); // could/does this need to be optimized?
    for (const auto &scene_node : mSceneNodesVector)
    {
        for (const auto &scene_object : scene_node.getSceneObjects())
        {
            if (scene_object.mMaterial.getVisible()) // woops branching in tight loop, optimize me please!
            {
                vmath::Matrix4 world_matrix = scene_node.transform.getTransformMatrix()/* * scene_object.mTransform.getTransformMatrix()*/;
                mDrawObjectsVector.emplace_back(world_matrix, scene_object.mMaterial, scene_object.mGeometry);
            }
        }
    }

    // actually draw the batched draw objects
    for (const auto &draw_object : mDrawObjectsVector)
    {
        drawDrawObject(draw_object, camera, mShaderProgram.getUniforms(), mGlobalWireframe);
    }
}

} // namespace gfx
