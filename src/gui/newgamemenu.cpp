#ifndef NEWGAMEMENU_H
#define NEWGAMEMENU_H

#include <functional>

#include "submenus.h"

#include "gui.h"
#include "guistyling.h"
#include "createbutton.h"
#include "createtoggle.h"
#include "worldviewer.h"
#include "../createscene.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"
#include "../common/threads/threadpool.h"
#include "../altplanet/altplanet.h"
#include "../system/async.h"

namespace gui {

struct NewGameMenuState : public gfx::gui::GUIStateBase
{
    using PlanetShape = events::GenerateWorldEvent::PlanetShape;
    using PlanetSize  = events::GenerateWorldEvent::PlanetSize;

    PlanetShape planet_shape;
    PlanetSize  planet_size;

    NewGameMenuState() :
        planet_shape(PlanetShape::Sphere),
        planet_size(PlanetSize::Medium)
    { /* default constructor */ }
};

void createNewGameMenu(GUI &gui, gfx::gui::GUINode &new_game_menu_root)
{
    const gfx::gui::GUIFont &font = gui.getDefaultFont();

    gfx::gui::GUINodeHandle newgame_bg_node = new_game_menu_root.addGUINode(
                gfx::gui::GUITransform({0.5f, 0.5f}, {0.95f, 0.95f}));

    newgame_bg_node->addElement( gfx::gui::BackgroundElement( styling::colorGuiBase() ) );
    newgame_bg_node->hide();

    gfx::gui::GUIStateHandle<NewGameMenuState> state_handle = newgame_bg_node->setState(NewGameMenuState());

    events::Immediate::add_callback<events::NewGameEvent>(
        [newgame_bg_node] (const events::NewGameEvent &evt) { newgame_bg_node->show(); }
    );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [newgame_bg_node] (const events::ToggleMainMenuEvent &evt) { newgame_bg_node->hide(); }
    );

    gfx::gui::GUINodeHandle title_node = newgame_bg_node->addGUINode(
        gfx::gui::GUITransform( {gfx::gui::HorzPos(0.0f, gfx::gui::Units::Relative, gfx::gui::HorzAnchor::Left),
                                 gfx::gui::VertPos(0.0f, gfx::gui::Units::Relative, gfx::gui::VertAnchor::Top)}, {0.15f, 0.10f} ));

    title_node->addElement( gfx::gui::TextElement( "New game -> World Generation", font));

    createWorldViewer(newgame_bg_node, font,
        {gfx::gui::HorzPos(30.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Right, gfx::gui::HorzFrom::Right),
         gfx::gui::VertPos(60.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Top, gfx::gui::VertFrom::Top)},

        {gfx::gui::SizeSpec(0.66f, gfx::gui::Units::Relative),
         gfx::gui::SizeSpec(0.66f, gfx::gui::Units::Relative)});

    // Toggle world shape
    createToggle(newgame_bg_node, "Disk", font,
                 gfx::gui::HorzPos(30.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Left, gfx::gui::HorzFrom::Left),
                 gfx::gui::VertPos(60.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Top, gfx::gui::VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     gfx::gui::GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_shape = NewGameMenuState::PlanetShape::Disk;
                 },
                 [state_handle] ()
                 {
                     gfx::gui::GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     return sr->planet_shape == NewGameMenuState::PlanetShape::Disk;
                 });

    createToggle(newgame_bg_node, "Sphere", font,
                 gfx::gui::HorzPos(30.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Left, gfx::gui::HorzFrom::Left),
                 gfx::gui::VertPos(120.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Top, gfx::gui::VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     gfx::gui::GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_shape = NewGameMenuState::PlanetShape::Sphere;
                 },
                 [state_handle] ()
                 {
                      gfx::gui::GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                      return sr->planet_shape == NewGameMenuState::PlanetShape::Sphere;
                 });

    createToggle(newgame_bg_node, "Torus", font,
                 gfx::gui::HorzPos(30.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Left, gfx::gui::HorzFrom::Left),
                 gfx::gui::VertPos(180.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Top, gfx::gui::VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     gfx::gui::GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_shape = NewGameMenuState::PlanetShape::Torus;
                 },
                 [state_handle] ()
                 {
                     gfx::gui::GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     return sr->planet_shape == NewGameMenuState::PlanetShape::Torus;
                 });


    // Toggle world size
    createToggle(newgame_bg_node, "Small", font,
                 gfx::gui::HorzPos(160.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Left, gfx::gui::HorzFrom::Left),
                 gfx::gui::VertPos(60.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Top, gfx::gui::VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     gfx::gui::GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_size = NewGameMenuState::PlanetSize::Small;
                 },
                 [state_handle] ()
                 {
                     gfx::gui::GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     return sr->planet_size == NewGameMenuState::PlanetSize::Small;
                 });

    createToggle(newgame_bg_node, "Medium", font,
                 gfx::gui::HorzPos(160.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Left, gfx::gui::HorzFrom::Left),
                 gfx::gui::VertPos(120.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Top, gfx::gui::VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                     gfx::gui::GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                     sw->planet_size = NewGameMenuState::PlanetSize::Medium;
                 },
                 [state_handle] ()
                 {
                     gfx::gui::GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     return sr->planet_size == NewGameMenuState::PlanetSize::Medium;
                 });

    createToggle(newgame_bg_node, "Large", font,
                 gfx::gui::HorzPos(160.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Left, gfx::gui::HorzFrom::Left),
                 gfx::gui::VertPos(180.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Top, gfx::gui::VertFrom::Top),
                 90.0f,
                 [state_handle] ()
                 {
                    gfx::gui::GUIStateWriter<NewGameMenuState> sw = state_handle.getStateWriter();
                    sw->planet_size = NewGameMenuState::PlanetSize::Large;
                 },
                 [state_handle] ()
                 {
                    gfx::gui::GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                    return sr->planet_size == NewGameMenuState::PlanetSize::Large;
                 });

    createButton(newgame_bg_node, "Generate", font,
                 gfx::gui::HorzPos(30.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Right, gfx::gui::HorzFrom::Right),
                 gfx::gui::VertPos(30.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Bottom, gfx::gui::VertFrom::Bottom),
                 180.0f,
                 [state_handle]()
                 {
                     gfx::gui::GUIStateReader<NewGameMenuState> sr = state_handle.getStateReader();
                     events::Immediate::broadcast(events::GenerateWorldEvent{sr->planet_shape, sr->planet_size});
                 });

    createButton(newgame_bg_node, "Back", font,
                 gfx::gui::HorzPos(30.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Left, gfx::gui::HorzFrom::Left),
                 gfx::gui::VertPos(30.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Bottom, gfx::gui::VertFrom::Bottom),
                 180.0f,
                 [](){ events::Immediate::broadcast(events::ToggleMainMenuEvent()); });

    // update the state
    newgame_bg_node->onStateUpdate();
}

} // namespace gui

#endif // NEWGAMEMENU_H
