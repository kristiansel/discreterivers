#ifndef SHXOPENGL_H
#define SHXOPENGL_H

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include <utility>
#include <tuple>
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
//vertex_shader<Ts...> make_vertex_shader(const out<expr<vec4_t>, expr<Ts>...> &out_vars)
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

    //const std::string &expr_str = out_strs[0];

    std::vector<val_type> out_val_types = { vec4_t::valtype, Ts::valtype... };

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

    template<typename GeomType, typename MatType,  typename ... ExtraUnisTs, typename ... IOTypes>
    static shader_program create_from_functions(
            const std::function<out<expr<vec4_t>, IOTypes...  >(GeomType g, shx::uniform<ExtraUnisTs...> unis)> &vertex_shader,
            const std::function<out<expr<vec4_t>>(IOTypes... g, MatType  m, shx::uniform<ExtraUnisTs...> unis)> &fragment_shader);

};

template<typename T>
struct function_traits;

template<typename R, typename ...Args>
struct function_traits<std::function<R(Args...)>>
{
    static const std::size_t n_args = sizeof...(Args);

    using result_type = R;

    template <std::size_t i>
    struct arg
    {
        using type = typename std::tuple_element<i, std::tuple<Args...>>::type ;
    };
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


template<typename ...Ts>
struct typelist_traits
{
    static const std::size_t n_types = sizeof...(Ts);

    template <std::size_t i>
    struct types
    {
        using type = typename std::tuple_element<i, std::tuple<Ts...>>::type ;
    };
};


template<typename GeomType, typename MatType,  typename ... ExtraUnisTs, typename ... IOTypes>
static shader_program create_from_functions(
        const std::function<out<expr<vec4_t>, IOTypes...  >(GeomType g, ExtraUnisTs... unis)> vertex_shader,
        const std::function<out<expr<vec4_t>>(IOTypes... g, MatType  m, ExtraUnisTs... unis)> fragment_shader)
{
    shader_program sp_out;
    return sp_out;
}


template<typename ShaderImpl, typename GeometryType, typename MaterialType, typename VarsInOut, typename ExtraUniforms>
class shader;

template<typename ShaderImpl, typename... GeomTypes, typename... MatTypes, typename... IOTypes, typename... ExtraUniTypes>
class shader<ShaderImpl, geometry<GeomTypes...>, material<MatTypes...>, iovars<IOTypes...>, uniforms<ExtraUniTypes...>>
{
public:
    shader()
    {
        geometry<GeomTypes...>      geometry;
        material<MatTypes...>       material;
        iovars<IOTypes...>         varsinout;
        uniforms<ExtraUniTypes...> extraunis;

        ShaderImpl &shader_impl = static_cast<ShaderImpl&>(*this);

        int n[sizeof...(IOTypes)];

    }
};



} // namespace opengl

} // namespace shx

#endif // SHXOPENGL_H
