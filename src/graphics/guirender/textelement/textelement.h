#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H

#include "../guitextvertices.h"
#include "../guitransform.h"
#include "../../texture.h"
#include "../guifontmanager.h"

namespace gfx {

namespace gui {

class TextElement
{
public:
    inline TextElement(const std::string &text, const GUIFont &font,
                       const vmath::Vector4 color = vmath::Vector4(1.0, 1.0, 1.0, 1.0))
        : mTextVertices(font.render(text)), mFontTextureAtlas(font.getTextureAtlas()), mColor(color)/*,
          mGUITransform({0.0f, 0.0f}, {1.0f, 1.0f})*/ {}

    inline const GUITextVertices &getGUITextVertices() const { return mTextVertices; }
    inline const GLuint getFontAtlasTextureID() const { return mFontTextureAtlas.getTextureID(); }
    inline const vmath::Vector4 &getColor() const { return mColor; }
    inline void setTextVertices(const GUITextVertices &text_vertices) { mTextVertices = text_vertices; }

    inline void updateText(const char * text, const GUIFont &font, unsigned int num_chars);

private:
    GUITextVertices mTextVertices;
    Texture mFontTextureAtlas;
    vmath::Vector4 mColor;

    //GUITransform mGUITransform;

    //HorzAnchor mHorzAlign;
    //VertAnchor mVertAlign;
};


inline void TextElement::updateText(const char * text, const GUIFont &font, unsigned int num_chars)
{
    unsigned int num_verts = 6*num_chars;

    std::vector<vmath::Vector4> points(num_verts);
    std::vector<TexCoords> tex_coords(num_verts);

    //font.updateText(text, points, tex_coords);
    font.updateTextData(text, &points[0], &tex_coords[0]);

    mTextVertices.updateText(&tex_coords[0], &points[0], num_verts);
}


}

}

#endif // TEXTELEMENT_H
