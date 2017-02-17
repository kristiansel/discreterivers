#ifndef MAINMENU_H
#define MAINMENU_H

#include "submenus.h"

#include "gui.h"
#include "guistyling.h"
#include "createbutton.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"


namespace gui {

using namespace gfx::gui;

void createMainMenu(GUI &gui, GUINode &main_menu_root)
{
    const GUIFont &font = gui.getDefaultFont();
    const GUIFont &body_font = gui.getBodyFont();

    gfx::Texture main_bg_tex = gfx::Texture("res/textures/tilling.jpg");

    GUINodeHandle main_bg_node = main_menu_root.addGUINode(
        GUITransform( {0.5f, 0.5f},

        {SizeSpec(400.0f, Units::Absolute),
         SizeSpec(600.0f, Units::Absolute)}));

    main_bg_node->addElement( BackgroundElement( gui::styling::colorGuiBase() ) );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [main_bg_node] (const events::ToggleMainMenuEvent &evt) { main_bg_node->toggleShow(); }
    );

    // register menu hide callbacks
    events::Immediate::add_callback<events::NewGameEvent>(
        [main_bg_node] (const events::NewGameEvent &evt) { main_bg_node->hide(); }
    );
    events::Immediate::add_callback<events::SaveGameEvent>(
        [main_bg_node] (const events::SaveGameEvent &evt) { main_bg_node->hide(); }
    );
    events::Immediate::add_callback<events::LoadGameEvent>(
        [main_bg_node] (const events::LoadGameEvent &evt) { main_bg_node->hide(); }
    );
    events::Immediate::add_callback<events::OptionsEvent>(
        [main_bg_node] (const events::OptionsEvent &evt) { main_bg_node->hide(); }
    );

    GUINodeHandle main_img_node = main_bg_node->addGUINode( GUITransform({0.50f, 0.25f}, {0.80f, 0.33f}) );
    main_img_node->addElement( ImageElement(main_bg_tex) );

    auto new_btn_node       = createButton(main_bg_node, "New", font, 0.5f, 0.500f, 180.0f, []()
    {
        std::cout << "Clicked new game!"      << std::endl;
        events::Immediate::broadcast(events::NewGameEvent());
    });
    auto save_btn_node       = createButton(main_bg_node, "Save", font, 0.5f, 0.580f, 180.0f, []()
    {
        std::cout << "Clicked save game!"      << std::endl;
        events::Immediate::broadcast(events::SaveGameEvent());
    });
    auto load_btn_node      = createButton(main_bg_node, "Load", font, 0.5f, 0.660f, 180.0f, []()
    {
        std::cout << "Clicked load game!"     << std::endl;
        events::Immediate::broadcast(events::LoadGameEvent());
    });
    auto options_btn_node   = createButton(main_bg_node, "Options", font, 0.5f, 0.740f, 180.0f, []()
    {
        std::cout << "Clicked options game!"  << std::endl;
        events::Immediate::broadcast(events::OptionsEvent());
    });
    auto exit_btn_node      = createButton(main_bg_node, "Exit", font, 0.5f, 0.820f, 180.0f, []()
    {
        events::Queued::emitEvent(events::Queued::QuitEvent());
        std::cout << "Clicked exit game!"     << std::endl;
    });

    GUINodeHandle text_test_node = main_menu_root.addGUINode(
        GUITransform( {HorzPos(30.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                       VertPos(60.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},

                      {SizeSpec(200.0f, Units::Absolute),
                       SizeSpec(200.0f, Units::Absolute)} ));

        text_test_node->addElement( BackgroundElement( gui::styling::colorGuiBase() ) );


        text_test_node->addElement( TextElement( "This text is to test if it wraps", body_font));
}

} // namespace gui

#endif // MAINMENU_H
