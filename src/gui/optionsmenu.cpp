#ifndef OPTIONSMENU_H
#define OPTIONSMENU_H

#include "submenus.h"

#include "guistyling.h"
#include "components/createbutton.h"
#include "components/createtoggle.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

using namespace gfx::gui;

struct OptionsMenuState : public GUIStateBase
{
    using ScaleFactor = events::UIScaleFactorUpdate::ScaleFactor;

    ScaleFactor scale_factor;

    OptionsMenuState() :
        scale_factor(ScaleFactor::Medium) {}
};

void createOptionsMenu(GUI &gui, GUINode &options_menu_root)
{
    const GUIFont &font = gui.getDefaultFont();
    const GUIFont &heading_font = gui.getHeadingFont();

    vmath::Vector4 color_gui_base = styling::colorGuiBase();

    GUINodeHandle options_bg_node = options_menu_root.addGUINode(
                GUITransform({0.50f, 0.50f}, // position and anchor in middle

                            {SizeSpec(60.0f, Units::Absolute, true),
                             SizeSpec(60.0f, Units::Absolute, true)}));

    options_bg_node->addElement( BackgroundElement( vmath::Vector4( 1*color_gui_base ) ) );
    options_bg_node->hide();

    GUIStateHandle<OptionsMenuState> state_handle = options_bg_node->setState(OptionsMenuState());

    events::Immediate::add_callback<events::OptionsEvent>(
        [options_bg_node] (const events::OptionsEvent &evt) { options_bg_node->show(); }
    );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [options_bg_node] (const events::ToggleMainMenuEvent &evt) { options_bg_node->hide(); }
    );

    GUINodeHandle title_node = options_bg_node->addGUINode(
        GUITransform( {HorzPos(0.0f, Units::Relative, HorzAnchor::Left),
                                 VertPos(0.0f, Units::Relative, VertAnchor::Top)}, {0.15f, 0.10f} ));

    title_node->addElement( TextElement( "Options", heading_font));

    GUINodeHandle ui_scaling_node = options_bg_node->addGUINode(
        GUITransform( { HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                        VertPos(90.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)}, {0.0, 0.0f} ));
    ui_scaling_node->addElement( TextElement( "Option 1...", font));

    // Toggle option 1 size
    createToggle(options_bg_node, "Small", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(120.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     GUIStateWriter<OptionsMenuState> sw = state_handle.getStateWriter();
                     sw->scale_factor = OptionsMenuState::ScaleFactor::Small;
                     events::Immediate::broadcast(events::UIScaleFactorUpdate{OptionsMenuState::ScaleFactor::Small});
                 },
                 [state_handle] ()
                 {
                     GUIStateReader<OptionsMenuState> sr = state_handle.getStateReader();
                     return sr->scale_factor == OptionsMenuState::ScaleFactor::Small;
                 });

    createToggle(options_bg_node, "Medium", font,
                 HorzPos(150.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(120.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     GUIStateWriter<OptionsMenuState> sw = state_handle.getStateWriter();
                     sw->scale_factor = OptionsMenuState::ScaleFactor::Medium;
                     events::Immediate::broadcast(events::UIScaleFactorUpdate{OptionsMenuState::ScaleFactor::Medium});
                 },
                 [state_handle] ()
                 {
                     GUIStateReader<OptionsMenuState> sr = state_handle.getStateReader();
                     return sr->scale_factor == OptionsMenuState::ScaleFactor::Medium;
                 });

    createToggle(options_bg_node, "Large", font,
                 HorzPos(270.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(120.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                    GUIStateWriter<OptionsMenuState> sw = state_handle.getStateWriter();
                    sw->scale_factor = OptionsMenuState::ScaleFactor::Large;
                    events::Immediate::broadcast(events::UIScaleFactorUpdate{OptionsMenuState::ScaleFactor::Large});
                 },
                 [state_handle] ()
                 {
                    GUIStateReader<OptionsMenuState> sr = state_handle.getStateReader();
                    return sr->scale_factor == OptionsMenuState::ScaleFactor::Large;
                 });

    createButton(options_bg_node, "Apply", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                 VertPos(30.0f, Units::Absolute, VertAnchor::Bottom, VertFrom::Bottom),
                 180.0f,
                 [](){});

    createButton(options_bg_node, "Back", font,
                 HorzPos(30.0f, Units::Absolute, HorzAnchor::Left, HorzFrom::Left),
                 VertPos(30.0f, Units::Absolute, VertAnchor::Bottom, VertFrom::Bottom),
                 180.0f,
                 [](){ events::Immediate::broadcast(events::ToggleMainMenuEvent()); });

    options_bg_node->forceStateUpdate();

}

} // namespace gui

#endif // OPTIONSMENU_H
