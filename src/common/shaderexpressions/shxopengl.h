#ifndef SHXOPENGL_H
#define SHXOPENGL_H

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "shxexpr.h"
#include "../macro/macrodebugassert.h"
#include "../../graphics/gfxcommon.h"

/*#define SHX_GL_UNIFORM_TAG  "uni"
#define SHX_GL_ATTR_TAG     "attr"
#define SHX_GL_INOUT_TAG    "vario"*/

namespace shx {

namespace opengl {

struct term_info {
    int id;
    val_type type;
};

using uniform_info = term_info;
using attribute_info = term_info;

template<typename ... OutTypes>
class vertex_shader
{
    friend class shader_program;

    template<typename ... Ts>
    friend vertex_shader<Ts...> make_vertex_shader(const expr<vec4_t> &pos_out_expr, const expr<Ts>&... out_exprs);

    GLint vertex_shader_id_;

    std::map<int, uniform_info> uniforms_;
    std::map<int, attribute_info> attributes_;

    int n_uniforms_;
    int n_attributes_;

    vertex_shader() : n_uniforms_(0), n_attributes_(0) {}
};

template<typename ... InTypes>
class fragment_shader
{
    friend class shader_program;

    template<typename ... Ts>
    friend fragment_shader<Ts...> make_fragment_shader(const expr<vec4_t> &col_out_expr, const expr<Ts>&... in_exprs);

    GLint fragment_shader_id_;

    std::map<int, uniform_info> uniforms_;
    std::map<int, term_info> inparams_;

    int n_uniforms_;
    int n_inparams_;

    fragment_shader() : n_uniforms_(0)/*, n_attributes_(0)*/ {}
};



// forward declare parse function
std::string get_glsl(const expr_info_t &e,
                     std::map<int, uniform_info> &uniforms,
                     std::map<int, attribute_info> &attributes,
                     std::map<int, term_info> &inparams,
                     int &n_uniforms, int &n_attributes, int &n_inparams);

std::string get_type_str(val_type vt);

template<typename ... Ts>
vertex_shader<Ts...> make_vertex_shader(const expr<vec4_t> &pos_out_expr, const expr<Ts>&... out_exprs)
{
    vertex_shader<Ts...> vs_out;

    std::map<int, term_info> inparams;
    int n_inparams = 0;

    std::string              expr_str =   get_glsl(pos_out_expr.expr_info_,
                                                   vs_out.uniforms_, vs_out.attributes_, inparams,
                                                   vs_out.n_uniforms_, vs_out.n_attributes_, n_inparams);

    std::vector<std::string> out_strs = { get_glsl(out_exprs.expr_info_,
                                                   vs_out.uniforms_, vs_out.attributes_, inparams,
                                                   vs_out.n_uniforms_, vs_out.n_attributes_, n_inparams)... };

    std::vector<val_type> out_val_types = { Ts::valtype... };

    std::stringstream vs;
    vs <<  "#version 410\n";
    vs << std::endl;

    // attributes
    for (const auto &e : vs_out.attributes_)
    {
        vs << "layout(location = " << e.second.id << ") in " << get_type_str(e.second.type) << " attr" << e.second.id << ";" << std::endl;
    }
    vs << std::endl;

    // out
    for (int i = 0; i<out_val_types.size(); i++)
    {
        vs << "out " << get_type_str(out_val_types[i]) << " vario" << i << ";" << std::endl;
    }
    vs << std::endl;

    // uniforms
    for (const auto &e : vs_out.uniforms_)
    {
        vs << "uniform " << get_type_str(e.second.type) << " uni" << e.second.id << ";" << std::endl;
    }
    vs << std::endl;


    vs << "void main()" << std::endl << "{" << std::endl;

    // out section of main
    for (int i = 0; i<out_val_types.size(); i++)
    {
        vs << "  vario" << i << " = " << out_strs[i] << ";" << std::endl;
    }

    // position output of main
    vs << "  gl_Position = " << expr_str << ";" << std::endl << "}";


    std::string vs_str = vs.str();
    const char * vs_c_str = vs_str.c_str();

    std::cout << "vertex shader:" << std::endl;
    std::cout << vs_c_str << std::endl;


    vs_out.vertex_shader_id_ = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs_out.vertex_shader_id_, 1, &vs_c_str, NULL);
    glCompileShader (vs_out.vertex_shader_id_);

