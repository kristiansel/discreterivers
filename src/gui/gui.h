#ifndef GUI_H
#define GUI_H

#include "../graphics/guirender/guinode.h"

namespace gui {

void createGUI(gfx::gui::GUINodeHandle &gui_root_hdl, const gfx::gui::GUIFontRenderer &font_renderer, int width, int height);

}

#endif // GUI_H
