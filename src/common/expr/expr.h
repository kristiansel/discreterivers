#ifndef EXPR_H
#define EXPR_H

#include <vector>
#include <string>

#include "../macro/macrodebugassert.h"
#include "combineunique.h"

//namespace expr {

enum class op_type       { none, add, sub, mul, div, sample, extract0, makevec2, makevec3, makevec4};
enum class node_type     { internal, uniform, attrib, literal };
enum class val_type      { sint, sfloat, svec2, svec3, svec4, smat3, smat4, stex2d};

struct sint       { static const val_type valtype = val_type::sint;     };
struct sfloat     { static const val_type valtype = val_type::sfloat;   };
struct svec2      { static const val_type valtype = val_type::svec2;    };
struct svec3      { static const val_type valtype = val_type::svec3;    };
struct svec4      { static const val_type valtype = val_type::svec4;    };
struct smat3      { static const val_type valtype = val_type::smat3;    };
struct smat4      { static const val_type valtype = val_type::smat4;    };
struct stex2d     { static const val_type valtype = val_type::stex2d;   };


// uniform and attrib
template<typename ValueType, typename UnifomTag>
struct uniform {};

template<typename ValueType, typename AttribTag>
struct attrib {};

// collections of uniforms/attribs
template<typename ...Uniforms>
struct uniforms {};

template<typename ...Attribs>
struct attribs {};

template<typename ...Types>
struct expr;

class expr_base
{
    template<typename ...Types>
    friend struct expr;

    expr_base() {}
public:
    inline void init(op_type o, node_type nt, val_type vt, float l=0.0f) { op_=o; nt_=nt; vt_=vt; literal_=l; }

    op_type op_;
    node_type nt_;
    val_type vt_;
    std::vector<const expr_base*> operands_;
    float literal_;
};


template<typename ...Types>
struct make_expr_helper;

template<typename ...Types>
struct expr;

template<typename ExprType, typename ...Uniforms, typename ...Attribs>
class expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>> : public expr_base
{
    friend struct make_expr_helper<ExprType, uniforms<Uniforms...>, attribs<Attribs...>>;
    expr() {}
public:

};

template<typename ExprType, typename ...Uniforms, typename ...Attribs>
struct make_expr_helper<ExprType, uniforms<Uniforms...>, attribs<Attribs...>>
{
    static expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>>
    make_0ary_expr(node_type nt, val_type vt)
    {
        expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>> out;
        out.init(op_type::none, nt, vt);
        std::cout << "created identity expression" << std::endl;
        return out;
    }

    template<typename AExprType, typename ...AUniforms, typename ...AAttribs>
    static expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>>
    make_1ary_expr(op_type o, node_type nt, val_type vt,
         const expr<AExprType, uniforms<AUniforms...>, attribs<AAttribs...>> &a)
    {
        expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>> out;
        out.init(o, nt, vt);
        out.operands_.push_back(&a);
        std::cout << "created unary expression" << std::endl;
        return out;
    }

    template<typename AExprType, typename ...AUniforms, typename ...AAttribs,
             typename BExprType, typename ...BUniforms, typename ...BAttribs>
    static expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>>
    make_2ary_expr(op_type o, node_type nt, val_type vt,
         const expr<AExprType, uniforms<AUniforms...>, attribs<AAttribs...>> &a,
         const expr<BExprType, uniforms<BUniforms...>, attribs<BAttribs...>> &b)
    {
        expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>> out;
        out.init(o, nt, vt);
        out.operands_.push_back(&a);
        out.operands_.push_back(&b);
        std::cout << "created binary expression" << std::endl;
        return out;
    }

    template<typename AExprType, typename ...AUniforms, typename ...AAttribs,
             typename BExprType, typename ...BUniforms, typename ...BAttribs,
             typename CExprType, typename ...CUniforms, typename ...CAttribs>
    static expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>>
    make_3ary_expr(op_type o, node_type nt, val_type vt,
         const expr<AExprType, uniforms<AUniforms...>, attribs<AAttribs...>> &a,
         const expr<BExprType, uniforms<BUniforms...>, attribs<BAttribs...>> &b,
         const expr<CExprType, uniforms<CUniforms...>, attribs<CAttribs...>> &c)
    {
        expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>> out;
        out.init(o, nt, vt);
        out.operands_.push_back(&a);
        out.operands_.push_back(&b);
        out.operands_.push_back(&c);
        std::cout << "created trinary expression" << std::endl;
        return out;
    }

