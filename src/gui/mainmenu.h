#ifndef MAINMENU_H
#define MAINMENU_H

#include "gui.h"
#include "guistyling.h"
#include "createbutton.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"


namespace gui {

inline void createMainMenu(GUI &gui, gfx::gui::GUINode &main_menu_root)
{
    const gfx::gui::GUIFont &font = gui.getDefaultFont();

    gfx::Texture main_bg_tex = gfx::Texture("res/textures/tilling.jpg");
    gfx::Texture font_atlas_tex = font.getTextureAtlas();

    gfx::gui::GUINodeHandle main_bg_node = main_menu_root.addGUINode(gfx::gui::GUITransform({0.50f, 0.50f}, {0.39f, 0.66f}));

    main_bg_node->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiBase() ) );

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

    gfx::gui::GUINodeHandle main_img_node = main_bg_node->addGUINode( gfx::gui::GUITransform({0.50f, 0.25f}, {0.80f, 0.33f}) );
    main_img_node->addElement( gfx::gui::ImageElement(main_bg_tex) );

    auto new_btn_node       = createButton(main_bg_node, "New", font, 0.5f, 0.500f, 0.5f, []()
    {
        std::cout << "Clicked new game!"      << std::endl;
        events::Immediate::broadcast(events::NewGameEvent());
    });
    auto save_btn_node       = createButton(main_bg_node, "Save", font, 0.5f, 0.580f, 0.5f, []()
    {
        std::cout << "Clicked save game!"      << std::endl;
        events::Immediate::broadcast(events::SaveGameEvent());
    });
    auto load_btn_node      = createButton(main_bg_node, "Load", font, 0.5f, 0.660f, 0.5f, []()
    {
        std::cout << "Clicked load game!"     << std::endl;
        events::Immediate::broadcast(events::LoadGameEvent());
    });
    auto options_btn_node   = createButton(main_bg_node, "Options", font, 0.5f, 0.740f, 0.5f, []()
    {
        std::cout << "Clicked options game!"  << std::endl;
        events::Immediate::broadcast(events::OptionsEvent());
    });
    auto exit_btn_node      = createButton(main_bg_node, "Exit", font, 0.5f, 0.820f, 0.5f, []()
    {
        events::Queued::emitEvent(events::Queued::QuitEvent());
        std::cout << "Clicked exit game!"     << std::endl;
    });
}

} // namespace gui

#endif // MAINMENU_H
