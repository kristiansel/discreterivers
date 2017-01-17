#ifndef BGELEMENT_H
#define BGELEMENT_H

#include "../../texture.h"

namespace gfx {

namespace gui {

struct BackgroundElement
{
public:
    inline BackgroundElement(const vmath::Vector4 &color)
        : mColor(color) {}

    inline const vmath::Vector4 &getColor() const { return mColor; }
    inline void setColor(const vmath::Vector4 &color) { mColor = color; }

private:
    vmath::Vector4 mColor;
};

}

}

#endif // BGELEMENT_H
