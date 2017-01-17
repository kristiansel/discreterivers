#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H

#include "../guitextvertices.h"
#include "../guitransform.h"
#include "../../texture.h"

namespace gfx {

namespace gui {

class TextElement
{
public:
    inline TextElement(const GUITextVertices &text_vertices,
                       const Texture &tex_atlas,
                       const vmath::Vector4 color = vmath::Vector4(1.0, 1.0, 1.0, 1.0))
        : mTextVertices(text_vertices), mFontTextureAtlas(tex_atlas), mColor(color) {}

    inline const GUITextVertices &getGUITextVertices() const { return mTextVertices; }
    inline const GLuint getFontAtlasTextureID() const { return mFontTextureAtlas.getTextureID(); }
    inline const vmath::Vector4 &getColor() const { return mColor; }
    inline void setTextVertices(const GUITextVertices &text_vertices) { mTextVertices = text_vertices; }

private:
    GUITextVertices mTextVertices;
    Texture mFontTextureAtlas;
    vmath::Vector4 mColor;

    //HorzAnchor mHorzAlign;
    //VertAnchor mVertAlign;
};

}

}

#endif // TEXTELEMENT_H
