#ifndef MAINMENU_H
#define MAINMENU_H

#include "gui.h"
#include "guistyling.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

inline void createMainMenu(GUI &gui, gfx::gui::GUINode &main_menu_root)
{
    const gfx::gui::GUIFontRenderer &font_renderer = gui.getFontRenderer();

    vmath::Vector4 color_gui_base = styling::colorGuiBase();
    gfx::Texture main_bg_tex = gfx::Texture("res/textures/tilling.jpg");
    gfx::Texture font_atlas_tex = font_renderer.getTextureAtlas();

    gfx::gui::GUINodeHandle main_bg_node = main_menu_root.addGUINode(gfx::gui::GUITransform({0.50f, 0.50f}, {0.39f, 0.66f}));

    main_bg_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 1*color_gui_base ) ) );

    events::Immediate::add_callback<events::ToggleMainMenuEvent>(
        [main_bg_node] (const events::ToggleMainMenuEvent &evt) { main_bg_node->toggleShow(); }
    );

    events::Immediate::add_callback<events::NewGameEvent>(
        [main_bg_node] (const events::NewGameEvent &evt) { main_bg_node->hide(); }
    );

    gfx::gui::GUINodeHandle main_img_node = main_bg_node->addGUINode( gfx::gui::GUITransform({0.50f, 0.25f}, {0.80f, 0.33f}) );
    main_img_node->addElement( gfx::gui::ImageElement(main_bg_tex) );

    auto createButton = [&](std::string &&text, float x, float y, std::function<void(void)> &&callback)
    {
        auto btn_node = main_bg_node->addGUINode( gfx::gui::GUITransform( {gfx::gui::HorzPos(x, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Middle),
                                                      gfx::gui::VertPos(y)}, {0.5f, 0.05f} ));
        auto bg_element = btn_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 2*color_gui_base ) ) );
        btn_node->addElement( gfx::gui::TextElement(
                             font_renderer.render(std::move(text)),
                             font_atlas_tex) );
        btn_node->mouseClick.addCallback( std::move(callback) );
        btn_node->mouseEnter.addCallback( [bg_element, color_gui_base]()
            {
                bg_element->get<gfx::gui::BackgroundElement>().setColor(vmath::Vector4( 4*color_gui_base ));
            }
        );
        btn_node->mouseLeave.addCallback( [bg_element, color_gui_base]()
            {
                bg_element->get<gfx::gui::BackgroundElement>().setColor(vmath::Vector4( 2*color_gui_base ));
            }
        );
        return btn_node;
    };

    auto new_btn_node       = createButton("New",     0.5f, 0.500f, []()
    {
        std::cout << "Clicked new game!"      << std::endl;
        events::Immediate::broadcast(events::NewGameEvent());
    });
    auto load_btn_node      = createButton("Load",    0.5f, 0.585f, []()
    {
        std::cout << "Clicked load game!"     << std::endl;
        events::Immediate::broadcast(events::LoadGameEvent());
    });
    auto options_btn_node   = createButton("Options", 0.5f, 0.670f, []()
    {
        std::cout << "Clicked options game!"  << std::endl;
        events::Immediate::broadcast(events::OptionsEvent());
    });
    auto exit_btn_node      = createButton("Exit",    0.5f, 0.750f, []()
    {
        events::Queued::emitEvent(events::Queued::QuitEvent());
        std::cout << "Clicked exit game!"     << std::endl;
    });
}


}

#endif // MAINMENU_H
