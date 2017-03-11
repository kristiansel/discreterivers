#ifndef SUBMENUS_H
#define SUBMENUS_H

#include "gui.h"
#include "../graphics/guirender/guinode.h"

namespace gui {

void createMainMenu(GUI &gui_obj, gfx::gui::GUINode &main_menu_root);

void createNewGameMenu(GUI &gui_obj, gfx::gui::GUINode &new_game_menu_root);

void createSaveGameMenu(GUI &gui_obj, gfx::gui::GUINode &savegame_menu_root);

void createLoadGameMenu(GUI &gui_obj, gfx::gui::GUINode &loadgame_menu_root);

void createOptionsMenu(GUI &gui_obj, gfx::gui::GUINode &options_menu_root);

void createProfilingPane(GUI &gui_obj, gfx::gui::GUINode &profiling_pane_root);

void createChooseOriginMenu(GUI &gui_obj, gfx::gui::GUINode &choose_origin_menu_root);

void createCharCreateMenu(GUI &gui_obj, gfx::gui::GUINode &char_create_menu_root);

}


#endif // SUBMENUS_H
