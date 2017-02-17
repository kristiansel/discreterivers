#ifndef GUIFONT_H
#define GUIFONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
// what tha fuark... is this?

#include <iostream>
#include <unordered_map>


#include "guitextvertices.h"
#include "guitransform.h"
#include "../texture.h"
#include "../../common/macro/macrodebugassert.h"


namespace gfx {

namespace gui {


class GUIFont
{
public:

    struct StdResolution
    {
        static const int width = 1080;
        static const int height = 1920;
    };

    GUIFont(const char * font_file_name, unsigned int size);

    struct GlyphDrawInfo {
        int bitmap_left;
        int bitmap_top;
        struct { unsigned int width, rows; } bitmap;
        struct { long x, y; } advance;
    };

    inline GlyphDrawInfo getGlyphDrawInfo(char glyph);

    Texture getTextureAtlas() const;
    GUITextVertices render(const std::string &text) const;

    // could go full template here, and use char array and tex coords array of related length?
    void updateText(const char * text, std::vector<vmath::Vector4> &points, std::vector<TexCoords> &tex_coords) const;
    void updateTextData(const char * text, vmath::Vector4 * position_data, gfx::TexCoords * texcoord_data,
                        unsigned int max_pixel_width = 1200) const;

    //static GUITextVertices renderText(const GUIFont &font, const std::string &text);

private:
    /*FT_Library mFTlibary;
    FT_Face mFontFace;*/

    // non-literals location --------------------------vv----vv-----------------------------------------------------------------------------------------vv
    static constexpr char const * sAllowedGlyphs = "' !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n";

    std::unordered_map<char, GlyphDrawInfo> mGlyphDrawInfo;

    struct TexAtlasPos {
        std::array<float, 2> texco_begin;
        std::array<float, 2> texco_end;
    };

    std::unordered_map<char, TexAtlasPos> mTexAtlasPosInfo;

    unsigned int mLineHeight;
    unsigned int mMonoWidth;

    Texture mTexAtlas;
};

} // namespace gui

} // namespace gfx


#endif // GUIFONT_H