    template<typename AExprType, typename ...AUniforms, typename ...AAttribs,
             typename BExprType, typename ...BUniforms, typename ...BAttribs,
             typename CExprType, typename ...CUniforms, typename ...CAttribs,
             typename DExprType, typename ...DUniforms, typename ...DAttribs>
    static expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>>
    make_4ary_expr(op_type o, node_type nt, val_type vt,
         const expr<AExprType, uniforms<AUniforms...>, attribs<AAttribs...>> &a,
         const expr<BExprType, uniforms<BUniforms...>, attribs<BAttribs...>> &b,
         const expr<CExprType, uniforms<CUniforms...>, attribs<CAttribs...>> &c,
         const expr<DExprType, uniforms<DUniforms...>, attribs<DAttribs...>> &d)
    {
        expr<ExprType, uniforms<Uniforms...>, attribs<Attribs...>> out;
        out.init(o, nt, vt);
        out.operands_.push_back(&a);
        out.operands_.push_back(&b);
        out.operands_.push_back(&c);
        out.operands_.push_back(&d);
        std::cout << "created quatary expression" << std::endl;
        return out;
    }
};

// result type helper
template<typename RExprType>
struct result_type
{
    template<typename AExprType, typename ...AUniforms, typename ...AAttribs>
    static
    expr<RExprType, uniforms<AUniforms...>, attribs<AAttribs...>>
    combine_uni(op_type o, const expr<AExprType, uniforms<AUniforms...>, attribs<AAttribs...>> &a)
    {

        return make_expr_helper<RExprType, uniforms<AUniforms...>, attribs<AAttribs...>
                                >::make_1ary_expr(o, node_type::internal, RExprType::valtype, a);
    }

    template<typename AExprType, typename ...AUniforms, typename ...AAttribs,
             typename BExprType, typename ...BUniforms, typename ...BAttribs>
    static
    expr<RExprType,
         typename combine_unique<uniforms<AUniforms...>, uniforms<BUniforms...>>::type,
         typename combine_unique<attribs <AAttribs... >, attribs <BAttribs... >>::type>
    combine_bin(op_type o, const expr<AExprType, uniforms<AUniforms...>, attribs<AAttribs...>> &a,
                           const expr<BExprType, uniforms<BUniforms...>, attribs<BAttribs...>> &b)
    {

        return make_expr_helper<RExprType, typename combine_unique<uniforms<AUniforms...>, uniforms<BUniforms...>>::type,
                                           typename combine_unique<attribs <AAttribs... >, attribs <BAttribs... >>::type
                                >::make_2ary_expr(o, node_type::internal, RExprType::valtype, a, b);
    }

    template<typename AExprType, typename ...AUniforms, typename ...AAttribs,
             typename BExprType, typename ...BUniforms, typename ...BAttribs,
             typename CExprType, typename ...CUniforms, typename ...CAttribs>
    static
    expr<RExprType,
         typename combine_unique<uniforms<AUniforms...>,
             typename combine_unique<uniforms<BUniforms...>, uniforms<CUniforms...>>::type >::type,
         typename combine_unique<attribs <AAttribs... >,
             typename combine_unique<attribs <BAttribs... >, attribs <CAttribs... >>::type >::type>
    combine_tri(op_type o, const expr<AExprType, uniforms<AUniforms...>, attribs<AAttribs...>> &a,
                           const expr<BExprType, uniforms<BUniforms...>, attribs<BAttribs...>> &b,
                           const expr<CExprType, uniforms<CUniforms...>, attribs<CAttribs...>> &c)
    {

        return make_expr_helper<RExprType, typename combine_unique<uniforms<AUniforms...>,
                                               typename combine_unique<uniforms<BUniforms...>, uniforms<CUniforms...>>::type >::type,
                                           typename combine_unique<attribs <AAttribs... >,
                                               typename combine_unique<attribs <BAttribs... >, attribs <CAttribs... >>::type >::type
                                >::make_3ary_expr(o, node_type::internal, RExprType::valtype, a, b, c);
    }

