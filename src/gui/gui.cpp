#include "gui.h"
#include "../events/queuedevents.h"
#include "../events/immediateevents.h"

#include "submenus.h"

namespace gui {

void createGUI(GUI &gui)
{
    gfx::gui::GUINode &gui_root_node = gui.getGUIRoot();

    createMainMenu(gui, gui_root_node);

    createNewGameMenu(gui, gui_root_node);

    createSaveGameMenu(gui, gui_root_node);

    createLoadGameMenu(gui, gui_root_node);

    createOptionsMenu(gui, gui_root_node);

    createProfilingPane(gui, gui_root_node);

}

}
