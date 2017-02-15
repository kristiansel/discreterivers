#ifndef SUBMENUS_H
#define SUBMENUS_H

#include "gui.h"
#include "../graphics/guirender/guinode.h"

namespace gui {

void createMainMenu(GUI &gui, gfx::gui::GUINode &main_menu_root);
void createNewGameMenu(GUI &gui, gfx::gui::GUINode &new_game_menu_root);

}


#endif // SUBMENUS_H
