#ifndef LOADGAMEMENU_H
#define LOADGAMEMENU_H

#include "submenus.h"

#include "guistyling.h"
#include "components/createbutton.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

using namespace gfx::gui;

void createLoadGameMenu(GUI &gui, GUINode &loadgame_menu_root)
{
    const GUIFont &font = gui.getDefaultFont();

    vmath::Vector4 color_gui_base = styling::colorGuiBase();

    GUINodeHandle loadgame_bg_node = loadgame_menu_root.addGUINode(GUITransform({0.50f, 0.50f}, {0.39f, 0.66f}));

    loadgame_bg_node->addElement( BackgroundElement( vmath::Vector4( 1*color_gui_base ) ) );
    loadgame_bg_node->hide();

    events::Immediate::add_callback<events::LoadGameEvent>(
        [loadgame_bg_node] (const events::LoadGameEvent &evt) { loadgame_bg_node->show(); }
    );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [loadgame_bg_node] (const events::ToggleMainMenuEvent &evt) { loadgame_bg_node->hide(); }
    );

    GUINodeHandle title_node = loadgame_bg_node->addGUINode(
        GUITransform( {0.5f, 0.1f}, {0.5f, 0.10f} ));

    title_node->addElement( TextElement( "Load game", font) );

    /*for (int i = 0; i<7; i++)
    {
        createButton(loadgame_bg_node, "New load", font, 0.5f, 0.2f+i*0.1f, 180.0f, []()
        {
            std::cout << "Clicked new load!"      << std::endl;
        });
    }*/

    createButton(loadgame_bg_node, "Back", font,
                 HorzPos(0.04f, Units::Relative, HorzAnchor::Left),
                 VertPos(0.96f, Units::Relative, VertAnchor::Bottom),
                 180.0f,
                 [](){ events::Immediate::broadcast(events::ToggleMainMenuEvent()); });
}

} // namespace gui

#endif // LOADGAMEMENU_H
