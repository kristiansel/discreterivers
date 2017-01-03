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
    GUIFontRenderer(const char * font_file_name);

    struct GlyphDrawInfo {
        int bitmap_left;
        int bitmap_top;
        struct { unsigned int width, rows; } bitmap;
        struct { long x, y; } advance;
    };

    inline GlyphDrawInfo getGlyphDrawInfo(char glyph);

    Texture getTextureAtlas() const;

    //inline GUITextVertices bakeGUIText(std::string &&text) { return GUITextVertices({}, {}); }

    GUITextVertices render(const std::string &text, const GUITransform &gui_transform) const;

private:
    FT_Library mFTlibary;
    FT_Face mFontFace;

    static constexpr char const * sAllowedGlyphs = R"(' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

    std::unordered_map<char, GlyphDrawInfo> mGlyphDrawInfo;

    struct TexAtlasPos {
        std::array<float, 2> texco_begin;
        std::array<float, 2> texco_end;
    };

    std::unordered_map<char, TexAtlasPos> mTexAtlasPosInfo;

    Texture mTexAtlas;

    static Texture createTextureAtlas(const char * font_file_name, std::unordered_map<char, TexAtlasPos> &tex_atlas_pos_out);
};

} // namespace gui

} // namespace gfx


#endif // GUIFONTRENDERER_H
