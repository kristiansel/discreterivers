#ifndef SHXDEBUGOUTPUT_H
#define SHXDEBUGOUTPUT_H

#include <string>
#include "shxexpr.h"
#include "../macro/macrodebugassert.h"

namespace shx {

// debug
template<typename T>
std::string get_expr_string(const expr<T> &e)
{
    return get_expr_info_string(e.expr_info_);
}

std::string get_expr_info_string(const expr_info_t &e, int n_uni = 0, int n_attr = 0)
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
            if (e.nt_ == node_type::uniform)        { out = "uni" +std::to_string(n_uni);           }
            if (e.nt_ == node_type::attribute )     { out = "attr"+std::to_string(n_attr);          }
        }
        break;
    case(op_type::add):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_expr_info_string(e.operands_[0])+"+"+get_expr_info_string(e.operands_[1]);
        }
        break;
    case(op_type::sub):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_expr_info_string(e.operands_[0])+"-"+get_expr_info_string(e.operands_[1]);
        }
        break;
    case(op_type::mul):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_expr_info_string(e.operands_[0])+"*"+get_expr_info_string(e.operands_[1]);
        }
        break;
    case(op_type::div):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_expr_info_string(e.operands_[0])+"/"+get_expr_info_string(e.operands_[1]);
        }
        break;
    case(op_type::sample):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = "texture("+get_expr_info_string(e.operands_[0])+","+get_expr_info_string(e.operands_[1])+")";
        }
        break;
    case(op_type::get0):
        {
            DEBUG_ASSERT(e.operands_.size()==1);
            out = get_expr_info_string(e.operands_[0])+".x";
        }
        break;
    case(op_type::get1):
        {
            DEBUG_ASSERT(e.operands_.size()==1);
            out = get_expr_info_string(e.operands_[0])+".y";
        }
        break;
    case(op_type::get2):
        {
            DEBUG_ASSERT(e.operands_.size()==1);
            out = get_expr_info_string(e.operands_[0])+".z";
        }
        break;
    case(op_type::get3):
        {
            DEBUG_ASSERT(e.operands_.size()==1);
            out = get_expr_info_string(e.operands_[0])+".w";
        }
        break;
    case(op_type::makevec2):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = "vec2("+get_expr_info_string(e.operands_[0])+","+get_expr_info_string(e.operands_[1])+")";
        }
        break;
    case(op_type::makevec3):
        {
            DEBUG_ASSERT(e.operands_.size()==3);
            out = "vec3("+get_expr_info_string(e.operands_[0])+","+get_expr_info_string(e.operands_[1])+","+get_expr_info_string(e.operands_[2])+")";
        }
        break;
    case(op_type::makevec4):
        {
            DEBUG_ASSERT(e.operands_.size()==4);
            out = "vec4("+get_expr_info_string(e.operands_[0])+","+get_expr_info_string(e.operands_[1])+","+get_expr_info_string(e.operands_[2])+","+get_expr_info_string(e.operands_[3])+")";
        }
        break;
    default:
        DEBUG_ASSERT(false&&"unknown op type");
    }
    // add braces
    out = "("+out+")";
    return out;
}

}

#endif // SHXDEBUGOUTPUT_H
