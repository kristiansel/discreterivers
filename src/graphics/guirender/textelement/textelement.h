#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H

#include "../guitextvertices.h"
#include "../guitransform.h"
#include "../guitextobject.h"
#include "../guifont.h"
#include "../../texture.h"

namespace gfx {

namespace gui {

class TextElement
{
public:
    inline TextElement(const std::string &text, const GUIFont &font,
                       const vmath::Vector4 color = vmath::Vector4(1.0, 1.0, 1.0, 1.0))
        : TextElement(font.render(text), color) {}

    inline TextElement(const GUIFont::RenderResult& render_result, const vmath::Vector4 color = vmath::Vector4(1.0, 1.0, 1.0, 1.0))
        : mTextObject(render_result.text_object), mColor(color) {}


    inline const GUITextVertices &getGUITextVertices() const { return mTextObject.getTextVertices(); }
    inline const GLuint getFontAtlasTextureID() const { return mTextObject.getFontAtlasTexture().getTextureID(); }

    inline const vmath::Vector4 &getColor() const { return mColor; }
    inline void setColor(const vmath::Vector4 &color) { mColor = color; }

    inline void setTextRenderResult(const GUIFont::RenderResult &render_result)
    {
        mTextObject = render_result.text_object;
    }

    inline void updateText(const char * text, const GUIFont &font, unsigned int num_chars);

private:
    GUITextObject mTextObject;
    vmath::Vector4 mColor;
};


inline void TextElement::updateText(const char * text, const GUIFont &font, unsigned int num_chars)
{
    unsigned int num_verts = 6*num_chars;

    std::vector<vmath::Vector4> points(num_verts);
    std::vector<TexCoords> tex_coords(num_verts);

    //font.updateText(text, points, tex_coords);
    font.updateTextData(text, &points[0], &tex_coords[0]);

    mTextObject.updateText(&tex_coords[0], &points[0], num_verts, font.getTextureAtlas());
}


}

}

#endif // TEXTELEMENT_H
