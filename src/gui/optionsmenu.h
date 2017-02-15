#ifndef OPTIONSMENU_H
#define OPTIONSMENU_H

#include "gui.h"
#include "guistyling.h"
#include "createbutton.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

inline void createOptionsMenu(GUI &gui, gfx::gui::GUINode &options_menu_root)
{
    const gfx::gui::GUIFont &font = gui.getDefaultFont();

    vmath::Vector4 color_gui_base = styling::colorGuiBase();

    gfx::gui::GUINodeHandle options_bg_node = options_menu_root.addGUINode(gfx::gui::GUITransform({0.50f, 0.50f}, {0.80f, 0.80f}));

    options_bg_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 1*color_gui_base ) ) );
    options_bg_node->hide();

    events::Immediate::add_callback<events::OptionsEvent>(
        [options_bg_node] (const events::OptionsEvent &evt) { options_bg_node->show(); }
    );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [options_bg_node] (const events::ToggleMainMenuEvent &evt) { options_bg_node->hide(); }
    );

    gfx::gui::GUINodeHandle title_node = options_bg_node->addGUINode(
        gfx::gui::GUITransform( {gfx::gui::HorzPos(0.0f, gfx::gui::Units::Relative, gfx::gui::HorzAnchor::Left),
                                 gfx::gui::VertPos(0.0f, gfx::gui::Units::Relative, gfx::gui::VertAnchor::Top)}, {0.15f, 0.10f} ));

    title_node->addElement( gfx::gui::TextElement( "Options", font));

    createButton(options_bg_node, "Apply", font,
                 gfx::gui::HorzPos(0.96f, gfx::gui::Units::Relative, gfx::gui::HorzAnchor::Right),
                 gfx::gui::VertPos(0.96f, gfx::gui::Units::Relative, gfx::gui::VertAnchor::Bottom),
                 0.3f,
                 [](){});

}

} // namespace gui

#endif // OPTIONSMENU_H
