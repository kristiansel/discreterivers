#ifndef TEXTSHADERMAT_H
#define TEXTSHADERMAT_H

#include "../../../../common/shaderexpressions/shxopengl.h"

namespace gfx {

namespace gui {


/*struct TextElementGeom
{
    shx::attribute<shx::vec4_t > pos;
    shx::attribute<shx::vec2_t > texco;
};*/


/*struct TextElementMat
{
    shx::uniform<shx::tex2d_t> tex;
    shx::uniform<shx::vec4_t > color;
};*/


/*struct AdditionalUniforms
{
    shx::uniform<shx::mat4_t> mv;
};*/


using TextElementGeom = shx::geometry<
    shx::vec4_t, shx::vec2_t
>;

using TextElementMat = shx::material<
    shx::tex2d_t, shx::vec4_t
>;

using ExtraUniforms = shx::uniforms<
    shx::mat4_t
>;

using VarsInOut = shx::iovars<
    shx::vec2_t
>;


class TextElementShader : public shx::opengl::shader<TextElementShader, TextElementGeom, TextElementMat, VarsInOut, ExtraUniforms>
{
public:
    // base class makes sure that these are instantiated.?
    // how to deal with io?
    // make just one function that returns position and color? could be it.
    shx::out<shx::expr<shx::vec4_t>,
             shx::expr<shx::vec2_t>> inline vertexShader(  const TextElementGeom &geom,
                                                           const TextElementMat   &mat,
                                                           const ExtraUniforms   &unis)
    {
        shx::attribute<shx::vec4_t> pos   = geom.get<0>();
        shx::attribute<shx::vec2_t> texco = geom.get<1>();
        shx::uniform  <shx::mat4_t> mv    = unis.get<0>();

        shx::expr<shx::vec4_t > pos_out = mv * pos;
        shx::expr<shx::vec2_t > texco_out = texco;
        return shx::make_out(pos_out, texco_out);
    }

    shx::out<shx::expr<shx::vec4_t>> inline fragmentShader(  const VarsInOut       &in,
                                                             const TextElementMat &mat,
                                                             const ExtraUniforms &unis)
    {
        shx::uniform<shx::tex2d_t> tex = mat.get<0>();
        shx::uniform<shx::vec4_t > col = mat.get<1>();
        shx::iovar<shx::vec2_t>  texco = in.get<0>();

        shx::expr<shx::vec4_t >  tex_sample   = shx::texture(tex, texco);
        shx::expr<shx::float_t>  tex_sample_r = tex_sample.r();
        shx::expr<shx::vec4_t >  col_out      = shx::vec4(1.0f, 1.0f, 1.0f, tex_sample_r) * col;
        return shx::make_out(col_out);
    }

    void kjashda() { std::cout << "testing" << std::endl; }

};


} // gui

} // gfx

#endif // TEXTSHADERMAT_H
