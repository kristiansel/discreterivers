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
    const gfx::gui::GUIFontRenderer &font_renderer = gui.getFontRenderer();

    vmath::Vector4 color_gui_base = styling::colorGuiBase();
    gfx::Texture main_bg_tex = gfx::Texture("res/textures/tilling.jpg");
    gfx::Texture font_atlas_tex = font_renderer.getTextureAtlas();

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

    gfx::gui::GUIElementHandle title_text_element = title_node->addElement( gfx::gui::TextElement(
                               font_renderer.render("New game -> World Generation"),
                               font_atlas_tex) );

    createButton(newgame_bg_node, font_renderer, font_atlas_tex, "Generate",
                 gfx::gui::HorzPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Right),
                 gfx::gui::VertPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Bottom),
                 [](){});

}

} // namespace gui

#endif // NEWGAMEMENU_H