    gfx::checkShaderCompiled(vs_out.vertex_shader_id_);

    return vs_out;
}


template<typename ... Ts>
fragment_shader<Ts...> make_fragment_shader(const expr<vec4_t> &col_out_expr, const expr<Ts>&... in_exprs)
{
    fragment_shader<Ts...> fs_out;

    std::vector<int> in_ids = { in_exprs.expr_info_.id_... };
    std::vector<val_type> in_val_types = { Ts::valtype... };

    // set inparams and n in params
    fs_out.n_inparams_ = in_ids.size();
    for (int i = 0; i<fs_out.n_inparams_; i++)
    {
        fs_out.inparams_[in_ids[i]] = {in_ids[i], in_val_types[i]};
    }


    std::map<int, attribute_info> attributes;
    int n_attributes = 0;

    std::string              expr_str =   get_glsl(col_out_expr.expr_info_,
                                                   fs_out.uniforms_, attributes, fs_out.inparams_,
                                                   fs_out.n_uniforms_, n_attributes, fs_out.n_inparams_);

    DEBUG_ASSERT((n_attributes==0)&&"error parsing fragment shader expression: attribute encountered, are `in` expressions properly included?");

    std::stringstream fs;
    fs <<  "#version 410\n";
    fs << std::endl;

    // attributes
    /*for (const auto &e : fs_out.attributes_)
    {
        fs << "layout(location = " << e.second.id << ") in " << get_type_str(e.second.type) << " attr" << e.second.id << ";" << std::endl;
    }
    fs << std::endl;*/

    // in
    for (int i = 0; i<in_val_types.size(); i++)
    {
        fs << "in " << get_type_str(in_val_types[i]) << " vario" << in_ids[i] << ";" << std::endl;
    }
    fs << std::endl;

    // out
    fs << "out vec4 frag_color;" << std::endl << std::endl;

    // uniforms
    for (const auto &e : fs_out.uniforms_)
    {
        fs << "uniform " << get_type_str(e.second.type) << " uni" << e.second.id << ";" << std::endl;
    }
    fs << std::endl;


    fs << "void main()" << std::endl << "{" << std::endl;

    // color output of main
    fs << "  frag_color = " << expr_str << ";" << std::endl << "}";


    std::string fs_str = fs.str();
    const char * fs_c_str = fs_str.c_str();

    std::cout << "fragment shader:" << std::endl;
    std::cout << fs_c_str << std::endl;

    fs_out.fragment_shader_id_ = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs_out.fragment_shader_id_, 1, &fs_c_str, NULL);
    glCompileShader (fs_out.fragment_shader_id_);

    gfx::checkShaderCompiled(fs_out.fragment_shader_id_);

    return fs_out;
}

class shader_program
{
    GLuint              program_id_;
    std::vector<GLint>  uniforms_;
    std::vector<GLint>  attributes_;

    shader_program() {}
public:
    template<typename ... IOTypes>
    static shader_program create(const vertex_shader<IOTypes...> &vertex_shader,
                                 const fragment_shader<IOTypes...> &fragment_shader);
};


template<typename ... IOTypes>
shader_program shader_program::create(const vertex_shader<IOTypes...> &vertex_shader,
                                      const fragment_shader<IOTypes...> &fragment_shader)
{
    shader_program sp_out;

    sp_out.program_id_ = glCreateProgram ();
    glAttachShader (sp_out.program_id_, fragment_shader.fragment_shader_id_);
    glAttachShader (sp_out.program_id_, vertex_shader.vertex_shader_id_);
    glLinkProgram (sp_out.program_id_);

    // should check linker error
    gfx::checkProgramLinked(sp_out.program_id_);

    // where should this be? leak it for now...

    // shaders are copied into the program, so time to
    // clean up shaders
    //glDetachShader(sp_out.program_id_, vertex_shader);
    //glDetachShader(sp_out.program_id_, fragment_shader);

    //glDeleteShader(vertex_shader);
    //glDeleteShader(fragment_shader);

    glUseProgram(sp_out.program_id_);

    //std::set<int,


    return sp_out;
}