    template<typename AExprType, typename ...AUniforms, typename ...AAttribs,
             typename BExprType, typename ...BUniforms, typename ...BAttribs,
             typename CExprType, typename ...CUniforms, typename ...CAttribs,
             typename DExprType, typename ...DUniforms, typename ...DAttribs>
    static
    expr<RExprType,
         typename combine_unique<uniforms<AUniforms...>,
             typename combine_unique<uniforms<BUniforms...>,
                typename combine_unique<uniforms<CUniforms...>, uniforms<DUniforms...>>::type >::type >::type,
         typename combine_unique<attribs <AAttribs... >,
             typename combine_unique<attribs <BAttribs... >,
                typename combine_unique<attribs <CAttribs... >, attribs <DAttribs... >>::type >::type >::type>
    combine_quat(op_type o, const expr<AExprType, uniforms<AUniforms...>, attribs<AAttribs...>> &a,
                            const expr<BExprType, uniforms<BUniforms...>, attribs<BAttribs...>> &b,
                            const expr<CExprType, uniforms<CUniforms...>, attribs<CAttribs...>> &c,
                            const expr<DExprType, uniforms<DUniforms...>, attribs<DAttribs...>> &d)
    {

        return make_expr_helper<RExprType,
            typename combine_unique<uniforms<AUniforms...>,
                typename combine_unique<uniforms<BUniforms...>,
                   typename combine_unique<uniforms<CUniforms...>, uniforms<DUniforms...>>::type >::type >::type,
            typename combine_unique<attribs <AAttribs... >,
                typename combine_unique<attribs <BAttribs... >,
                   typename combine_unique<attribs <CAttribs... >, attribs <DAttribs... >>::type >::type >::type
                                >::make_4ary_expr(o, node_type::internal, RExprType::valtype, a, b, c, d);
    }
};

//========================//
//      OPERATORS         //
//========================//

// multiplication
// mat4 * vec4
template<typename... ATerminals, typename... BTerminals>
auto operator*(const expr<smat4, ATerminals...> &a,
               const expr<svec4, BTerminals...> &b) -> decltype(result_type<svec4>::combine_bin(op_type::mul, a, b))
{
    return result_type<svec4>::combine_bin(op_type::mul, a, b);
}

// vec4 * vec4
template<typename... ATerminals, typename... BTerminals>
auto operator*(const expr<svec4, ATerminals...> &a,
               const expr<svec4, BTerminals...> &b) -> decltype(result_type<svec4>::combine_bin(op_type::mul, a, b))
{
    return result_type<svec4>::combine_bin(op_type::mul, a, b);
}

// texture sample
template<typename... ATerminals, typename... BTerminals>
auto texture(const expr<stex2d, ATerminals...> &a,
              const expr<svec2, BTerminals...> &b) -> decltype(result_type<svec4>::combine_bin(op_type::sample, a, b))
{
    return result_type<svec4>::combine_bin(op_type::sample, a, b);
}

// extract component
template</*typename ExprType, */typename... ATerminals>
auto extract_r(const expr<svec4, ATerminals...> &a) -> decltype(result_type<sfloat>::combine_uni(op_type::extract0, a))
{
    //static_assert(is_in<svec4, typelist<svec2, svec3, svec4>>::value, "expression value type must be vector to extract first component `r`");
    return result_type<sfloat>::combine_uni(op_type::extract0, a);
}

// make vector
template<typename... ATerminals, typename... BTerminals>
auto make_vec2(const expr<sfloat, ATerminals...> &a,
               const expr<sfloat, BTerminals...> &b) -> decltype(result_type<svec2>::combine_bin(op_type::makevec2, a, b))
{
    //static_assert(is_in<svec4, typelist<svec2, svec3, svec4>>::value, "expression value type must be vector to extract first component `r`");
    return result_type<svec2>::combine_bin(op_type::makevec2, a, b);
}

template<typename... ATerminals, typename... BTerminals, typename... CTerminals>
auto make_vec3(const expr<sfloat, ATerminals...> &a,
               const expr<sfloat, BTerminals...> &b,
               const expr<sfloat, CTerminals...> &c ) -> decltype(result_type<svec3>::combine_tri(op_type::makevec3, a, b, c))
{
    return result_type<svec3>::combine_tri(op_type::makevec3, a, b, c);
}

template<typename... ATerminals, typename... BTerminals, typename... CTerminals, typename... DTerminals>
auto make_vec4(const expr<sfloat, ATerminals...> &a,
               const expr<sfloat, BTerminals...> &b,
               const expr<sfloat, CTerminals...> &c,
               const expr<sfloat, DTerminals...> &d ) -> decltype(result_type<svec4>::combine_quat(op_type::makevec4, a, b, c, d))
{
    return result_type<svec4>::combine_quat(op_type::makevec4, a, b, c, d);
}

// partial expr specialization for "member" operators
template<typename ...Uniforms, typename ...Attribs>
class expr<svec4, uniforms<Uniforms...>, attribs<Attribs...>> : public expr_base
{
    friend struct make_expr_helper<svec4, uniforms<Uniforms...>, attribs<Attribs...>>;
    expr() {}
public:
    expr<sfloat, uniforms<Uniforms...>, attribs<Attribs...>> r() { return extract_r(*this); }
};

