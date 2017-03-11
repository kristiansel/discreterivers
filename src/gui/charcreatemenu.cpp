#ifndef CHARCREATEMENU_H
#define CHARCREATEMENU_H

#include <functional>

#include "submenus.h"

#include "gui.h"
#include "guistyling.h"
#include "components/createbutton.h"
#include "components/mapviewer.h"
#include "../events/immediateevents.h"
namespace gui {

using namespace gfx::gui;

struct CharCreateMenuState : public GUIStateBase
{
    //CharCreateMenuState();
};

void createCharCreateMenu(GUI &gui, GUINode &char_create_menu_root)
{
    const GUIFont &font = gui.getDefaultFont();
    const GUIFont &heading_font = gui.getHeadingFont();

    GUINodeHandle char_create_bg_node = char_create_menu_root.addGUINode(
                GUITransform({0.50f, 0.50f}, // position and anchor in middle

                            {SizeSpec(60.0f, Units::Absolute, true),
                             SizeSpec(60.0f, Units::Absolute, true)}));

    char_create_bg_node->addElement( BackgroundElement( styling::colorGuiBase() ) );
    char_create_bg_node->hide();

    GUIStateHandle<CharCreateMenuState> state_handle = char_create_bg_node->setState(CharCreateMenuState());

    events::Immediate::add_callback<events::ChooseOriginEvent>(
        [char_create_bg_node] (const events::ChooseOriginEvent &evt) { char_create_bg_node->hide(); }
    );

    events::Immediate::add_callback<events::CharCreateEvent>(
        [char_create_bg_node] (const events::CharCreateEvent &evt) { char_create_bg_node->show(); }
    );

    events::Immediate::add_callback<events::StartGameEvent>(
        [char_create_bg_node] (const events::StartGameEvent &evt) { char_create_bg_node->hide(); }
    );


    GUINodeHandle title_node = char_create_bg_node->addGUINode(
        GUITransform( {HorzPos(10.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                       VertPos(0.0f, Units::Absolute, VertAnchor::Top)}, {0.15f, 0.10f} ));

    title_node->addElement( TextElement( "New game - Customize Character", heading_font));

    createButton(char_create_bg_node, "Next", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                 VertPos(30.0f, Units::Absolute, VertAnchor::Bottom, VertFrom::Bottom),
                 90.0f,
                 [](){ events::Immediate::broadcast(events::StartGameEvent()); }/*,
                 [state_handle]()  // is active
                 {
                     // wat
                 }*/);

    createButton(char_create_bg_node, "Back", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(30.0f, Units::Absolute, VertAnchor::Bottom, VertFrom::Bottom),
                 180.0f,
                 [](){ events::Immediate::broadcast(events::ChooseOriginEvent()); });

    // update the state
    char_create_bg_node->forceStateUpdate();
}

} // namespace gui

#endif // CHARCREATEMENU_H
