#ifndef SHXOPENGL_H
#define SHXOPENGL_H

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "shxexpr.h"
#include "../macro/macrodebugassert.h"
#include "../../graphics/gfxcommon.h"

namespace shx {

namespace opengl {

class vertex_shader
{

};

class fragment_shader
{

};

struct term_info {
    int id;
    val_type type;
};

using uniform_info = term_info;
using attribute_info = term_info;

// forward declare parse function
std::string get_glsl(const expr_info_t &e,
                     std::map<const void*, uniform_info> &uniforms,
                     std::map<const void*, attribute_info> &attributes,
                     int &n_uniforms, int &n_attributes);

std::string get_type_str(val_type vt);

template<typename ... Ts>
vertex_shader make_vertex_shader(const expr<vec4_t> &pos_out_expr, const expr<Ts>&... out_exprs)
{
    std::map<const void*, uniform_info> uniforms;
    std::map<const void*, attribute_info> attributes;
    int n_uniforms = 0;
    int n_attributes = 0;
    std::string              expr_str =   get_glsl(pos_out_expr.expr_info_, uniforms, attributes, n_uniforms, n_attributes)     ;
    std::vector<std::string> out_strs = { get_glsl(out_exprs.expr_info_   , uniforms, attributes, n_uniforms, n_attributes)... };

    std::vector<val_type> out_val_types = { Ts::valtype... };

    std::stringstream vs;
    vs <<  "#version 410\n";
    vs << std::endl;

    // attributes
    for (const auto &e : attributes)
    {
        vs << "layout(location = " << e.second.id << ") in " << get_type_str(e.second.type) << " attr" << e.second.id << ";" << std::endl;
    }
    vs << std::endl;

    // out
    for (int i = 0; i<out_val_types.size(); i++)
    {
        vs << "out " << get_type_str(out_val_types[i]) << " outparam" << i << ";" << std::endl;
    }
    vs << std::endl;

    // uniforms
    for (const auto &e : uniforms)
    {
        vs << "uniform " << get_type_str(e.second.type) << " uni" << e.second.id << ";" << std::endl;
    }
    vs << std::endl;


    vs << "void main()" << std::endl << "{" << std::endl;

    // out section of main
    for (int i = 0; i<out_val_types.size(); i++)
    {
        vs << "  outparam" << i << " = " << out_strs[i] << ";" << std::endl;
    }

    // position output of main
    vs << "  gl_Position = " << expr_str << ";" << std::endl << "}";

    std::string vs_str = vs.str();
    const char * vs_c_str = vs_str.c_str();

    std::cout << "vertex shader:" << std::endl;
    std::cout << vs_c_str << std::endl;

    GLuint vertex_shader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vertex_shader, 1, &vs_c_str, NULL);
    glCompileShader (vertex_shader);

    gfx::checkShaderCompiled(vertex_shader);
}

fragment_shader make_fragment_shader() {}

class shader_program
{
    GLuint              program_id_;
    std::vector<GLint>  uniforms_;

