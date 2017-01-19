#ifndef GUIFONTMANAGER_H
#define GUIFONTMANAGER_H

#include <unordered_map>

#include "guifontrenderer.h"

namespace gfx {

namespace gui {

// TODO: Rename files to "fontmanager.h" etc... including the renderer
class FontManager
{
public:

    inline FontManager &get()
    {
        static FontManager sFontManager;
        return sFontManager;
    }

    // add fonts to the manager with key to font hame and where to find the file name.

private:
    FontManager() {}

    std::unordered_map<std::string, GUIFontRenderer> mFontRendererMap;

};

} // namespace gui

} // namespace gfx

#endif // GUIFONTMANAGER_H
