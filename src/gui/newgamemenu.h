#ifndef NEWGAMEMENU_H
#define NEWGAMEMENU_H

#include "gui.h"
#include "guistyling.h"
#include "createbutton.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

inline void createNewGameMenu(GUI &gui, gfx::gui::GUINode &new_game_menu_root)
{
    const gfx::gui::GUIFont &font = gui.getDefaultFont();

    vmath::Vector4 color_gui_base = styling::colorGuiBase();

    gfx::gui::GUINodeHandle newgame_bg_node = new_game_menu_root.addGUINode(gfx::gui::GUITransform({0.50f, 0.50f}, {0.80f, 0.80f}));

    newgame_bg_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 1*color_gui_base ) ) );
    newgame_bg_node->hide();

    events::Immediate::add_callback<events::NewGameEvent>(
        [newgame_bg_node] (const events::NewGameEvent &evt) { newgame_bg_node->show(); }
    );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [newgame_bg_node] (const events::ToggleMainMenuEvent &evt) { newgame_bg_node->hide(); }
    );

    gfx::gui::GUINodeHandle title_node = newgame_bg_node->addGUINode(
        gfx::gui::GUITransform( {gfx::gui::HorzPos(0.0f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                                 gfx::gui::VertPos(0.0f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Top)}, {0.15f, 0.10f} ));

    title_node->addElement( gfx::gui::MonospaceTextElement( "New game -> World Generation", font, 32));

    createButton(newgame_bg_node, "Sphere", font,
                 gfx::gui::HorzPos(0.06f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.15f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle),
                 0.15f,
                 [](){});

    createButton(newgame_bg_node, "Disk", font,
                 gfx::gui::HorzPos(0.06f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.25f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle),
                 0.15f,
                 [](){});

    createButton(newgame_bg_node, "Torus", font,
                 gfx::gui::HorzPos(0.06f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.35f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle),
                 0.15f,
                 [](){});


    createButton(newgame_bg_node, "Generate", font,
                 gfx::gui::HorzPos(0.75f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Right),
                 gfx::gui::VertPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Bottom),
                 0.2f,
                 [](){});

    createButton(newgame_bg_node, "Next", font,
                 gfx::gui::HorzPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Right),
                 gfx::gui::VertPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Bottom),
                 0.2f,
                 [](){});

    createButton(newgame_bg_node, "Back", font,
                 gfx::gui::HorzPos(0.04f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Bottom),
                 0.2f,
                 [](){ events::Immediate::broadcast(events::ToggleMainMenuEvent()); });
}

} // namespace gui

#endif // NEWGAMEMENU_H
