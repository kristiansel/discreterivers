#ifndef SHEXPR_H
#define SHEXPR_H

#include <vector>

namespace shx {

enum class op_type       { none,
                           add, sub, mul, div,
                           sample,
                           get0, get1, get2, get3,
                           makevec2, makevec3, makevec4 };

enum class node_type     { internal, uniform, attribute, literal, iovar};

enum class val_type      { int_t, float_t, vec2_t, vec3_t, vec4_t, mat3_t, mat4_t, tex2d_t};

struct int_t       { static const val_type valtype = val_type::int_t;     };
struct float_t     { static const val_type valtype = val_type::float_t;   };
struct vec2_t      { static const val_type valtype = val_type::vec2_t;    };
struct vec3_t      { static const val_type valtype = val_type::vec3_t;    };
struct vec4_t      { static const val_type valtype = val_type::vec4_t;    };
struct mat3_t      { static const val_type valtype = val_type::mat3_t;    };
struct mat4_t      { static const val_type valtype = val_type::mat4_t;    };
struct tex2d_t     { static const val_type valtype = val_type::tex2d_t;   };

struct expr_info_t
{
    op_type     op_;
    node_type   nt_;
    val_type    vt_;

    std::vector<expr_info_t> operands_;

    struct {
        union {
            float literal_float;
            int   literal_int;
        };
    } terminal_;


    int         id_;

    static int generate_id()
    {
        static int guid = 0;
        return guid++;
    }

};

template<typename T>
struct expr_helper;

struct expr_base
{
protected:
    expr_base() {}
public:
    expr_base(op_type op, node_type nt, val_type vt, int id) :
       expr_info_{op, nt, vt, {}, 1.0f, id} {}

    template<typename T>
    friend struct make_expr_helper;

