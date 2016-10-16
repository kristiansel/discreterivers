#include "openglrenderer.h"

#include <iostream>

namespace gfx
{

OpenGLRenderer::OpenGLRenderer(int width, int height) : mGlobalWireframe(false)
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

    // Hi DPI
    //glLineWidth(2.2f*1.2f);
    //glPointSize(2.2f*2.6f);

    glLineWidth(1.2f);
    glPointSize(2.6f);

    glCullFace(GL_BACK);

    // set up shaders
    const char * vertex_shader_src =
    "#version 430\n"

    "layout(location = 0) in vec4 vertex_position;"
    "layout(location = 1) in vec4 vertex_normal;"
    "layout(location = 2) in vec2 vertex_texture_coordinates;"

    "out vec4 position;"
    "out vec4 normal;"

    "uniform mat4 mv;"
    "uniform mat4 p;"

    "void main() {"
    "  position = mv * vec4(vertex_position.xyz, 1.0);"
    "  normal = mv * vec4(vertex_normal.xyz, 0.0);"
    "  gl_Position = p * position;"
    "}";

    const char * fragment_shader_src =
    "#version 430\n"

    "in vec4 position;"
    "in vec4 normal;"

    "out vec4 frag_color;"

    "uniform vec4 color;"
    "uniform vec4 light_position;"
    "uniform vec4 light_color;"

    "void main() {"
    //"  vec3 eyedirn = normalize(vec3(0,0,0) - position) ;"
    //"  vec3 lightdirection = normalize(vec3(1.0, 1.0, -1.0));"
    "  vec4 lightdirection = light_position - position;"
    "  float nDotL = dot(normal.xyz, normalize(lightdirection.xyz));"
    "  frag_color = vec4(color.rgb * max(nDotL, 0), 1.0);"
    "}";

    std::cout << "compiling shaders" << std::endl;

    // compile vertex shader
    GLuint vertex_shader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader (vertex_shader);

    checkShaderCompiled(vertex_shader);

    // compile fragment shader
    GLuint fragment_shader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader (fragment_shader);

    checkShaderCompiled(fragment_shader);

    std::cout << "linking shaders" << std::endl;

    mShaderProgramID = glCreateProgram ();
    glAttachShader (mShaderProgramID, fragment_shader);
    glAttachShader (mShaderProgramID, vertex_shader);
    glLinkProgram (mShaderProgramID);

    // should check linker error
    checkProgramLinked(mShaderProgramID);

    //glUseProgram(mShaderProgramID);r

    mUniforms.mv = glGetUniformLocation(mShaderProgramID, "mv") ;
    mUniforms.p = glGetUniformLocation(mShaderProgramID, "p") ;
    mUniforms.color = glGetUniformLocation(mShaderProgramID, "color") ;
    mUniforms.light_position = glGetUniformLocation(mShaderProgramID, "light_position") ;
    mUniforms.light_color = glGetUniformLocation(mShaderProgramID, "light_color") ;

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
    mCamera.mProjectionMatrix = vmath::Matrix4::orthographic(
        -aspect_ratio*half_height, aspect_ratio*half_height, -half_height, half_height, 0.1f, 100.0f
    );


    vmath::Matrix4 camera_matrix = vmath::Matrix4::translation(vmath::Vector3(0.0f, 0.0f, 10.0f));
    mCamera.mCamMatrixInverse = vmath::inverse(camera_matrix);

    // Check for errors:
    checkOpenGLErrors("OpenGLRenderer::OpenGLRenderer");

}


bool OpenGLRenderer::checkShaderCompiled(GLuint shader)
{
    GLint shader_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);
    if(shader_compiled == GL_FALSE)
    {
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        std::vector<GLchar> info_log(max_length);
        glGetShaderInfoLog(shader, max_length, &max_length, &info_log[0]);

        for (auto err_char : info_log) std::cout << err_char;

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        // glDeleteShader(shader); // Don't leak the shader.
        return false;
    }
    else
    {
        return true;
    }
}

