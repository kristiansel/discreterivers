#ifndef GUIFONT_H
#define GUIFONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
// what tha fuark... is this?

#include <iostream>
#include <unordered_map>


#include "guitextvertices.h"
#include "guitransform.h"
#include "guitextobject.h"

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

    GUIFont(const char * font_file_name, float abs_size, float scale_factor);

    struct GlyphDrawInfo {
        int bitmap_left;
        int bitmap_top;
        struct { int width, rows; } bitmap;
        struct { long x, y; } advance;
    };

    inline GlyphDrawInfo getGlyphDrawInfo(char glyph);

    Texture getTextureAtlas() const;

    struct TextSizeAbs
    {
        float w_abs;
        float h_abs;
    };

    struct RenderResult
    {
        //GUITextVertices vertices;
        //Texture texture;
        GUITextObject text_object;
        TextSizeAbs text_size;
    };

    RenderResult render(const std::string &text,
                        float w_abs = static_cast<float>(StdResolution::width), // constrained width and height of the text
                        float h_abs = static_cast<float>(StdResolution::height)) const;

    // could go full template here, and use char array and tex coords array of related length?
    TextSizeAbs updateText(const char * text, std::vector<vmath::Vector4> &points, std::vector<TexCoords> &tex_coords) const;
    TextSizeAbs updateTextData(const char * text, vmath::Vector4 * position_data, gfx::TexCoords * texcoord_data,
                        unsigned int max_pixel_width = 1200) const;

    void updateUIScaleFactor(float scale_factor);

private:
    // non-literals location --------------------------vv----vv-----------------------------------------------------------------------------------------vv
    static constexpr char const * sAllowedGlyphs = "' !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n";

    std::unordered_map<char, GlyphDrawInfo> mGlyphDrawInfo;

    struct TexAtlasPos {
        std::array<float, 2> texco_begin;
        std::array<float, 2> texco_end;
    };

    std::unordered_map<char, TexAtlasPos> mTexAtlasPosInfo;

    unsigned int mLineHeight;
    float mScaleFactor;

    Texture mTexAtlas;

    // private methods
    inline Texture createTextureAtlas(FT_Face &face, unsigned int max_width, unsigned int max_rows, unsigned int n_chars);
};

} // namespace gui

} // namespace gfx


#endif // GUIFONT_H
