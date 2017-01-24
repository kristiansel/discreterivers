#ifndef GUIFONTRENDERER_H
#define GUIFONTRENDERER_H

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


class GUIFontRenderer
{
public:

    struct StdResolution
    {
        static const int width = 1080;
        static const int height = 1920;
    };

    GUIFontRenderer(const char * font_file_name, unsigned int size, unsigned int dpi);

    struct GlyphDrawInfo {
        int bitmap_left;
        int bitmap_top;
        struct { unsigned int width, rows; } bitmap;
        struct { long x, y; } advance;
    };

    inline GlyphDrawInfo getGlyphDrawInfo(char glyph);

    Texture getTextureAtlas() const;

    //inline GUITextVertices bakeGUIText(std::string &&text) { return GUITextVertices({}, {}); }

    GUITextVertices render(const std::string &text) const;

private:
    FT_Library mFTlibary;
    FT_Face mFontFace;

    // non-literals location --------------------------vv----vv-----------------------------------------------------------------------------------------vv
    static constexpr char const * sAllowedGlyphs = "' !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n";

    std::unordered_map<char, GlyphDrawInfo> mGlyphDrawInfo;

    struct TexAtlasPos {
        std::array<float, 2> texco_begin;
        std::array<float, 2> texco_end;
    };

    std::unordered_map<char, TexAtlasPos> mTexAtlasPosInfo;
    unsigned int mLineHeight;

    Texture mTexAtlas;

    static Texture createTextureAtlas(const char * font_file_name,
                                      unsigned int size,
                                      unsigned int dpi,
                                      std::unordered_map<char, TexAtlasPos> &tex_atlas_pos_out,
                                      unsigned int &line_height_out);
};

} // namespace gui

} // namespace gfx


#endif // GUIFONTRENDERER_H
