#ifndef MATERIAL_H
#define MATERIAL_H

#include "../common/gfx_primitives.h"
#include "texture.h"

namespace gfx {

class Material
{
public:
    inline explicit Material(const vmath::Vector4 &color_in);
    inline explicit Material(const char * texture_file);

    // get
    inline const vmath::Vector4 &getColor() const {return mColor;}
    inline const Texture &getTexture() const {return mTexture;}

    inline static Material VertexColors(const std::vector<float> &vertex_colors,
                                 std::vector<gfx::TexCoords> &tex_coords_out);

private:
    Material() {}
    vmath::Vector4 mColor;
    Texture mTexture;
};

/*inline Material::Material(const vmath::Vector4 &color_in) :
    mColor(color_in), mTexture(Texture())
{

}*/

inline Material::Material(const vmath::Vector4 &color_in) :
    mColor(color_in), mTexture(color_in)
{

}

inline Material::Material(const char * texture_file) :
    mColor{0, 0, 0, 1}, mTexture(texture_file)
{

}


inline Material Material::VertexColors(const std::vector<float> &vertex_colors,
                                std::vector<gfx::TexCoords> &tex_coords_out)
{
    tex_coords_out = std::vector<gfx::TexCoords>(vertex_colors.size());

    Material mat_out;
    mat_out.mColor = {0, 0, 0, 1}; // unused

    float pixels[] = {
        0.0, 0.0, 1.0,  0.0, 1.0, 0.0,  1.0, 0.0, 0.0
    };

    int w = 3;
    int h = 1;

    mat_out.mTexture = Texture(pixels, w, h, gl_type(GL_FLOAT));

    float vc_max = std::numeric_limits<float>::min();
    float vc_min = std::numeric_limits<float>::max();
    for (int i = 0; i<vertex_colors.size(); i++)
    {
        vc_max = vertex_colors[i] > vc_max ? vertex_colors[i] : vc_max;
        vc_min = vertex_colors[i] < vc_min ? vertex_colors[i] : vc_min;
    }

    for (int i = 0; i<vertex_colors.size(); i++)
    {
        // magic numbers because pixel centers are not along texture edges
        float vc_scaled = 0.66666*(vertex_colors[i]-vc_min)/(vc_max-vc_min) + 0.1666666;
        //tex_coords_out[i] = {vc_scaled, 0.5f};

        tex_coords_out[i] = {vc_scaled, 0.5f};
    }

    return mat_out;
}

}

#endif // MATERIAL_H
