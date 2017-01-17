#ifndef GUI_H
#define GUI_H

#include "../graphics/guirender/guinode.h"

namespace gui {

gfx::gui::GUIElementHandle createGUI(gfx::gui::GUINode &gui_root, const gfx::gui::GUIFontRenderer &font_renderer, int width, int height);

}

#endif // GUI_H
