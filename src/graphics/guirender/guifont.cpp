#include "guifont.h"

namespace gfx {

namespace gui {

GUIFont::GUIFont(const char * font_file_name, unsigned int size, unsigned int dpi) :
    mTexAtlas(vmath::Vector4{1.0, 1.0, 1.0, 1.0})

{
    unsigned int size_dpi_scaled = float(size)/float(100) * dpi;

    FT_Library ft_library;
    if(FT_Init_FreeType(&ft_library)) {
        assert(false&&"Could not init freetype library");
    }

    FT_Face face;
    if(FT_New_Face(ft_library, font_file_name, 0, &face)) {
        //std::cout << "font_file_name: " << font_file_name << std::endl;
        assert(false&&"Couldn't load font, check font file name");
    }

    FT_Set_Pixel_Sizes(face, 0, size_dpi_scaled); // change size later

    int n_chars = strlen( sAllowedGlyphs );

    unsigned int max_width = 0;
    unsigned int max_rows = 0;

    for (int i = 0; i < n_chars; i++)
    {
        char character = sAllowedGlyphs[i];
        if(FT_Load_Char(face, character, FT_LOAD_RENDER))
        {
            std::cerr << "Could not load character `" << character << "`" << std::endl;
            assert(false);
        }
        FT_GlyphSlot glyph = face->glyph;

        // populate the drawinfo
        mGlyphDrawInfo[character] = { glyph->bitmap_left, glyph->bitmap_top,
                                    { glyph->bitmap.width, glyph->bitmap.rows },
                                    { glyph->advance.x, glyph->advance.y } };

        // populate texture atlas info
        auto &bmp = glyph->bitmap;
        max_width   = bmp.width >   max_width   ? bmp.width : max_width;
        max_rows    = bmp.rows  >   max_rows    ? bmp.rows  : max_rows;
    }

    int max_chars_per_row = 10;
    unsigned int tex_atlas_width = max_chars_per_row * max_width;
    unsigned int tex_atlas_rows = (n_chars/max_chars_per_row + 1)*max_rows;

    // set the line height to equal the tallest character (this may not be high enough?)
    mLineHeight = max_rows;
    mMonoWidth = max_width;

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

        mTexAtlasPosInfo[character] = {
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

    mTexAtlas = Texture(&tex_atlas_data[0], tex_atlas_width, tex_atlas_rows,
                   gl_type(GL_UNSIGNED_BYTE), Texture::filter::nearest,
                   Texture::pixel_format::red, // internal format
                   Texture::pixel_format::red,
                   true);  // format
}

Texture GUIFont::getTextureAtlas() const
{
    return mTexAtlas;
}

GUITextVertices GUIFont::render(const std::string &text) const
{
    /*std::cout << "in text render" << std::endl;
    std::cout << text << std::endl;*/

    // oh shit. This could be in tight loop for example for reference counter

    unsigned int res_x = GUIFont::StdResolution::width;
    unsigned int res_y = GUIFont::StdResolution::height;

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

void GUIFont::updateText(const char * text, std::vector<vmath::Vector4> &position_data, std::vector<gfx::TexCoords> &texcoord_data) const
{

    unsigned int res_x = GUIFont::StdResolution::width;
    unsigned int res_y = GUIFont::StdResolution::height;

    float x=0; float y=0; float sx = 2.0f/static_cast<float>(res_x); float sy=2.0f/static_cast<float>(res_y);

    for (int i = 0; text[i]!=0; i++)
    {
        char c = text[i];
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
}

void GUIFont::updateTextPtrs(const char * text, vmath::Vector4 * position_data, gfx::TexCoords * texcoord_data) const
{

    unsigned int res_x = GUIFont::StdResolution::width;
    unsigned int res_y = GUIFont::StdResolution::height;

    float x=0; float y=0; float sx = 2.0f/static_cast<float>(res_x); float sy=2.0f/static_cast<float>(res_y);

    for (int i = 0; text[i]!=0; i++)
    {
        char c = text[i];
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
        position_data[i*6+0] = vmath::Vector4{x2,     -y0-y2,     0,    1}; // 0
        position_data[i*6+1] = vmath::Vector4{x2 + w, -y0-y2,     0,    1}; // 1
        position_data[i*6+2] = vmath::Vector4{x2,     -y0-y2 - h, 0,    1}; // 2
        position_data[i*6+3] = vmath::Vector4{x2,     -y0-y2 - h, 0,    1}; // 2
        position_data[i*6+4] = vmath::Vector4{x2 + w, -y0-y2,     0,    1}; // 1
        position_data[i*6+5] = vmath::Vector4{x2 + w, -y0-y2 - h, 0,    1}; // 3

        texcoord_data[i*6+0] = gfx::TexCoords{pos_info.texco_begin[0],    pos_info.texco_begin[1]};   // 0
        texcoord_data[i*6+1] = gfx::TexCoords{pos_info.texco_end[0],      pos_info.texco_begin[1]};   // 1
        texcoord_data[i*6+2] = gfx::TexCoords{pos_info.texco_begin[0],    pos_info.texco_end[1]};     // 2
        texcoord_data[i*6+3] = gfx::TexCoords{pos_info.texco_begin[0],    pos_info.texco_end[1]};     // 2
        texcoord_data[i*6+4] = gfx::TexCoords{pos_info.texco_end[0],      pos_info.texco_begin[1]};   // 1
        texcoord_data[i*6+5] = gfx::TexCoords{pos_info.texco_end[0],      pos_info.texco_end[1]};     // 3
 // 0
        x += (draw_info.advance.x/64) * sx;
        y += (draw_info.advance.y/64) * sy;
    }
}


} // namespace gui

} // namespace gfx
