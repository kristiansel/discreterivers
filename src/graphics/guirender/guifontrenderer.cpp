#include "guifontrenderer.h"

namespace gfx {

namespace gui {

GUIFontRenderer::GUIFontRenderer(const char * font_file_name, unsigned int size) :
    mTexAtlas(createTextureAtlas(font_file_name, size, mTexAtlasPosInfo, mLineHeight))

{
    FT_Library &ft_library = mFTlibary;
    if(FT_Init_FreeType(&ft_library)) {
        assert(false&&"Could not init freetype library");
    }

    FT_Face &face = mFontFace;
    if(FT_New_Face(ft_library, font_file_name, 0, &face)) {
        //std::cout << "font_file_name: " << font_file_name << std::endl;
        assert(false&&"Couldn't load font, check font file name");
    }

    FT_Set_Pixel_Sizes(face, 0, size); // change size later

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

        mGlyphDrawInfo[character] = { glyph->bitmap_left, glyph->bitmap_top,
                                      { glyph->bitmap.width, glyph->bitmap.rows },
                                      { glyph->advance.x, glyph->advance.y } };
    }
}

Texture GUIFontRenderer::getTextureAtlas() const
{
    return mTexAtlas;
}


Texture GUIFontRenderer::createTextureAtlas(const char * font_file_name,
                                            unsigned int size,
                                            std::unordered_map<char, TexAtlasPos> &tex_atlas_pos_info_out,
                                            unsigned int &line_height)
{
    FT_Library ft_library;
    if(FT_Init_FreeType(&ft_library)) {
        assert(false&&"Could not init freetype library");
    }

    FT_Face face;
    if(FT_New_Face(ft_library, font_file_name, 0, &face)) {
        //std::cout << "font_file_name: " << font_file_name << std::endl;
        assert(false&&"Couldn't load font, check font file name");
    }

    FT_Set_Pixel_Sizes(face, 0, size); // change size later

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
    unsigned int tex_atlas_width = max_chars_per_row * max_width;
    unsigned int tex_atlas_rows = (n_chars/max_chars_per_row + 1)*max_rows;

    // set the line height to equal the tallest character (this may not be high enough?)
    line_height = max_rows;

    std::cout << "atlas width: " << tex_atlas_width << std::endl;
    std::cout << "atlas rows: " << tex_atlas_rows << std::endl;

    std::vector<unsigned char> tex_atlas_data = std::vector<unsigned char>(tex_atlas_width*tex_atlas_rows);

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
                tex_atlas_data[at_pos_w + at_pos_r * tex_atlas_width] = bmp.buffer[w+r*bmp.pitch];
            }
        }

        tex_atlas_pos_info_out[character] = {
            { static_cast<float>(glyph_col * max_width)/static_cast<float>(tex_atlas_width),
              static_cast<float>(glyph_row * max_rows)/static_cast<float>(tex_atlas_rows) },

            { static_cast<float>(glyph_col * max_width + bmp.width)/static_cast<float>(tex_atlas_width),
              static_cast<float>(glyph_row * max_rows + bmp.rows)/static_cast<float>(tex_atlas_rows) }
        };

        if (glyph_col < max_chars_per_row-1)
        {
            glyph_col++;
        }
        else
        {
            //std::cout << "switching rows on :" << sAllowedGlyphs[i] << std::endl;
            glyph_col = 0;
            glyph_row++;
        }
    } // for (int i = 0; i < n_chars; i++)

    return Texture(&tex_atlas_data[0], tex_atlas_width, tex_atlas_rows,
                   gl_type(GL_UNSIGNED_BYTE), Texture::filter::nearest,
                   Texture::pixel_format::red, // internal format
                   Texture::pixel_format::red,
                   true);  // format
}

