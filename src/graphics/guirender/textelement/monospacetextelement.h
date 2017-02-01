#ifndef MONOSPACETEXTELEMENT_H
#define MONOSPACETEXTELEMENT_H


#include "../guitextvertices.h"
#include "../guitransform.h"
#include "../../texture.h"
#include "../guifontmanager.h"

namespace gfx {

namespace gui {

class MonospaceTextElement
{
public:
    inline MonospaceTextElement(const std::string &text, const GUIFont &font, unsigned int buffersize = 16,
                       const vmath::Vector4 color = vmath::Vector4(1.0, 1.0, 1.0, 1.0))
        : mTextVertices(font.renderMonospace(text, buffersize)), mFontTextureAtlas(font.getTextureAtlas()), mColor(color) {}

    inline const GUITextVertices &getGUITextVertices() const { return mTextVertices; }
    inline const GLuint getFontAtlasTextureID() const { return mFontTextureAtlas.getTextureID(); }
    inline const vmath::Vector4 &getColor() const { return mColor; }
    inline void setText(const char * text) {}

private:
    GUITextVertices mTextVertices;
    Texture mFontTextureAtlas;
    vmath::Vector4 mColor;

    //HorzAnchor mHorzAlign;
    //VertAnchor mVertAlign;
};

}

}

#endif // MONOSPACETEXTELEMENT_H
