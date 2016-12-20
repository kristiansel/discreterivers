#ifndef GUIFONTRENDERER_H
#define GUIFONTRENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H
// what tha fuark... is this?

#include <iostream>

#include "../texture.h"
#include "../../common/macro/macrodebugassert.h"


namespace gfx {

namespace gui {


class GUIFontRenderer
{
public:
    GUIFontRenderer();

private:
    FT_Library mFTlibary;
    FT_Face mFontFace;

    static constexpr char const * sAllowedGlyphs = R"(' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

    std::vector<Texture> mGlyphTextures;

    struct GlyphDrawInfo {

    };

    std::vector<GlyphDrawInfo> mGlyphDrawInfo;

};

} // namespace gui

} // namespace gfx


#endif // GUIFONTRENDERER_H
