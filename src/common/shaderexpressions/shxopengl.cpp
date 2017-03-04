#include "shxopengl.h"

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include "shxexpr.h"
#include "../macro/macrodebugassert.h"
#include "../../graphics/gfxcommon.h"


namespace shx {

namespace opengl {

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

} // opengl

} // shx
