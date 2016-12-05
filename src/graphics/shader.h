#ifndef SHADER_H
#define SHADER_H

#include "gfxcommon.h"

namespace gfx {

class Shader {
public:
    Shader()
    {
        // set up shaders
        const char * vertex_shader_src =
        "#version 330\n"

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
        "#version 330\n"

        "in vec4 position;"
        "in vec4 normal;"

        "out vec4 frag_color;"

        "uniform sampler2D tex;"
        "uniform vec4 color;"
        "uniform vec4 light_position;"
        "uniform vec4 light_color;"

        "void main() {"
        //"  vec4 texel = texture(tex, vec2(0, 0));"
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

    GLuint getProgramID() const {return mShaderProgramID;}

    struct Uniforms;
    const Uniforms &getUniforms() const {return mUniforms;}

private:
    GLuint mShaderProgramID;

    struct Uniforms
    {
        GLint mv;
        GLint p;
        GLint tex;
        GLint color;
        GLint light_position;
        GLint light_color;
    } mUniforms;
};

}

#endif // SHADER_H