    expr_info_t expr_info_;
};

#define EXPR_TRAITS(T)  public:                                                                         \
                        template<typename D>                                                            \
                        friend class expr_helper;                                                       \
                                                                                                        \
                        /* implicit construct from attribute                                            \
                        expr(const attribute<T> &attr) :                                                \
                            expr_base(op_type::none, node_type::attribute, val_type(T::valtype))        \
                            { expr_info_.terminal_.term_ptr = &attr; }                                  \
                                                                                                        \
                        /* implicit construct from uniform                                              \
                        expr(const uniform<T> &uni) :                                                   \
                            expr_base(op_type::none, node_type::uniform, val_type(T::valtype))          \
                            { expr_info_.terminal_.term_ptr = &uni; }   */                              \
                                                                                                        \
                        template<typename UniType>                                                      \
                        friend class uniform;                                                           \
                                                                                                        \
                        template<typename AttrType>                                                     \
                        friend class attribute;                                                         \
                                                                                                        \
                        expr(op_type op, node_type nt, val_type vt, int id) :                           \
                            expr_base(op, nt, vt, id) {}                                                \
                                                                                                        \
                        /* not default constructible  */                                                \
                        private:                                                                        \
                        expr() {}


template<typename T>
class expr : public expr_base
{
    EXPR_TRAITS(T)
};

template<typename T>
struct expr_helper
{
    template<typename... Ts>
    static expr<T> make_nary_expr(op_type op, node_type nt, val_type vt, const expr<Ts>&... n)
    {
        expr<T> out;
        out.expr_info_.op_ = op;
        out.expr_info_.nt_ = nt;
        out.expr_info_.vt_ = vt;
        out.expr_info_.operands_ = {n.expr_info_...};
        out.expr_info_.id_ = expr_info_t::generate_id();
        return out;
    }
};

// operators

// template specializations (should macro out some of this...)
template<>
struct expr<float_t> : public expr_base
{
    EXPR_TRAITS(float_t)
public:
    expr<float_t>(const float &f)
    {
        expr_info_.op_ = op_type::none;
        expr_info_.nt_ = node_type::literal;
        expr_info_.vt_ = val_type::float_t;
        expr_info_.terminal_.literal_float = f;
        expr_info_.id_ = expr_info_t::generate_id();
    }
};

template<>
struct expr<int_t> : public expr_base
{
    EXPR_TRAITS(int_t)
public:
    expr<int_t>(const int &i)
    {
        expr_info_.op_ = op_type::none;
        expr_info_.nt_ = node_type::literal;
        expr_info_.vt_ = val_type::int_t;
        expr_info_.terminal_.literal_int = i;
        expr_info_.id_ = expr_info_t::generate_id();
    }
};

template<>
struct expr<vec4_t> : public expr_base
{
    EXPR_TRAITS(vec4_t)
public:
    expr<float_t> x() { return expr_helper<float_t>::make_nary_expr(op_type::get0, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> y() { return expr_helper<float_t>::make_nary_expr(op_type::get1, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> z() { return expr_helper<float_t>::make_nary_expr(op_type::get2, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> w() { return expr_helper<float_t>::make_nary_expr(op_type::get3, node_type::internal, val_type(float_t::valtype), *this); }

    expr<float_t> r() { return expr_helper<float_t>::make_nary_expr(op_type::get0, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> g() { return expr_helper<float_t>::make_nary_expr(op_type::get1, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> b() { return expr_helper<float_t>::make_nary_expr(op_type::get2, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> a() { return expr_helper<float_t>::make_nary_expr(op_type::get3, node_type::internal, val_type(float_t::valtype), *this); }

    expr<float_t> s() { return expr_helper<float_t>::make_nary_expr(op_type::get0, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> t() { return expr_helper<float_t>::make_nary_expr(op_type::get1, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> p() { return expr_helper<float_t>::make_nary_expr(op_type::get2, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> q() { return expr_helper<float_t>::make_nary_expr(op_type::get3, node_type::internal, val_type(float_t::valtype), *this); }
};

template<>
struct expr<vec3_t> : public expr_base
{
    EXPR_TRAITS(vec3_t)
public:
    expr<float_t> x() { return expr_helper<float_t>::make_nary_expr(op_type::get0, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> y() { return expr_helper<float_t>::make_nary_expr(op_type::get1, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> z() { return expr_helper<float_t>::make_nary_expr(op_type::get2, node_type::internal, val_type(float_t::valtype), *this); }

    expr<float_t> r() { return expr_helper<float_t>::make_nary_expr(op_type::get0, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> g() { return expr_helper<float_t>::make_nary_expr(op_type::get1, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> b() { return expr_helper<float_t>::make_nary_expr(op_type::get2, node_type::internal, val_type(float_t::valtype), *this); }

    expr<float_t> s() { return expr_helper<float_t>::make_nary_expr(op_type::get0, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> t() { return expr_helper<float_t>::make_nary_expr(op_type::get1, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> p() { return expr_helper<float_t>::make_nary_expr(op_type::get2, node_type::internal, val_type(float_t::valtype), *this); }
};

template<>
struct expr<vec2_t> : public expr_base
{
    EXPR_TRAITS(vec2_t)
public:
    expr<float_t> x() { return expr_helper<float_t>::make_nary_expr(op_type::get0, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> y() { return expr_helper<float_t>::make_nary_expr(op_type::get1, node_type::internal, val_type(float_t::valtype), *this); }

    expr<float_t> r() { return expr_helper<float_t>::make_nary_expr(op_type::get0, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> g() { return expr_helper<float_t>::make_nary_expr(op_type::get1, node_type::internal, val_type(float_t::valtype), *this); }

    expr<float_t> s() { return expr_helper<float_t>::make_nary_expr(op_type::get0, node_type::internal, val_type(float_t::valtype), *this); }
    expr<float_t> t() { return expr_helper<float_t>::make_nary_expr(op_type::get1, node_type::internal, val_type(float_t::valtype), *this); }
};

// normal operators
expr<vec4_t> inline operator*(const expr<mat4_t> &a, const expr<vec4_t> &b)
{
    return expr_helper<vec4_t>::make_nary_expr(op_type::mul, node_type::internal, val_type(vec4_t::valtype), a, b);
}

expr<vec4_t> inline operator*(const expr<vec4_t> &a, const expr<vec4_t> &b)
{
    return expr_helper<vec4_t>::make_nary_expr(op_type::mul, node_type::internal, val_type(vec4_t::valtype), a, b);
}

expr<mat4_t> inline operator*(const expr<mat4_t> &a, const expr<mat4_t> &b)
{
    return expr_helper<mat4_t>::make_nary_expr(op_type::mul, node_type::internal, val_type(mat4_t::valtype), a, b);
}

expr<float_t> inline operator*(const expr<float_t> &a, const expr<float_t> &b)
{
    return expr_helper<float_t>::make_nary_expr(op_type::mul, node_type::internal, val_type(float_t::valtype), a, b);
}


// addition
expr<vec4_t> inline operator+(const expr<vec4_t> &a, const expr<vec4_t> &b)
{
    return expr_helper<vec4_t>::make_nary_expr(op_type::add, node_type::internal, val_type(vec4_t::valtype), a, b);
}

expr<vec3_t> inline operator+(const expr<vec3_t> &a, const expr<vec3_t> &b)
{
    return expr_helper<vec3_t>::make_nary_expr(op_type::add, node_type::internal, val_type(vec3_t::valtype), a, b);
}

expr<vec2_t> inline operator+(const expr<vec2_t> &a, const expr<vec2_t> &b)
{
    return expr_helper<vec2_t>::make_nary_expr(op_type::add, node_type::internal, val_type(vec2_t::valtype), a, b);
}

expr<mat3_t> inline operator+(const expr<mat3_t> &a, const expr<mat3_t> &b)
{
    return expr_helper<mat3_t>::make_nary_expr(op_type::add, node_type::internal, val_type(mat3_t::valtype), a, b);
}

expr<mat4_t> inline operator+(const expr<mat4_t> &a, const expr<mat4_t> &b)
{
    return expr_helper<mat4_t>::make_nary_expr(op_type::add, node_type::internal, val_type(mat4_t::valtype), a, b);
}

expr<float_t> inline operator+(const expr<float_t> &a, const expr<float_t> &b)
{
    return expr_helper<float_t>::make_nary_expr(op_type::add, node_type::internal, val_type(float_t::valtype), a, b);
}

// subtraction
expr<vec4_t> inline operator-(const expr<vec4_t> &a, const expr<vec4_t> &b)
{
    return expr_helper<vec4_t>::make_nary_expr(op_type::sub, node_type::internal, val_type(vec4_t::valtype), a, b);
}

expr<vec3_t> inline operator-(const expr<vec3_t> &a, const expr<vec3_t> &b)
{
    return expr_helper<vec3_t>::make_nary_expr(op_type::sub, node_type::internal, val_type(vec3_t::valtype), a, b);
}

expr<vec2_t> inline operator-(const expr<vec2_t> &a, const expr<vec2_t> &b)
{
    return expr_helper<vec2_t>::make_nary_expr(op_type::sub, node_type::internal, val_type(vec2_t::valtype), a, b);
}

expr<mat3_t> inline operator-(const expr<mat3_t> &a, const expr<mat3_t> &b)
{
    return expr_helper<mat3_t>::make_nary_expr(op_type::sub, node_type::internal, val_type(mat3_t::valtype), a, b);
}

expr<mat4_t> inline operator-(const expr<mat4_t> &a, const expr<mat4_t> &b)
{
    return expr_helper<mat4_t>::make_nary_expr(op_type::sub, node_type::internal, val_type(mat4_t::valtype), a, b);
}

expr<float_t> inline operator-(const expr<float_t> &a, const expr<float_t> &b)
{
    return expr_helper<float_t>::make_nary_expr(op_type::sub, node_type::internal, val_type(float_t::valtype), a, b);
}


// functions
expr<vec4_t> inline texture(const expr<tex2d_t> &a, const expr<vec2_t> &b)
{
    return expr_helper<vec4_t>::make_nary_expr(op_type::sample, node_type::internal, val_type(vec4_t::valtype), a, b);
}

expr<vec4_t> inline vec4(const expr<float_t> &f0, const expr<float_t> &f1, const expr<float_t> &f2, const expr<float_t> &f3)
{
    return expr_helper<vec4_t>::make_nary_expr(op_type::makevec4, node_type::internal, val_type(vec4_t::valtype), f0, f1, f2, f3);
}

expr<vec3_t> inline vec3(const expr<float_t> &f0, const expr<float_t> &f1, const expr<float_t> &f2)
{
    return expr_helper<vec3_t>::make_nary_expr(op_type::makevec3, node_type::internal, val_type(vec3_t::valtype), f0, f1, f2);
}

expr<vec2_t> inline vec2(const expr<float_t> &f0, const expr<float_t> &f1)
{
    return expr_helper<vec2_t>::make_nary_expr(op_type::makevec2, node_type::internal, val_type(vec2_t::valtype), f0, f1);
}

// attributes and uniforms
// need to remove the overhead in uniforms and attributes so that they can be used by

template<typename T>
class uniform : public expr<T>
{
    //uniform(const uniform &u) = delete;
    //uniform(uniform &&u) = delete;
public:
    uniform() : expr<T>(op_type::none, node_type::uniform, val_type(T::valtype), expr_info_t::generate_id())
        /*{ expr<T>::expr_base::expr_info_.terminal_.term_ptr = this; }*/ {}
};

template<typename T>
class attribute : public expr<T>
{
    //attribute(const attribute &u) = delete;
    //attribute(attribute &&u) = delete;
public:
    attribute() :
        expr<T>(op_type::none, node_type::attribute, val_type(T::valtype), expr_info_t::generate_id())
        /*{ expr<T>::expr_base::expr_info_.terminal_.term_ptr = this; }*/ {}
};

template<typename T>
class iovar : public expr<T>
{
    //iovar(const attribute &u) = delete;
    //iovar(attribute &&u) = delete;
public:
    iovar() :
        expr<T>(op_type::none, node_type::iovar, val_type(T::valtype), expr_info_t::generate_id())
        /*{ expr<T>::expr_base::expr_info_.terminal_.term_ptr = this; }*/ {}
};



// generic typelists for out and in parameters
template<typename ...Ts>
class out {};

template<typename ...Ts>
out<Ts...> make_out(Ts... args) { return out<Ts...>(); }

// template<typename ...Ts>
class in {};


template<template<typename> typename ExprType, typename ...Ts>
class expr_type_list
{
public:
    static const std::size_t n_types = sizeof...(Ts);

    template <std::size_t i>
    struct comp
    {
        using type = typename std::tuple_element<i, std::tuple<Ts...>>::type ;
    };

    template <std::size_t i>
    ExprType<typename comp<i>::type> get() { return ExprType<typename comp<i>::type>(); }
};

template<typename ...Ts>
class geometry : public expr_type_list<shx::attribute, Ts...> {};

template<typename ...Ts>
class material : public expr_type_list<shx::uniform, Ts...> {};

template<typename ...Ts>
class uniforms : public expr_type_list<shx::uniform, Ts...> {};

template<typename ...Ts>
class iovars : public expr_type_list<shx::iovar, Ts...> {};


} // namespace shx








#endif // SHEXPR_H
