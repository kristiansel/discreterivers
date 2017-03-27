#ifndef CHOOSEORIGINMENU_H
#define CHOOSEORIGINMENU_H

#include <functional>

#include "submenus.h"

#include "gui.h"
#include "guistyling.h"
#include "components/createbutton.h"
#include "components/mapviewer.h"
#include "../events/immediateevents.h"

namespace gui {

using namespace gfx::gui;

struct ChooseOriginMenuState : public GUIStateBase
{
    //ChooseOriginMenuState();
};

void createChooseOriginMenu(GUI &gui, GUINode &choose_origin_menu_root)
{
    const GUIFont &font = gui.getDefaultFont();
    const GUIFont &heading_font = gui.getHeadingFont();

    GUINodeHandle choose_origin_bg_node = choose_origin_menu_root.addGUINode(
                GUITransform({0.50f, 0.50f}, // position and anchor in middle

                            {SizeSpec(60.0f, Units::Absolute, true),
                             SizeSpec(60.0f, Units::Absolute, true)}));

    choose_origin_bg_node->addElement( BackgroundElement( styling::colorGuiBase() ) );
    choose_origin_bg_node->hide();

    GUIStateHandle<ChooseOriginMenuState> state_handle = choose_origin_bg_node->setState(ChooseOriginMenuState());

    events::Immediate::add_callback<events::NewGameEvent>(
        [choose_origin_bg_node] (const events::NewGameEvent &evt) { choose_origin_bg_node->hide(); }
    );

    events::Immediate::add_callback<events::ChooseOriginEvent>(
        [choose_origin_bg_node] (const events::ChooseOriginEvent &evt) { choose_origin_bg_node->show(); }
    );

    events::Immediate::add_callback<events::CharCreateEvent>(
        [choose_origin_bg_node] (const events::CharCreateEvent &evt) { choose_origin_bg_node->hide(); }
    );

    GUINodeHandle title_node = choose_origin_bg_node->addGUINode(
        GUITransform( {HorzPos(10.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                       VertPos(0.0f, Units::Absolute, VertAnchor::Top)}, {0.15f, 0.10f} ));

    title_node->addElement( TextElement( "New game - Choose Origin", heading_font));

    createMapViewer(choose_origin_bg_node, font,
        {HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
         VertPos(60.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},

        {SizeSpec(420.0f,  Units::Absolute, true),
         SizeSpec(150.0f, Units::Absolute, true)} );


    createButton(choose_origin_bg_node, "Next", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                 VertPos(30.0f, Units::Absolute, VertAnchor::Bottom, VertFrom::Bottom),
                 90.0f,
                 [](){
                     // set the origin...
                     //events::Immediate::broadcast(events::ChooseOrigin(/*people*/))
                     events::Immediate::broadcast(events::CharCreateEvent());
                 }/*,
                 [state_handle]()  // is active
                 {
                     // wat
                 }*/);

    createButton(choose_origin_bg_node, "Back", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(30.0f, Units::Absolute, VertAnchor::Bottom, VertFrom::Bottom),
                 180.0f,
                 [](){ events::Immediate::broadcast(events::NewGameEvent(/* clear_state = */ false)); });

    // update the state
    choose_origin_bg_node->forceStateUpdate();
}

} // namespace gui

#endif // CHOOSEORIGINMENU_H
