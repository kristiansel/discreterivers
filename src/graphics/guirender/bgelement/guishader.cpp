#include "guishader.h"

namespace gfx {

namespace gui {
GUIShader::GUIShader()
{
    // set up shaders
    const char * vertex_shader_src =
    "#version 410\n"

    "layout(location = 0) in vec4 vertex_position;"

    "uniform mat4 mv;"

    "void main() {"
    "   gl_Position = mv * vertex_position;"
    "}";

    const char * fragment_shader_src =
    "#version 410\n"

    "out vec4 frag_color;"

    "uniform vec4 color;"

    "void main() {"
    "  frag_color = color;"
    "}";

    std::cout << "compiling shaders" << std::endl;

    mShaderProgramID = createProgramFromShaders(vertex_shader_src, fragment_shader_src);

    // get uniform locations
    glUseProgram(mShaderProgramID);

    mUniforms.color = glGetUniformLocation(mShaderProgramID, "color") ;
    mUniforms.mv = glGetUniformLocation(mShaderProgramID, "mv") ;

    // Check for errors:
    common:checkOpenGLErrors("GUIShader::GUIShader()");


    // create the rectangle buffer used as instance for all rectangular objects
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    const GLfloat vertices[] = {
        0.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };

    glGenBuffers(1, &mPositionArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mPositionArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

GUIShader::~GUIShader()
{
    std::cout << "deleting shader: " << mShaderProgramID << std::endl;
    glDeleteProgram(mShaderProgramID);
}

} // namespace gui

} // namespace gfx
