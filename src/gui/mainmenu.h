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
    const gfx::gui::GUIFontRenderer &font_renderer = gui.getFontRenderer();

    gfx::Texture main_bg_tex = gfx::Texture("res/textures/tilling.jpg");
    gfx::Texture font_atlas_tex = font_renderer.getTextureAtlas();

    gfx::gui::GUINodeHandle main_bg_node = main_menu_root.addGUINode(gfx::gui::GUITransform({0.50f, 0.50f}, {0.39f, 0.66f}));

    main_bg_node->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiBase() ) );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [main_bg_node] (const events::ToggleMainMenuEvent &evt) { main_bg_node->toggleShow(); }
    );

    events::Immediate::add_callback<events::NewGameEvent>(
        [main_bg_node] (const events::NewGameEvent &evt) { main_bg_node->hide(); }
    );

    gfx::gui::GUINodeHandle main_img_node = main_bg_node->addGUINode( gfx::gui::GUITransform({0.50f, 0.25f}, {0.80f, 0.33f}) );
    main_img_node->addElement( gfx::gui::ImageElement(main_bg_tex) );

    auto new_btn_node       = createButton(main_bg_node, font_renderer, font_atlas_tex,
                                           "New",     0.5f, 0.500f, []()
    {
        std::cout << "Clicked new game!"      << std::endl;
        events::Immediate::broadcast(events::NewGameEvent());
    });
    auto save_btn_node       = createButton(main_bg_node, font_renderer, font_atlas_tex,
                                            "Save",     0.5f, 0.580f, []()
    {
        std::cout << "Clicked save game!"      << std::endl;
        events::Immediate::broadcast(events::SaveGameEvent());
    });
    auto load_btn_node      = createButton(main_bg_node, font_renderer, font_atlas_tex,
                                           "Load",    0.5f, 0.660f, []()
    {
        std::cout << "Clicked load game!"     << std::endl;
        events::Immediate::broadcast(events::LoadGameEvent());
    });
    auto options_btn_node   = createButton(main_bg_node, font_renderer, font_atlas_tex,
                                           "Options", 0.5f, 0.740f, []()
    {
        std::cout << "Clicked options game!"  << std::endl;
        events::Immediate::broadcast(events::OptionsEvent());
    });
    auto exit_btn_node      = createButton(main_bg_node, font_renderer, font_atlas_tex,
                                           "Exit",    0.5f, 0.820f, []()
    {
        events::Queued::emitEvent(events::Queued::QuitEvent());
        std::cout << "Clicked exit game!"     << std::endl;
    });
}

} // namespace gui

#endif // MAINMENU_H