    shader_program() = delete;
public:
    shader_program(const vertex_shader &vs, const fragment_shader &fs) {}
};

shader_program make_shader_program() {}

// parser
std::string get_glsl_rec(const expr_info_t &e,
                         std::map<const void*, uniform_info> &uniforms,
                         std::map<const void*, attribute_info> &attributes,
                         int &n_uniforms, int &n_attributes)
{
    std::string out = "";
    switch (e.op_)
    {
    case(op_type::none):
        {
            DEBUG_ASSERT(e.operands_.size()==0 && e.nt_!=node_type::internal);
            if (e.nt_ == node_type::literal)
            {
                if (e.vt_ == val_type::float_t) { out = std::to_string(e.terminal_.literal_float); }
                if (e.vt_ == val_type::int_t)   { out = std::to_string(e.terminal_.literal_int); }
            }
            if (e.nt_ == node_type::uniform)
            {
                const void *term_ptr = e.terminal_.term_ptr;
                auto it = uniforms.find(term_ptr);
                if (it == uniforms.end())
                {
                    uniforms[term_ptr] = { n_uniforms, e.vt_ };;
                    out = "uni" +std::to_string(n_uniforms);
                    n_uniforms++;
                }
                else
                {
                    out = "uni"+std::to_string(it->second.id);
                }
            }
            if (e.nt_ == node_type::attribute)
            {
                const void *term_ptr = e.terminal_.term_ptr;
                auto it = attributes.find(term_ptr);
                if (it == attributes.end())
                {
                    attributes[term_ptr] = { n_attributes, e.vt_ };
                    out = "attr" +std::to_string(n_attributes);
                    n_attributes++;
                }
                else
                {
                    out = "attr"+std::to_string(it->second.id);
                }
            }
        }
        break;
    case(op_type::add):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+"+"+get_glsl_rec(e.operands_[1], uniforms, attributes, n_uniforms, n_attributes);
        }
        break;
    case(op_type::sub):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+"-"+get_glsl_rec(e.operands_[1], uniforms, attributes, n_uniforms, n_attributes);
        }
        break;
    case(op_type::mul):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+"*"+get_glsl_rec(e.operands_[1], uniforms, attributes, n_uniforms, n_attributes);
        }
        break;
    case(op_type::div):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+"/"+get_glsl_rec(e.operands_[1], uniforms, attributes, n_uniforms, n_attributes);
        }
        break;
    case(op_type::sample):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = "texture("+get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+","+get_glsl_rec(e.operands_[1], uniforms, attributes, n_uniforms, n_attributes)+")";
        }
        break;
    case(op_type::get0):
        {
            DEBUG_ASSERT(e.operands_.size()==1);
            out = get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+".x";
        }
        break;
    case(op_type::get1):
        {
            DEBUG_ASSERT(e.operands_.size()==1);
            out = get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+".y";
        }
        break;
    case(op_type::get2):
        {
            DEBUG_ASSERT(e.operands_.size()==1);
            out = get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+".z";
        }
        break;
    case(op_type::get3):
        {
            DEBUG_ASSERT(e.operands_.size()==1);
            out = get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+".w";
        }
        break;
    case(op_type::makevec2):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = "vec2("+get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+","+get_glsl_rec(e.operands_[1], uniforms, attributes, n_uniforms, n_attributes)+")";
        }
        break;
    case(op_type::makevec3):
        {
            DEBUG_ASSERT(e.operands_.size()==3);
            out = "vec3("+get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+","+get_glsl_rec(e.operands_[1], uniforms, attributes, n_uniforms, n_attributes)+","+get_glsl_rec(e.operands_[2], uniforms, attributes, n_uniforms, n_attributes)+")";
        }
        break;
    case(op_type::makevec4):
        {
            DEBUG_ASSERT(e.operands_.size()==4);
            out = "vec4("+get_glsl_rec(e.operands_[0], uniforms, attributes, n_uniforms, n_attributes)+","+get_glsl_rec(e.operands_[1], uniforms, attributes, n_uniforms, n_attributes)+","+get_glsl_rec(e.operands_[2], uniforms, attributes, n_uniforms, n_attributes)+","+get_glsl_rec(e.operands_[3], uniforms, attributes, n_uniforms, n_attributes)+")";
        }
        break;
    default:
        DEBUG_ASSERT(false&&"unknown op type");
    }
    // add braces
    out = "("+out+")";
    return out;
}

std::string get_glsl(const expr_info_t &e,
                     std::map<const void*, uniform_info> &uniforms,
                     std::map<const void*, attribute_info> &attributes,
                     int &n_uniforms, int &n_attributes)
{
    return get_glsl_rec(e, uniforms, attributes, n_uniforms, n_attributes);
}

template<typename T>
std::string get_glsl_expr(const expr<T> &e)
{
    return get_glsl_expr_info(e.expr_info_);
}

//{ int_t, float_t, vec2_t, vec3_t, vec4_t, mat3_t, mat4_t, tex2d_t};


std::string get_type_str(val_type vt)
{
    switch (vt)
    {
    case(val_type::int_t    )   : return "int"      ; break;
    case(val_type::float_t  )   : return "float"    ; break;
    case(val_type::vec2_t   )   : return "vec2"     ; break;
    case(val_type::vec3_t   )   : return "vec3"     ; break;
    case(val_type::vec4_t   )   : return "vec4"     ; break;
    case(val_type::mat3_t   )   : return "mat3"     ; break;
    case(val_type::mat4_t   )   : return "mat4"     ; break;
    case(val_type::tex2d_t  )   : return "sampler2D"; break;
    default: DEBUG_ASSERT(false&&"shxopengl:get_type_str: unknown val_type");
    }
}

} // namespace opengl

} // namespace shx

#endif // SHXOPENGL_H
