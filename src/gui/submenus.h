#ifndef SUBMENUS_H
#define SUBMENUS_H

#include "gui.h"
#include "../graphics/guirender/guinode.h"

namespace gui {

void createMainMenu(GUI &gui, gfx::gui::GUINode &main_menu_root);

void createNewGameMenu(GUI &gui, gfx::gui::GUINode &new_game_menu_root);

void createSaveGameMenu(GUI &gui, gfx::gui::GUINode &savegame_menu_root);

void createLoadGameMenu(GUI &gui, gfx::gui::GUINode &loadgame_menu_root);

void createOptionsMenu(GUI &gui, gfx::gui::GUINode &options_menu_root);

void createProfilingPane(GUI &gui, gfx::gui::GUINode &profiling_pane_root);

}


#endif // SUBMENUS_H
