#ifndef LOADGAMEMENU_H
#define LOADGAMEMENU_H

#include "gui.h"
#include "guistyling.h"
#include "createbutton.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

inline void createLoadGameMenu(GUI &gui, gfx::gui::GUINode &loadgame_menu_root)
{
    const gfx::gui::GUIFont &font = gui.getDefaultFont();

    vmath::Vector4 color_gui_base = styling::colorGuiBase();

    gfx::gui::GUINodeHandle loadgame_bg_node = loadgame_menu_root.addGUINode(gfx::gui::GUITransform({0.50f, 0.50f}, {0.39f, 0.66f}));

    loadgame_bg_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 1*color_gui_base ) ) );
    loadgame_bg_node->hide();

    events::Immediate::add_callback<events::LoadGameEvent>(
        [loadgame_bg_node] (const events::LoadGameEvent &evt) { loadgame_bg_node->show(); }
    );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [loadgame_bg_node] (const events::ToggleMainMenuEvent &evt) { loadgame_bg_node->hide(); }
    );

    gfx::gui::GUINodeHandle title_node = loadgame_bg_node->addGUINode(
        gfx::gui::GUITransform( {0.5f, 0.1f}, {0.5f, 0.10f} ));

    title_node->addElement( gfx::gui::TextElement( "Load game", font) );

    /*for (int i = 0; i<7; i++)
    {
        createButton(loadgame_bg_node, "New load", font, 0.5f, 0.2f+i*0.1f, 0.5f, []()
        {
            std::cout << "Clicked new load!"      << std::endl;
        });
    }*/
}

} // namespace gui

#endif // LOADGAMEMENU_H