void checkOpenGLErrors(const std::string &error_check_label)
{
    GLenum gl_err = GL_NO_ERROR;
    while((gl_err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error (" << error_check_label << "): " << gl_err << std::endl;
    }
}

bool OpenGLRenderer::checkProgramLinked(GLuint program)
{
    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if(is_linked == GL_FALSE)
    {
        GLint max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        //The maxLength includes the NULL character
        std::vector<GLchar> info_log(max_length);
        glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);

        for (auto info_char : info_log) std::cout << info_char;

        return false;
    }
    else
    {
        return true;
    }
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

Vertices::Vertices(const std::vector<vmath::Vector4> &position_data,
                   const std::vector<vmath::Vector4> &normal_data)
{
    // potentially more optimizations:
    /*
     * http://stackoverflow.com/questions/27027602/glvertexattribpointer-gl-invalid-operation-invalid-vao-vbo-pointer-usage
     */

    // Vertex Array Object
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    // Prepare buffer data
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
    glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    // create normal buffer
    mNormalArrayBuffer = 0;
    glGenBuffers (1, &mNormalArrayBuffer);
    glBindBuffer (GL_ARRAY_BUFFER, mNormalArrayBuffer);
    glBufferData (GL_ARRAY_BUFFER, normal_data.size()*sizeof(vmath::Vector4), &normal_data[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray (1);
    glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);


    checkOpenGLErrors("Vertices::Vertices");
}

Geometry::Geometry(const Vertices &vertices, const Primitives &primitives) :
    mVertices(vertices), mPrimitives(primitives) {}

inline void OpenGLRenderer::drawDrawObject(const DrawObject &draw_object, const Camera &camera, const Uniforms &uniforms, bool global_wireframe)
{
    // deconstruct scene_object
    // wouldn't it be nice to write for ({transform, {vertices, primitives}, material} : mSceneObjectsVector)... std::tie?
    const auto &model_matrix = draw_object.mMatrix;
    const auto &geometry = draw_object.mGeometry;
    const auto &material = draw_object.mMaterial;

    // deconstruct geometry
    const auto &vertices = geometry.getVertices();
    const auto &primitives = geometry.getPrimitives();

    vmath::Matrix4 mv = camera.mCamMatrixInverse * model_matrix;

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

    //glBindVertexArray (vertices.vao);
    // glBindBuffer (GL_ARRAY_BUFFER, vertices.getPositionArrayBuffer());
    // glBindBuffer (GL_ARRAY_BUFFER, vertices.getNormalArrayBuffer());

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



    //                                                  | num indices | type of index | wtf is this for?
    glDrawElements(PRIMITIVE_GL_CODE(primitives.getPrimitiveType()), primitives.getNumIndices(), GL_UNSIGNED_INT, (void*)0 );


}

void OpenGLRenderer::draw() const
{
    // switch shader, (might be done later at material stage...)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram (mShaderProgramID);

    // prepare lights
    mLightObjectsVector.clear();
    for (const auto &scene_node : mSceneNodesVector)
    {
        for (const auto &light : scene_node.getLights())
        {
            vmath::Vector4 light_world_pos = scene_node.transform.getTransformMatrix() * light.mTransform.position;
            mLightObjectsVector.emplace_back(

                LightObject(light_world_pos, light.mColor)
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
    vmath::Matrix4 vp_matrix = mCamera.mCamMatrixInverse;
    vmath::Vector4 light_position = vp_matrix * light_position_world;

    glUniform4fv(mUniforms.light_position, 1, (const GLfloat*)&light_position);
    glUniform4fv(mUniforms.light_color, 1, (const GLfloat*)&light_color);

    // prepare the drawobjects
    mDrawObjectsVector.clear(); // could/does this need to be optimized?
    for (const auto &scene_node : mSceneNodesVector)
    {
        for (const auto &scene_object : scene_node.getSceneObjects())
        {
            if (scene_object.mMaterial.getVisible()) // woops branching in tight loop, optimize me please!
            {
                vmath::Matrix4 world_matrix = scene_node.transform.getTransformMatrix() * scene_object.mTransform.getTransformMatrix();
                mDrawObjectsVector.emplace_back(

                    DrawObject(world_matrix, scene_object.mMaterial, scene_object.mGeometry)
                );
            }
        }
    }

    // actually draw the batched draw objects
    for (const auto &draw_object : mDrawObjectsVector)
    {
        drawDrawObject(draw_object, mCamera, mUniforms, mGlobalWireframe);
    }


}

} // namespace gfx
