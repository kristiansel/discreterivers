#include "shader.h"

namespace gfx {

Shader::Shader()
{
    // set up shaders
    const char * vertex_shader_src =
    "#version 410\n"

    "layout(location = 0) in vec4 vertex_position;"
    "layout(location = 1) in vec4 vertex_normal;"
    "layout(location = 2) in vec2 vertex_tex_coords;"

    "out vec4 position;"
    "out vec3 normal;"
    "out vec2 tex_coords;"

    "uniform mat4 mv;"
    "uniform mat4 p;"
    "uniform float z_offset;"

    "void main() {"
    "  tex_coords = vertex_tex_coords;"
    "  position = mv * vec4(vertex_position.xyz, 1.0);"
    //"  vec4 n4 = transpose(inverse(mv)) * vec4(vertex_normal.xyz, 0.0);"
    "  vec4 n4 = mv * vec4(vertex_normal.xyz, 0.0);"
    "  normal = normalize(n4.xyz);"
    "  gl_Position = p * (position + vec4(0, 0, z_offset, 0));"
    "}";

    const char * fragment_shader_src =
    "#version 410\n"

    "in vec4 position;"
    "in vec3 normal;"
    "in vec2 tex_coords;"

    "out vec4 frag_color;"

    "uniform sampler2D tex;"
    "uniform vec4 color;"
    "uniform int  num_lights = 0;"
    "uniform vec4 light_position_array[10];"
    "uniform vec4 light_color_array[10];"

    "uniform vec3 ambient = vec3(0.05, 0.05, 0.05);"

    "void main() {"
    "  vec3 total_light = ambient;" // initialize light accumulator
    "  "
    "  for (int i = 0; i<min(num_lights, 10); i++)"
    "  {"
    "    vec3 light_dir_cam_space = normalize((light_position_array[i] - position).xyz);"
    "    float nDotL = dot(normal, light_dir_cam_space);"
    "    total_light = total_light + max(nDotL, 0) * light_color_array[i].rgb;"
    "  }"
    "  "
    "  vec4 texel = texture(tex, tex_coords);"
    "  frag_color = vec4(texel.rgb * total_light, 1.0);"
    "}";

    std::cout << "compiling shaders" << std::endl;
    mShaderProgramID = createProgramFromShaders(vertex_shader_src, fragment_shader_src);

    // get uniform locations
    glUseProgram(mShaderProgramID);

    mUniforms.mv = glGetUniformLocation(mShaderProgramID, "mv") ;
    mUniforms.p = glGetUniformLocation(mShaderProgramID, "p") ;
    mUniforms.z_offset = glGetUniformLocation(mShaderProgramID, "z_offset") ;
    mUniforms.tex = glGetUniformLocation(mShaderProgramID, "tex") ;
    mUniforms.color = glGetUniformLocation(mShaderProgramID, "color") ;
    mUniforms.num_lights = glGetUniformLocation(mShaderProgramID, "num_lights") ;
    mUniforms.light_position_array = glGetUniformLocation(mShaderProgramID, "light_position_array") ;
    mUniforms.light_color_array = glGetUniformLocation(mShaderProgramID, "light_color_array") ;

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
