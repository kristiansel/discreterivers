#ifndef MATERIAL_H
#define MATERIAL_H

#include "../common/gfx_primitives.h"
#include "texture.h"

namespace gfx {

class Material
{
public:
    inline explicit Material(const vmath::Vector4 &color_in);

    // get
    inline const vmath::Vector4 &getColor() const {return mColor;}
    inline const Texture &getTexture() const {return mTexture;}

private:
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


}

#endif // MATERIAL_H
