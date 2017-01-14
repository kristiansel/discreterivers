#ifndef IMAGEELEMENT_H
#define IMAGEELEMENT_H

#include "../../texture.h"

namespace gfx {

namespace gui {

class ImageElement
{
public:
    inline ImageElement(const Texture &tex) : mTexture(tex) {}

    inline const GLuint getTextureID() const { return mTexture.getTextureID(); }

private:
    Texture mTexture;
};

}

}

#endif // IMAGEELEMENT_H
