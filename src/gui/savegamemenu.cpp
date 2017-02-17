#ifndef SAVEMENU_H
#define SAVEMENU_H

#include "submenus.h"

#include "guistyling.h"
#include "createbutton.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

using namespace gfx::gui;

void createSaveGameMenu(GUI &gui, GUINode &savegame_menu_root)
{
    const GUIFont &font = gui.getDefaultFont();

    vmath::Vector4 color_gui_base = styling::colorGuiBase();

    GUINodeHandle savegame_bg_node = savegame_menu_root.addGUINode(GUITransform({0.50f, 0.50f}, {0.39f, 0.66f}));

    savegame_bg_node->addElement( BackgroundElement( vmath::Vector4( 1*color_gui_base ) ) );
    savegame_bg_node->hide();

    events::Immediate::add_callback<events::SaveGameEvent>(
        [savegame_bg_node] (const events::SaveGameEvent &evt) { savegame_bg_node->show(); }
    );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [savegame_bg_node] (const events::ToggleMainMenuEvent &evt) { savegame_bg_node->hide(); }
    );

    GUINodeHandle title_node = savegame_bg_node->addGUINode(
        GUITransform( {0.5f, 0.1f}, {0.5f, 0.10f} ));

    title_node->addElement( TextElement( "Save game", font) );

    for (int i = 0; i<7; i++)
    {
        createButton(savegame_bg_node, "New save", font, 0.5f, 0.2f+i*0.1f, 180.0f, []()
        {
            std::cout << "Clicked new save!"      << std::endl;
        });
    }

    createButton(savegame_bg_node, "Back", font,
                 HorzPos(0.04f, Units::Relative, HorzAnchor::Left),
                 VertPos(0.96f, Units::Relative, VertAnchor::Bottom),
                 180.0f,
                 [](){ events::Immediate::broadcast(events::ToggleMainMenuEvent()); });
}

} // namespace gui

#endif // SAVEMENU_H
