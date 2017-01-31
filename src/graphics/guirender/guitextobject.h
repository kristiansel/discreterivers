#ifndef GUITEXTOBJECT_H
#define GUITEXTOBJECT_H

#include "../texture.h"
#include "guitextvertices.h"

namespace gfx {

namespace gui {

class GUITextObject
{
    GUITextObject(const GUITextVertices &gui_text_vertices, const Texture &font_atlas_texture) :
        mTextVertices(gui_text_vertices), mFontAtlasTexture(font_atlas_texture) {}

    GUITextVertices &getTextVertices() const { return mTextVertices; }
    Texture &getFontAtlasTexture() const { return mFontAtlasTexture; }

private:
    GUITextObject() = delete;

    GUITextVertices mTextVertices;
    Texture mFontAtlasTexture;
};

#endif // GUITEXTOBJECT_H
