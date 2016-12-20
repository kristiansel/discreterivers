#include "guifontrenderer.h"

namespace gfx {

namespace gui {

GUIFontRenderer::GUIFontRenderer()
{
    FT_Library ft_library;
    if(FT_Init_FreeType(&ft_library)) {
        assert(false&&"Could not init freetype library");
    }

    FT_Face face;
    if(FT_New_Face(ft_library, "IMFePIrm28P.ttf", 0, &face)) {
        assert(false&&"Could load font `IMFePIrm28P.ttf`");
    }

    FT_Set_Pixel_Sizes(face, 0, 28); // change size later

    int len = strlen( sAllowedGlyphs );

    for (int i = 0; i < len; i++)
    {
        if(FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        {
            assert(false&&"Could not load character `X`");
        }
        FT_GlyphSlot glyph = face->glyph;

        mGlyphTextures.emplace_back(glyph->bitmap.buffer, glyph->bitmap.width, glyph->bitmap.rows,
                                    gl_type(GL_UNSIGNED_BYTE), Texture::filter::nearest);
    }
}

} // namespace gui

} // namespace gfx