template<>
class expr<sfloat, uniforms< >, attribs< >> : public expr_base
{
    friend struct make_expr_helper<sfloat, uniforms< >, attribs< >>;
    expr() {}
public:
    expr(float l) { expr_base::init(op_type::none, node_type::literal, sfloat::valtype, l); }
};

/*template<typename... Terminals>
struct expr<sfloat, Terminals...>
{
    friend struct make_expr_helper<sfloat, Terminals...>;
    expr() {}
public:
    expr(float l) { expr_base::init(op_type::none, node_type::literal, sfloat::valtype, l); }
};*/

//========================//
//      Make functions    //
//========================//
template<typename ValueType, typename UniformTag>
expr<ValueType, uniforms<
        uniform<ValueType, UniformTag> >,
     attribs< > >
make_expr(const uniform<ValueType, UniformTag> &u)
{
    return make_expr_helper<ValueType, uniforms<
               uniform<ValueType, UniformTag> >,
           attribs< > >::make_0ary_expr(node_type::uniform, ValueType::valtype);
}

template<typename ValueType, typename AttribTag>
expr<ValueType, uniforms< >,
         attribs<
            attrib<ValueType, AttribTag> > >
make_expr(const attrib<ValueType, AttribTag> &a)
{
    return make_expr_helper<ValueType, uniforms< >,
                attribs<
                    attrib<ValueType, AttribTag> >
                >::make_0ary_expr(node_type::attrib, ValueType::valtype);
}

expr<sfloat, uniforms< >,
             attribs < > >
make_expr(float l)
{
    auto out = make_expr_helper<sfloat, uniforms< >,
                                        attribs < >
                >::make_0ary_expr(node_type::literal, sfloat::valtype);
    out.literal_ = l;
    return out;
}

expr<sfloat, uniforms< >,
             attribs < > >
inline lit(float l) { return make_expr(l); }

//===========================//
// get text output functions //
//===========================//

//{ none, add, sub, mul, div, sample, extract0, makevec2, makevec3, makevec4};

std::string get_expr_string(const expr_base *e_ptr, int n_uni = 0, int n_attr = 0)
{
    const expr_base &e = *e_ptr;
    std::string out = "";
    switch (e.op_)
    {
    case(op_type::none):
        {
            DEBUG_ASSERT(e.operands_.size()==0 && e.nt_!=node_type::internal);
            if (e.nt_ == node_type::literal) { out = std::to_string(e.literal_);    }
            if (e.nt_ == node_type::uniform) { out = "uni" +std::to_string(n_uni);  }
            if (e.nt_ == node_type::attrib ) { out = "attr"+std::to_string(n_attr); }
        }
        break;
    case(op_type::add):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_expr_string(e.operands_[0])+"+"+get_expr_string(e.operands_[1]);
        }
        break;
    case(op_type::sub):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_expr_string(e.operands_[0])+"-"+get_expr_string(e.operands_[1]);
        }
        break;
    case(op_type::mul):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_expr_string(e.operands_[0])+"*"+get_expr_string(e.operands_[1]);
        }
        break;
    case(op_type::div):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = get_expr_string(e.operands_[0])+"/"+get_expr_string(e.operands_[1]);
        }
        break;
    case(op_type::sample):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = "texture("+get_expr_string(e.operands_[0])+","+get_expr_string(e.operands_[1])+")";
        }
        break;
    case(op_type::extract0):
        {
            DEBUG_ASSERT(e.operands_.size()==1);
            out = get_expr_string(e.operands_[0])+".x";
        }
        break;
    case(op_type::makevec2):
        {
            DEBUG_ASSERT(e.operands_.size()==2);
            out = "vec2("+get_expr_string(e.operands_[0])+","+get_expr_string(e.operands_[1])+")";
        }
        break;
    case(op_type::makevec3):
        {
            DEBUG_ASSERT(e.operands_.size()==3);
            out = "vec3("+get_expr_string(e.operands_[0])+","+get_expr_string(e.operands_[1])+","+get_expr_string(e.operands_[2])+")";
        }
        break;
    case(op_type::makevec4):
        {
            DEBUG_ASSERT(e.operands_.size()==4);
            out = "vec4("+get_expr_string(e.operands_[0])+","+get_expr_string(e.operands_[1])+","+get_expr_string(e.operands_[2])+","+get_expr_string(e.operands_[3])+")";
        }
        break;
    default:
        DEBUG_ASSERT(false&&"unknown op type");
    }
    // add braces
    out = "("+out+")";
    return out;
}

//std::string get_expr_string_rec(const expr_base &e, int &n_uni, int n_attr = 0)


//} // namespace expr

#endif // EXPR_H
