#include "guitextshader.h"

namespace gfx {

namespace gui {

GUITextShader::GUITextShader(int w, int h) :
    /*mOriginalResolution{w, h},*/ mRescaleMatrix(vmath::Matrix4::identity())
{
    // set up shaders
    const char * vertex_shader_src =
    "#version 410\n"

    "layout(location = 0) in vec4 vertex_position;"
    "layout(location = 1) in vec2 vertex_tex_coords;"

    "out vec2 tex_coords;"
    "uniform mat4 mv;"

    "void main() {"
    "  tex_coords = vertex_tex_coords;"
    "  gl_Position = mv * vertex_position;"
    "}";

    const char * fragment_shader_src =
    "#version 410\n"

    "in vec2 tex_coords;"

    "out vec4 frag_color;"

    "uniform sampler2D tex;"
    "uniform vec4 color;"

    "void main() {"
    "  frag_color = vec4(1, 1, 1, texture(tex, tex_coords).r) * color;"
    "}";

    std::cout << "compiling shaders" << std::endl;

    mShaderProgramID = createProgramFromShaders(vertex_shader_src, fragment_shader_src);

    // get uniform locations
    glUseProgram(mShaderProgramID);

    mUniforms.tex = glGetUniformLocation(mShaderProgramID, "tex") ;
    mUniforms.mv = glGetUniformLocation(mShaderProgramID, "mv") ;
    mUniforms.color = glGetUniformLocation(mShaderProgramID, "color");

    // Set shader uniform value
    glUniform1i(mUniforms.tex, 0); // ALWAYS CHANNEL 0

    // Check for errors:
    common:checkOpenGLErrors("GUIShader::GUIShader()");
}

GUITextShader::~GUITextShader()
{
    std::cout << "deleting shader: " << mShaderProgramID << std::endl;
    glDeleteProgram(mShaderProgramID);
}

} // namespace gui

} // namespace gfx
