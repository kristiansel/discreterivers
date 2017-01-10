#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H

#include "../guitextvertices.h"
#include "../../texture.h"

namespace gfx {

namespace gui {

class TextElement
{
public:
    inline TextElement(const GUITextVertices &text_vertices, const Texture &tex_atlas)
        : mTextVertices(text_vertices), mFontTextureAtlas(tex_atlas) {}

    inline const GUITextVertices &getGUITextVertices() const { return mTextVertices; }
    inline const GLuint getFontAtlasTextureID() const { return mFontTextureAtlas.getTextureID(); }

private:
    GUITextVertices mTextVertices;
    Texture mFontTextureAtlas;
};

}

}

#endif // TEXTELEMENT_H
