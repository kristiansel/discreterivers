#include "openglrenderer.h"

#include <iostream>

namespace gfx
{

OpenGLRenderer::OpenGLRenderer(int width, int height)
{
    // GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

    // opengl hello triangle
    glEnable(GL_DEPTH_TEST); // Is this necessary?
    glDepthFunc(GL_LESS); // Is this necessary?

    glLineWidth(1.2f);
    glPointSize(2.6f);

    // set up shaders
    const char * vertex_shader =
    "#version 400\n"

    "layout(location = 0) in vec4 vertex_position;"
    "layout(location = 1) in vec4 vertex_normal;"
    "layout(location = 2) in vec2 vertex_texture_coordinates;"

    "out vec4 position;"
    "out vec4 normal;"

    "uniform mat4 mvp;"

    "void main() {"
    "  position = mvp * vec4(vertex_position.xyz, 1.0);"
    "  normal = mvp * vec4(vertex_normal.xyz, 0.0);"
    "  gl_Position = position;"
    "}";

    const char * fragment_shader =
    "#version 400\n"

    "in vec4 position;"
    "in vec4 normal;"

    "out vec4 frag_color;"

    "uniform vec4 color;"

    "void main() {"
    //"  vec3 eyedirn = normalize(vec3(0,0,0) - position) ;"
    "  vec3 lightdirection = normalize(vec3(1.0, 1.0, -1.0));"
    "  float nDotL = dot(normal.xyz, lightdirection);"
    "  frag_color = vec4(color.rgb * max(nDotL, 0), 1.0);"
    "}";

    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);

    // should check compilation error

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fragment_shader, NULL);
    glCompileShader (fs);

    // should check compilation error

    mShaderProgramID = glCreateProgram ();
    glAttachShader (mShaderProgramID, fs);
    glAttachShader (mShaderProgramID, vs);
    glLinkProgram (mShaderProgramID);

    // should check linker error

    mUniforms.mvp = glGetUniformLocation(mShaderProgramID, "mvp") ;
    mUniforms.color = glGetUniformLocation(mShaderProgramID, "color") ;

    // set up camera
    // camera
    // projection matrix
    mCamera.mProjectionMatrix = vmath::Matrix4::perspective(
        M_PI_2,                         // field of view (radians)
        (float)(width)/(float)(height), // aspect ratio
        0.1f,                           // znear
        100.0f                          // zfar
    );

    vmath::Matrix4 camera_matrix = vmath::Matrix4::translation(vmath::Vector3(0.0f, 0.0f, 5.0f));
    mCamera.mCamMatrixInverse = vmath::inverse(camera_matrix);

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
                                         const Transform &transform_in)
{
    sceneobject_id id_out(mSceneObjects.size());
    mSceneObjects.emplace_back(transform_in, material, geometry);

    return SceneObjectHandle(this, id_out);
}

SceneObject * SceneNode::getSceneObjectPtr(sceneobject_id id)
{
    return &mSceneObjects[int(id)];
}

Vertices::Vertices(const std::vector<vmath::Vector4> &position_data,
                   const std::vector<vmath::Vector4> &normal_data)
{
    GLsizeiptr num_vertices = position_data.size();

    const GLfloat *points = (GLfloat *)&position_data[0];
    GLsizeiptr num_points = num_vertices;
    GLsizeiptr point_buffer_size = num_points * sizeof(vmath::Vector4);

    // create position buffer
    mPositionArrayBuffer = 0;
    glGenBuffers (1, &mPositionArrayBuffer);
    glBindBuffer (GL_ARRAY_BUFFER, mPositionArrayBuffer);
    glBufferData (GL_ARRAY_BUFFER, point_buffer_size, points, GL_STATIC_DRAW);
    glEnableVertexAttribArray (0);

    // create normal buffer
    mNormalArrayBuffer = 0;
    glGenBuffers (1, &mNormalArrayBuffer);
    glBindBuffer (GL_ARRAY_BUFFER, mNormalArrayBuffer);
    glBufferData (GL_ARRAY_BUFFER, normal_data.size()*sizeof(vmath::Vector4), &normal_data[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray (1);
}

Geometry::Geometry(const Vertices &vertices, const Primitives &primitives) :
    mVertices(vertices), mPrimitives(primitives) {}

inline void OpenGLRenderer::drawDrawObject(const DrawObject &draw_object, const Camera &camera, const Uniforms &uniforms)
{
    // deconstruct scene_object
    // wouldn't it be nice to write for ({transform, {vertices, primitives}, material} : mSceneObjectsVector)... std::tie?
    const auto &model_matrix = draw_object.mMatrix;
    const auto &geometry = draw_object.mGeometry;
    const auto &material = draw_object.mMaterial;

    // deconstruct geometry
    const auto &vertices = geometry.getVertices();
    const auto &primitives = geometry.getPrimitives();

    vmath::Matrix4 model_view_matrix = camera.mCamMatrixInverse * model_matrix;

#ifdef _VECTORMATH_DEBUG
    //vmath::print(model_matrix, ("model_matrix"+std::to_string(debug_counter)).c_str());
#endif

    // combined model view projection matrix
    vmath::Matrix4 mvp = camera.mProjectionMatrix * model_view_matrix;

//#ifdef _VECTORMATH_DEBUG
//    vmath::print(mvp, "mvp");
//#endif

    glUniformMatrix4fv(uniforms.mvp, 1, false, (const GLfloat*)&(mvp[0]));
    glUniform4fv(uniforms.color, 1, (const GLfloat*)&material.getColor());

    //glBindVertexArray (vertices.vao);
    glBindBuffer (GL_ARRAY_BUFFER, vertices.getPositionArrayBuffer());
    glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer (GL_ARRAY_BUFFER, vertices.getNormalArrayBuffer());
    glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    // pretty sure the indices also need to be bound
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitives.getElementArrayBuffer());

    // wireframe !
    if (material.getWireframe()==true) // woops branching in tight loop, should fix...
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

    //                                                  | num indices | type of index | wtf is this for?
    glDrawElements(PRIMITIVE_GL_CODE(primitives.getPrimitiveType()), primitives.getNumIndices(), GL_UNSIGNED_INT, (void*)0 );
}

void OpenGLRenderer::draw() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram (mShaderProgramID);

    mDrawObjectsVector.clear(); // could/does this need to be optimized?
    for (const auto &scene_node : mSceneNodesVector)
    {
        for (const auto &scene_object : scene_node.getSceneObjects())
        {
            vmath::Matrix4 world_matrix = scene_node.transform.getTransformMatrix() * scene_object.mTransform.getTransformMatrix();
            mDrawObjectsVector.emplace_back(

                DrawObject(world_matrix, scene_object.mMaterial, scene_object.mGeometry)
            );
        }
    }

    for (const auto &draw_object : mDrawObjectsVector)
    {
        drawDrawObject(draw_object, mCamera, mUniforms);
    }


}

} // namespace gfx
