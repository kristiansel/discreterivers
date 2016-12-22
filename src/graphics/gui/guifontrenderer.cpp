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

    FT_Set_Pixel_Sizes(face, 0, 192); // change size later

    int len = strlen( sAllowedGlyphs );

    for (int i = 0; i < len; i++)
    {
        //assert(sAllowedGlyphs[i] <= 255);
        if(FT_Load_Char(face, sAllowedGlyphs[i], FT_LOAD_RENDER))
        {
            std::cerr << "Could not load character `" << sAllowedGlyphs[i] << "`" << std::endl;
            assert(false);
        }
        FT_GlyphSlot glyph = face->glyph;

        mGlyphTextures[sAllowedGlyphs[i]] = Texture(glyph->bitmap.buffer, glyph->bitmap.width, glyph->bitmap.rows,
                                    gl_type(GL_UNSIGNED_BYTE), Texture::filter::nearest,
                                    Texture::pixel_format::red, // internal format
                                    Texture::pixel_format::red);  // format
    }
}

Texture GUIFontRenderer::getTexture(char glyph)
{
    auto search = mGlyphTextures.find(glyph);
    if(search != mGlyphTextures.end())
    {
        return search->second;
    }
    else
    {
        assert(false&&"could not find texture for glyph");
    }
}

} // namespace gui

} // namespace gfx