// parser
std::string get_glsl_rec(const expr_info_t &e,
                         std::map<int, uniform_info> &uniforms,
                         std::map<int, attribute_info> &attributes,
                         std::map<int, term_info> &inparams,
                         int &n_uniforms, int &n_attributes, int &n_inparams)
{
    std::string out = "";

    auto it = inparams.find(e.id_);
    if (it == inparams.end())
    {
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
                    //intterm_ptr = e.terminal_.term_ptr;
                    auto it = uniforms.find(e.id_);
                    if (it == uniforms.end())
                    {
                        uniforms[e.id_] = { e.id_, e.vt_ };;
                        out = "uni" +std::to_string(e.id_);
                        n_uniforms++;
                    }
                    else
                    {
                        out = "uni"+std::to_string(it->second.id);
                    }
                }
                if (e.nt_ == node_type::attribute)
                {
                    //intterm_ptr = e.terminal_.term_ptr;
                    auto it = attributes.find(e.id_);
                    if (it == attributes.end())
                    {
                        attributes[e.id_] = { e.id_, e.vt_ };
                        out = "attr" +std::to_string(e.id_);
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
                out = get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+"+"+get_glsl_rec(e.operands_[1], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams);
            }
            break;
        case(op_type::sub):
            {
                DEBUG_ASSERT(e.operands_.size()==2);
                out = get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+"-"+get_glsl_rec(e.operands_[1], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams);
            }
            break;
        case(op_type::mul):
            {
                DEBUG_ASSERT(e.operands_.size()==2);
                out = get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+"*"+get_glsl_rec(e.operands_[1], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams);
            }
            break;
        case(op_type::div):
            {
                DEBUG_ASSERT(e.operands_.size()==2);
                out = get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+"/"+get_glsl_rec(e.operands_[1], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams);
            }
            break;
        case(op_type::sample):
            {
                DEBUG_ASSERT(e.operands_.size()==2);
                out = "texture("+get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+","+get_glsl_rec(e.operands_[1], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+")";
            }
            break;
        case(op_type::get0):
            {
                DEBUG_ASSERT(e.operands_.size()==1);
                out = get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+".x";
            }
            break;
        case(op_type::get1):
            {
                DEBUG_ASSERT(e.operands_.size()==1);
                out = get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+".y";
            }
            break;
        case(op_type::get2):
            {
                DEBUG_ASSERT(e.operands_.size()==1);
                out = get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+".z";
            }
            break;
        case(op_type::get3):
            {
                DEBUG_ASSERT(e.operands_.size()==1);
                out = get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+".w";
            }
            break;
        case(op_type::makevec2):
            {
                DEBUG_ASSERT(e.operands_.size()==2);
                out = "vec2("+get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+","+get_glsl_rec(e.operands_[1], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+")";
            }
            break;
        case(op_type::makevec3):
            {
                DEBUG_ASSERT(e.operands_.size()==3);
                out = "vec3("+get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+","+get_glsl_rec(e.operands_[1], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+","+get_glsl_rec(e.operands_[2], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+")";
            }
            break;
        case(op_type::makevec4):
            {
                DEBUG_ASSERT(e.operands_.size()==4);
                out = "vec4("+get_glsl_rec(e.operands_[0], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+","+get_glsl_rec(e.operands_[1], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+","+get_glsl_rec(e.operands_[2], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+","+get_glsl_rec(e.operands_[3], uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams)+")";
            }
            break;
        default:
            DEBUG_ASSERT(false&&"unknown op type");
        } // switch (e.op_)
    } // if (it == inparams.end())
    else
    {
        return "vario"+std::to_string(it->second.id);
    }

    // add braces
    return "("+out+")";
}

std::string get_glsl(const expr_info_t &e,
                     std::map<int, uniform_info> &uniforms,
                     std::map<int, attribute_info> &attributes,
                     std::map<int, term_info> &inparams,
                     int &n_uniforms, int &n_attributes, int &n_inparams)
{
    return get_glsl_rec(e, uniforms, attributes, inparams, n_uniforms, n_attributes, n_inparams);
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
