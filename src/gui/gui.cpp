#include "gui.h"
#include "../events/queuedevents.h"
#include "../events/immediateevents.h"

#include "mainmenu.h"
#include "profilingpane.h"

namespace gui {

namespace ImEvt = events::Immediate;
namespace QuEvt = events::Queued;

void createGUI(GUI &gui)
{
    gfx::gui::GUINode &gui_root_node = gui.getGUIRoot();

    createMainMenu(gui, gui_root_node);

    createProfilingPane(gui, gui_root_node);

}

}
