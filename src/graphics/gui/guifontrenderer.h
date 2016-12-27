#ifndef GUIFONTRENDERER_H
#define GUIFONTRENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H
// what tha fuark... is this?

#include <iostream>
#include <unordered_map>

#include "../texture.h"
#include "../../common/macro/macrodebugassert.h"


namespace gfx {

namespace gui {


class GUIFontRenderer
{
public:
    GUIFontRenderer(const char * font_file_name);

    Texture getTexture(char glyph);

    struct GlyphDrawInfo {
        int bitmap_left;
        int bitmap_top;
        struct { long x, y; } advance;
    };

    inline GlyphDrawInfo getGlyphDrawInfo(char glyph);

    Texture getTextureAtlas();

private:
    FT_Library mFTlibary;
    FT_Face mFontFace;

    static constexpr char const * sAllowedGlyphs = R"(' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

    std::unordered_map<char, Texture> mGlyphTextures;

    std::unordered_map<char, GlyphDrawInfo> mGlyphDrawInfo;

    std::vector<unsigned char> mTexAtlasData;
    unsigned int mTexAtlasWidth;
    unsigned int mTexAtlasRows;

};

} // namespace gui

} // namespace gfx


#endif // GUIFONTRENDERER_H
