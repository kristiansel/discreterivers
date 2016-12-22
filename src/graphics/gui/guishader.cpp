#include "guishader.h"

namespace gfx {

namespace gui {
GUIShader::GUIShader()
{
    // set up shaders
    const char * vertex_shader_src =
    "#version 330\n"

    "layout(location = 0) in vec4 vertex_position;"
    "layout(location = 1) in vec2 vertex_tex_coords;"

    //"out vec4 position;"
    "out vec2 tex_coords;"
    "uniform mat4 mv;"

    "void main() {"
    "  tex_coords = vertex_tex_coords;"
    //"  position = mv * vertex_position;"
    //"  gl_Position = position;"
    "   gl_Position = mv * vertex_position;"
    "}";

    const char * fragment_shader_src =
    "#version 330\n"

    //"in vec4 position;"
    "in vec2 tex_coords;"

    "out vec4 frag_color;"

    "uniform sampler2D tex;"
    "uniform vec4 color;"

    "void main() {"
    "  vec4 texel = texture(tex, tex_coords);"
    //"  frag_color = vec4(1.0, 1.0, 1.0, 1.0);"
    "  frag_color = 0.5*(texel+color);"
    //"   frag_color = abs(position);"
    "}";

    std::cout << "compiling shaders" << std::endl;

    mShaderProgramID = createProgramFromShaders(vertex_shader_src, fragment_shader_src);

    // get uniform locations
    glUseProgram(mShaderProgramID);

    mUniforms.tex = glGetUniformLocation(mShaderProgramID, "tex") ;
    mUniforms.color = glGetUniformLocation(mShaderProgramID, "color") ;
    mUniforms.mv = glGetUniformLocation(mShaderProgramID, "mv") ;

    // Set shader uniform value
    glUniform1i(mUniforms.tex, 0); // ALWAYS CHANNEL 0

    // Check for errors:
    common:checkOpenGLErrors("GUIShader::GUIShader()");


    // create the rectangle buffer used as instance for all rectangular objects
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    const GLfloat vertices[] = {
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f
    };

    glGenBuffers(1, &mPositionArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mPositionArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    const GLfloat texcoords[] = {
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f
    };

    glGenBuffers(1, &mTexCoordArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords)*sizeof(gfx::TexCoords), &texcoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, sizeof(gfx::TexCoords)/sizeof(float), GL_FLOAT, GL_FALSE, 0, NULL);
}

GUIShader::~GUIShader()
{
    std::cout << "deleting shader: " << mShaderProgramID << std::endl;
    glDeleteProgram(mShaderProgramID);
}

} // namespace gui

} // namespace gfx
