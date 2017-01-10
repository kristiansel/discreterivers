#ifndef BGELEMENT_H
#define BGELEMENT_H

#include "../../texture.h"

namespace gfx {

namespace gui {

struct BackgroundElement
{
public:
    inline BackgroundElement(const vmath::Vector4 &color, const Texture &tex = Texture(vmath::Vector4(1.0, 0.0, 0.0, 1.0)))
        : mColor(color), mTexture(tex) {}

    inline const vmath::Vector4 &getColor() const { return mColor; }
    inline const GLuint getTextureID() const { return mTexture.getTextureID(); }

private:
    vmath::Vector4 mColor;
    Texture mTexture;
};

}

}

#endif // BGELEMENT_H
