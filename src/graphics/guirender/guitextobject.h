#ifndef GUITEXTOBJECT_H
#define GUITEXTOBJECT_H

#include "../texture.h"
#include "guitextvertices.h"

namespace gfx {

namespace gui {

class GUITextObject
{
public:
    GUITextObject(const GUITextVertices &gui_text_vertices, const Texture &font_atlas_texture) :
        mTextVertices(gui_text_vertices), mFontAtlasTexture(font_atlas_texture) {}

    const GUITextVertices &getTextVertices() const { return mTextVertices; }
    const Texture &getFontAtlasTexture() const { return mFontAtlasTexture; }

    inline void updateText( const gfx::TexCoords * tex_coords_ptr, const vmath::Vector4 * points_ptr, unsigned int size, const Texture &tex)
    {
        mTextVertices.updateText(tex_coords_ptr, points_ptr, size);
        mFontAtlasTexture = tex;
    }

private:
    GUITextObject() = delete;

    GUITextVertices mTextVertices;
    Texture mFontAtlasTexture;
};

}

}

#endif // GUITEXTOBJECT_H
