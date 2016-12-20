#include "shader.h"

namespace gfx {

Shader::Shader()
{
    // set up shaders
    const char * vertex_shader_src =
    "#version 330\n"

    "layout(location = 0) in vec4 vertex_position;"
    "layout(location = 1) in vec4 vertex_normal;"
    "layout(location = 2) in vec2 vertex_tex_coords;"

    "out vec4 position;"
    "out vec4 normal;"
    "out vec2 tex_coords;"

    "uniform mat4 mv;"
    "uniform mat4 p;"

    "void main() {"
    "  tex_coords = vertex_tex_coords;"
    "  position = mv * vec4(vertex_position.xyz, 1.0);"
    "  normal = mv * vec4(vertex_normal.xyz, 0.0);"
    "  gl_Position = p * position;"
    "}";

    const char * fragment_shader_src =
    "#version 330\n"

    "in vec4 position;"
    "in vec4 normal;"
    "in vec2 tex_coords;"

    "out vec4 frag_color;"

    "uniform sampler2D tex;"
    "uniform vec4 color;"
    "uniform vec4 light_position;"
    "uniform vec4 light_color;"

    "void main() {"
    "  vec4 texel = texture(tex, tex_coords);"
    "  /*vec4 col = 0.5*(texel+color);*/"
    "  vec4 lightdirection = light_position - position;"
    "  float nDotL = dot(normal.xyz, normalize(lightdirection.xyz));"
    "  frag_color = vec4(texel.rgb * max(nDotL, 0), 1.0);"
    "  /*frag_color = vec4(vec3(tex_coords, 0) * max(nDotL, 0), 1.0);*/"
    "}";

    std::cout << "compiling shaders" << std::endl;
    mShaderProgramID = createProgramFromShaders(vertex_shader_src, fragment_shader_src);

    // get uniform locations
    glUseProgram(mShaderProgramID);

    mUniforms.mv = glGetUniformLocation(mShaderProgramID, "mv") ;
    mUniforms.p = glGetUniformLocation(mShaderProgramID, "p") ;
    mUniforms.tex = glGetUniformLocation(mShaderProgramID, "tex") ;
    mUniforms.color = glGetUniformLocation(mShaderProgramID, "color") ;
    mUniforms.light_position = glGetUniformLocation(mShaderProgramID, "light_position") ;
    mUniforms.light_color = glGetUniformLocation(mShaderProgramID, "light_color") ;

    // Set shader uniform value
    glUniform1i(mUniforms.tex, 0); // ALWAYS CHANNEL 0

    // Check for errors:
    common:checkOpenGLErrors("Shader::Shader()");
}

Shader::~Shader()
{
    std::cout << "deleting shader: " << mShaderProgramID << std::endl;
    glDeleteProgram(mShaderProgramID);
}

} // namespace gfx
