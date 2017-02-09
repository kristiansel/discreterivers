#ifndef NEWGAMEMENU_H
#define NEWGAMEMENU_H

#include "gui.h"
#include "guistyling.h"
#include "createbutton.h"
#include "createtoggle.h"
#include "../createscene.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

#include "../altplanet/altplanet.h"

namespace gui {

struct NewGameMenuState : public gfx::gui::GUIState
{
    using PlanetShape = AltPlanet::PlanetShape;
    enum class PlanetSize {Small, Medium, Large};

    PlanetShape planet_shape;
    PlanetSize  planet_size;

    NewGameMenuState() :
        planet_shape(PlanetShape::Sphere),
        planet_size(PlanetSize::Medium)
    { /* default constructor */ }
};

inline void createNewGameMenu(GUI &gui, gfx::gui::GUINode &new_game_menu_root)
{
    const gfx::gui::GUIFont &font = gui.getDefaultFont();

    vmath::Vector4 color_gui_base = styling::colorGuiBase();

    gfx::gui::GUINodeHandle newgame_bg_node = new_game_menu_root.addGUINode(gfx::gui::GUITransform({0.50f, 0.50f}, {0.80f, 0.80f}));

    newgame_bg_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 1*color_gui_base ) ) );
    newgame_bg_node->hide();
    newgame_bg_node->setState(NewGameMenuState());

    events::Immediate::add_callback<events::NewGameEvent>(
        [newgame_bg_node] (const events::NewGameEvent &evt) { newgame_bg_node->show(); }
    );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [newgame_bg_node] (const events::ToggleMainMenuEvent &evt) { newgame_bg_node->hide(); }
    );

    gfx::gui::GUINodeHandle title_node = newgame_bg_node->addGUINode(
        gfx::gui::GUITransform( {gfx::gui::HorzPos(0.0f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                                 gfx::gui::VertPos(0.0f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Top)}, {0.15f, 0.10f} ));

    title_node->addElement( gfx::gui::TextElement( "New game -> World Generation", font));

    gfx::gui::GUINodeHandle world_scene_node = newgame_bg_node->addGUINode(
        gfx::gui::GUITransform( {gfx::gui::HorzPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Right),
                                 gfx::gui::VertPos(0.5f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle)}, {0.15f, 0.10f} ));

    //world_scene_node->addElement( gfx::gui::TextElement( "World goes here...", font));

    gfx::Camera camera(300, 300);
    camera.mTransform.position = vmath::Vector3(0.0, 1.0, 10.0f);

    gfx::gui::GUIElementHandle scene_element = world_scene_node->addElement( gfx::gui::SceneElement(camera));

    events::Immediate::add_callback<events::GenerateWorldEvent>(
        [scene_element] (const events::GenerateWorldEvent &evt) {
            SceneData scene_data = createPlanetData();
            gfx::SceneNode &scene_node = scene_element->get<gfx::gui::SceneElement>().getSceneRoot();
            createScene(scene_node, scene_data);
        }
    );

    // Toggle world shape
    createToggle(newgame_bg_node, "Sphere", font,
                 gfx::gui::HorzPos(0.06f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.15f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle),
                 0.15f,
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     state.planet_shape = NewGameMenuState::PlanetShape::Sphere;
                     newgame_bg_node->setState(state);
                 },
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     return state.planet_shape == NewGameMenuState::PlanetShape::Sphere;
                 });

    createToggle(newgame_bg_node, "Disk", font,
                 gfx::gui::HorzPos(0.06f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.25f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle),
                 0.15f,
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     state.planet_shape = NewGameMenuState::PlanetShape::Disk;
                     newgame_bg_node->setState(state);
                 },
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     return state.planet_shape == NewGameMenuState::PlanetShape::Disk;
                 });

    createToggle(newgame_bg_node, "Torus", font,
                 gfx::gui::HorzPos(0.06f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.35f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle),
                 0.15f,
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     state.planet_shape = NewGameMenuState::PlanetShape::Torus;
                     newgame_bg_node->setState(state);
                 },
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     return state.planet_shape == NewGameMenuState::PlanetShape::Torus;
                 });


    // Toggle world size
    createToggle(newgame_bg_node, "Small", font,
                 gfx::gui::HorzPos(0.26f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.15f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle),
                 0.15f,
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     state.planet_size = NewGameMenuState::PlanetSize::Small;
                     newgame_bg_node->setState(state);
                 },
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     return state.planet_size == NewGameMenuState::PlanetSize::Small;
                 });

    createToggle(newgame_bg_node, "Medium", font,
                 gfx::gui::HorzPos(0.26f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.25f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle),
                 0.15f,
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     state.planet_size = NewGameMenuState::PlanetSize::Medium;
                     newgame_bg_node->setState(state);
                 },
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     return state.planet_size == NewGameMenuState::PlanetSize::Medium;
                 });

    createToggle(newgame_bg_node, "Large", font,
                 gfx::gui::HorzPos(0.26f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.35f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle),
                 0.15f,
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     state.planet_size = NewGameMenuState::PlanetSize::Large;
                     newgame_bg_node->setState(state);
                 },
                 [newgame_bg_node] ()
                 {
                     NewGameMenuState state = *(newgame_bg_node->getState<NewGameMenuState>());
                     return state.planet_size == NewGameMenuState::PlanetSize::Large;
                 });


    createButton(newgame_bg_node, "Generate", font,
                 gfx::gui::HorzPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Right),
                 gfx::gui::VertPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Bottom),
                 0.2f,
                 [](){ events::Immediate::broadcast(events::GenerateWorldEvent()); });

    createButton(newgame_bg_node, "Back", font,
                 gfx::gui::HorzPos(0.04f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Left),
                 gfx::gui::VertPos(0.96f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Bottom),
                 0.2f,
                 [](){ events::Immediate::broadcast(events::ToggleMainMenuEvent()); });

    // update the state
    newgame_bg_node->setState(NewGameMenuState());
}

} // namespace gui

#endif // NEWGAMEMENU_H
