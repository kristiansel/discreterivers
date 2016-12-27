#include "guifontrenderer.h"

namespace gfx {

namespace gui {

GUIFontRenderer::GUIFontRenderer(const char * font_file_name)
{
    FT_Library &ft_library = mFTlibary;
    if(FT_Init_FreeType(&ft_library)) {
        assert(false&&"Could not init freetype library");
    }

    FT_Face &face = mFontFace;
    if(FT_New_Face(ft_library, font_file_name, 0, &face)) {
        assert(false&&"Could load font");
    }

    FT_Set_Pixel_Sizes(face, 0, 192); // change size later

    int n_chars = strlen( sAllowedGlyphs );

    for (int i = 0; i < n_chars; i++)
    {
        //assert(sAllowedGlyphs[i] <= 255);
        char character = sAllowedGlyphs[i];
        if(FT_Load_Char(face, character, FT_LOAD_RENDER))
        {
            std::cerr << "Could not load character `" << character << "`" << std::endl;
            assert(false);
        }
        FT_GlyphSlot glyph = face->glyph;

        auto *bmp = &(glyph->bitmap);

        /*if (character == 'r' || character == 'R')
        {
            std::cout << "character: " << character << std::endl;
            std::cout << "bmp->rows: " << bmp->rows << std::endl;
            std::cout << "bmp->width: " << bmp->width << std::endl;
            std::cout << "bmp->pitch: " << bmp->pitch << std::endl;
            std::cout << "bmp->buffer: " << std::size_t(bmp->buffer) << std::endl;
            std::cout << "bmp->pixel_mode: " << static_cast<int>(bmp->pixel_mode) << std::endl;
            std::cout << "bmp->num_grays: " << bmp->num_grays << std::endl;
        }*/

        mGlyphTextures[sAllowedGlyphs[i]] = Texture(glyph->bitmap.buffer, glyph->bitmap.width, glyph->bitmap.rows,
                                    gl_type(GL_UNSIGNED_BYTE), Texture::filter::nearest,
                                    Texture::pixel_format::red, // internal format
                                    Texture::pixel_format::red,
                                    true);  // format
    }
}

Texture GUIFontRenderer::getTextureAtlas()
{
    /*FT_Library ft_library;
    if(FT_Init_FreeType(&ft_library)) {
        assert(false&&"Could not init freetype library");
    }


    if(FT_New_Face(ft_library, font_file_path, 0, &face)) {
        assert(false&&"Could load font `IMFePIrm28P.ttf`");
    }


    FT_Library &ft_library = mFTlibary;

    */
    FT_Face &face = mFontFace;
    FT_Set_Pixel_Sizes(face, 0, 96); // change size later


    int n_chars = strlen( sAllowedGlyphs );

    // create texture atlas
    // first find max height and width
    unsigned int max_width = 0;
    unsigned int max_rows = 0;
    for (int i = 0; i < n_chars; i++)
    {
        char character = sAllowedGlyphs[i];
        if(FT_Load_Char(face, character, FT_LOAD_RENDER))
            assert(false&&"GUIFontRenderer::getTextureAtlas failed to load font character");

        auto &bmp = face->glyph->bitmap;
        max_width   = bmp.width >   max_width   ? bmp.width : max_width;
        max_rows    = bmp.rows  >   max_rows    ? bmp.rows  : max_rows;
    }
    int max_chars_per_row = 10;
    mTexAtlasWidth = max_chars_per_row * max_width;
    mTexAtlasRows= (n_chars/max_chars_per_row + 1)*max_rows;

    std::cout << "atlas width: " << mTexAtlasWidth << std::endl;
    std::cout << "atlas rows: " << mTexAtlasRows << std::endl;

    mTexAtlasData = std::vector<unsigned char>(mTexAtlasWidth*mTexAtlasRows);

    int glyph_row = 0;
    int glyph_col = 0;

    for (int i = 0; i < n_chars; i++)
    {
        char character = sAllowedGlyphs[i];
        FT_Load_Char(face, character, FT_LOAD_RENDER); // assume OK
        auto &bmp = face->glyph->bitmap;

        for (int r = 0; r<bmp.rows; r++)
        {
            for (int w = 0; w<bmp.width; w++)
            {
                int at_pos_r = glyph_row*max_rows+r;
                int at_pos_w = glyph_col*max_width+w;
                mTexAtlasData[at_pos_w + at_pos_r * mTexAtlasWidth] = bmp.buffer[w+r*bmp.pitch];
            }
        }

        if (glyph_col < max_chars_per_row)
        {
            glyph_col++;
        }
        else
        {
            glyph_col = 0;
            glyph_row++;
        }
    }

    return Texture(&mTexAtlasData[0], mTexAtlasWidth, mTexAtlasRows,
                   gl_type(GL_UNSIGNED_BYTE), Texture::filter::nearest,
                   Texture::pixel_format::red, // internal format
                   Texture::pixel_format::red,
                   true);  // format
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
