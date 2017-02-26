#ifndef SHEXPR_H
#define SHEXPR_H

#include <vector>
#include "../macro/macrodebugassert.h"

enum class op_type       { none,
                           add, sub, mul, div,
                           sample,
                           get0, get1, get2, get3,
                           makevec2, makevec3, makevec4 };

enum class node_type     { internal, uniform, attribute, literal };

enum class val_type      { sint, sfloat, svec2, svec3, svec4, smat3, smat4, stex2d};

struct sint       { static const val_type valtype = val_type::sint;     };
struct sfloat     { static const val_type valtype = val_type::sfloat;   };
struct svec2      { static const val_type valtype = val_type::svec2;    };
struct svec3      { static const val_type valtype = val_type::svec3;    };
struct svec4      { static const val_type valtype = val_type::svec4;    };
struct smat3      { static const val_type valtype = val_type::smat3;    };
struct smat4      { static const val_type valtype = val_type::smat4;    };
struct stex2d     { static const val_type valtype = val_type::stex2d;   };

template<typename T>
struct uniform {};

template<typename T>
struct attribute {};

struct expr_info_t
{
    op_type     op_;
    node_type   nt_;
    val_type    vt_;

    std::vector<expr_info_t> operands_;

    float literal_; // could unionize with int and pointer in case of terminal
};

template<typename T>
struct expr_helper;

struct expr_base
{
protected:
    expr_base() {}
public:
    expr_base(op_type op, node_type nt, val_type vt) :
       expr_info_{op, nt, vt, {}, 1.0f} {}

    template<typename T>
    friend struct make_expr_helper;

    expr_info_t expr_info_;
};

template<typename T>
struct expr : public expr_base
{
    expr(const attribute<T> &attr) :
        expr_base(op_type::none, node_type::attribute, val_type(T::valtype)) {}

    expr(const uniform<T> &attr) :
        expr_base(op_type::none, node_type::uniform, val_type(T::valtype)) {}

    template<typename Ta, typename Tb>
    static expr<T> make_binary_expr(op_type op, node_type nt, val_type vt, const expr<Ta> &a, const expr<Tb> &b)
    {
        expr<T> out;
        out.expr_info_.op_ = op;
        out.expr_info_.nt_ = nt;
        out.expr_info_.vt_ = vt;
        out.expr_info_.operands_.push_back(a.expr_info_);
        out.expr_info_.operands_.push_back(b.expr_info_);
        return out;
    }
private:
    expr() {}
};

// operators
expr<svec4> operator*(const expr<smat4> &a, const expr<svec4> &b)
{
    return expr<svec4>::make_binary_expr(op_type::mul, node_type::internal, val_type(svec4::valtype), a, b);
}

expr<svec4> operator*(const expr<svec4> &a, const expr<svec4> &b)
{
    return expr<svec4>::make_binary_expr(op_type::mul, node_type::internal, val_type(svec4::valtype), a, b);
}

expr<smat4> operator*(const expr<smat4> &a, const expr<smat4> &b)
{
    return expr<smat4>::make_binary_expr(op_type::mul, node_type::internal, val_type(smat4::valtype), a, b);
}

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
            if (e.nt_ == node_type::literal) { out = std::to_string(e.literal_);    }
            if (e.nt_ == node_type::uniform) { out = "uni" +std::to_string(n_uni);  }
            if (e.nt_ == node_type::attribute ) { out = "attr"+std::to_string(n_attr); }
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



#endif // SHEXPR_H
