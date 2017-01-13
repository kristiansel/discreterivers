#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H

#include "../guitextvertices.h"
#include "../../texture.h"

namespace gfx {

namespace gui {

class TextElement
{
public:
    inline TextElement(const GUITextVertices &text_vertices,
                       const Texture &tex_atlas,
                       const vmath::Vector4 color = vmath::Vector4(0.2, 0.2, 0.2, 1.0))
        : mTextVertices(text_vertices), mFontTextureAtlas(tex_atlas), mColor(color) {}

    inline const GUITextVertices &getGUITextVertices() const { return mTextVertices; }
    inline const GLuint getFontAtlasTextureID() const { return mFontTextureAtlas.getTextureID(); }
    inline const vmath::Vector4 &getColor() const { return mColor; }

private:
    GUITextVertices mTextVertices;
    Texture mFontTextureAtlas;
    vmath::Vector4 mColor;
};

}

}

#endif // TEXTELEMENT_H