GUITextVertices GUIFontRenderer::render(const std::string &text, unsigned int res_x, unsigned int res_y) const
{
    /*std::cout << "in text render" << std::endl;
    std::cout << text << std::endl;*/

    // oh shit. This could be in tight loop for example for reference counter

    std::vector<vmath::Vector4> position_data;
    std::vector<gfx::TexCoords> texcoord_data;

    float x=0; float y=0; float sx = 2.0f/static_cast<float>(res_x); float sy=2.0f/static_cast<float>(res_y);

    for (const char c : text)
    {
        const auto draw_info_it = mGlyphDrawInfo.find(c);
        if (draw_info_it == mGlyphDrawInfo.end()) assert(false&&"wat1?");
        const GlyphDrawInfo &draw_info = draw_info_it->second;

        float x2 = x + draw_info.bitmap_left * sx;
        float y2 = -y - draw_info.bitmap_top * sy;
        float w = draw_info.bitmap.width * sx;
        float h = draw_info.bitmap.rows * sy;

        const auto pos_info_it = mTexAtlasPosInfo.find(c);
        if (pos_info_it == mTexAtlasPosInfo.end()) assert(false&&"wat2?");
        const TexAtlasPos &pos_info = pos_info_it->second;

        float y0 = (float)(mLineHeight) * sy; // translate text down one line...

        // quad of two triangles
        position_data.push_back(vmath::Vector4{x2,     -y0-y2,     0,    1}); // 0
        position_data.push_back(vmath::Vector4{x2 + w, -y0-y2,     0,    1}); // 1
        position_data.push_back(vmath::Vector4{x2,     -y0-y2 - h, 0,    1}); // 2
        position_data.push_back(vmath::Vector4{x2,     -y0-y2 - h, 0,    1}); // 2
        position_data.push_back(vmath::Vector4{x2 + w, -y0-y2,     0,    1}); // 1
        position_data.push_back(vmath::Vector4{x2 + w, -y0-y2 - h, 0,    1}); // 3

        texcoord_data.push_back(gfx::TexCoords{pos_info.texco_begin[0],    pos_info.texco_begin[1]});   // 0
        texcoord_data.push_back(gfx::TexCoords{pos_info.texco_end[0],      pos_info.texco_begin[1]});   // 1
        texcoord_data.push_back(gfx::TexCoords{pos_info.texco_begin[0],    pos_info.texco_end[1]});     // 2
        texcoord_data.push_back(gfx::TexCoords{pos_info.texco_begin[0],    pos_info.texco_end[1]});     // 2
        texcoord_data.push_back(gfx::TexCoords{pos_info.texco_end[0],      pos_info.texco_begin[1]});   // 1
        texcoord_data.push_back(gfx::TexCoords{pos_info.texco_end[0],      pos_info.texco_end[1]});     // 3
 // 0
        x += (draw_info.advance.x/64) * sx;
        y += (draw_info.advance.y/64) * sy;


        /*if (c == 'r' || c == 'R')
        {
            std::cout << "character (c): " << c << std::endl;
            std::cout << "draw_info.bitmap.rows: " << draw_info.bitmap.rows << std::endl;
            std::cout << "draw_info.bitmap.width: " << draw_info.bitmap.width << std::endl;
            std::cout << "draw_info.bitmap.width: " << draw_info.bitmap.width << std::endl;
            std::cout << "pos_info.texco_begin: " << pos_info.texco_begin[0] << ", " << pos_info.texco_begin[1] << std::endl;
            std::cout << "pos_info.texco_end: " << pos_info.texco_end[0] << ", " << pos_info.texco_end[1] << std::endl;
            std::cout << "v1: "; vmath::print(vmath::Vector4{x2,     -y2,     0,    1}); std::cout << std::endl;
            std::cout << "v2: "; vmath::print(vmath::Vector4{x2 + w, -y2,     0,    1}); std::cout << std::endl;
            std::cout << "v3: "; vmath::print(vmath::Vector4{x2,     -y2 - h, 0,    1}); std::cout << std::endl;
            std::cout << "v4: "; vmath::print(vmath::Vector4{x2 + w, -y2 - h, 0,    1}); std::cout << std::endl;
        }*/
    }

    return GUITextVertices(position_data, texcoord_data);
}

} // namespace gui

} // namespace gfx
