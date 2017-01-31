#ifndef GUIFONTMANAGER_H
#define GUIFONTMANAGER_H

#include <unordered_map>

#include "guifont.h"

namespace gfx {

namespace gui {

// how should this be used?

/*
// TODO: Rename files to "fontmanager.h" etc... including the renderer
class FontManager
{
public:

    inline FontManager &get()
    {
        static FontManager sFontManager(
            std::unordered_map<std::string, GUIFont>(
                "default", ""
        ));
        return sFontManager;
    }

    // add fonts to the manager with key to font hame and where to find the file name.

private:
    FontManager(std::unordered_map<std::string, GUIFont> &&font_map) : mFontRendererMap(font_map) {}

    std::unordered_map<std::string, GUIFont> mFontRendererMap;

};
*/

} // namespace gui

} // namespace gfx

#endif // GUIFONTMANAGER_H
