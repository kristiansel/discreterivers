#ifndef MAINMENU_H
#define MAINMENU_H

#include "submenus.h"

#include "gui.h"
#include "guistyling.h"
#include "createbutton.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"


namespace gui {

void createMainMenu(GUI &gui, gfx::gui::GUINode &main_menu_root)
{
    const gfx::gui::GUIFont &font = gui.getDefaultFont();

    gfx::Texture main_bg_tex = gfx::Texture("res/textures/tilling.jpg");
    gfx::Texture font_atlas_tex = font.getTextureAtlas();

    gfx::gui::GUINodeHandle main_bg_node = main_menu_root.addGUINode(
        gfx::gui::GUITransform( {0.5f, 0.5f},

        {gfx::gui::SizeSpec(400.0f, gfx::gui::Units::Absolute),
         gfx::gui::SizeSpec(600.0f, gfx::gui::Units::Absolute)}));

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

    // abs test node 0
//    gfx::gui::GUINodeHandle abs_test_node = main_menu_root.addGUINode(
//        gfx::gui::GUITransform( {gfx::gui::HorzPos(100.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Left),
//                                 gfx::gui::VertPos(100.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Top)},

//                                {gfx::gui::SizeSpec(400.0f, gfx::gui::Units::Absolute),
//                                 gfx::gui::SizeSpec(300.0f, gfx::gui::Units::Absolute)}));



//    abs_test_node->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiBase() ) );

//    /*gfx::gui::GUINodeHandle abs_test_child = abs_test_node->addGUINode(
//                gfx::gui::GUITransform( {gfx::gui::HorzPos(100.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Left),
//                                         gfx::gui::VertPos(100.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Top)},

//                                        {gfx::gui::SizeSpec(100.0f, gfx::gui::Units::Absolute),
//                                         gfx::gui::SizeSpec(200.0f, gfx::gui::Units::Absolute)}));

//    abs_test_child->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiHighlight() ) );*/

//    // abs test node 1
//    gfx::gui::GUINodeHandle abs_test_node1 = abs_test_node->addGUINode(
//        gfx::gui::GUITransform( {gfx::gui::HorzPos(50.0f, gfx::gui::Units::Absolute, gfx::gui::HorzAnchor::Right, gfx::gui::HorzFrom::Right),
//                                 gfx::gui::VertPos(50.0f, gfx::gui::Units::Absolute, gfx::gui::VertAnchor::Bottom, gfx::gui::VertFrom::Bottom)},

//                                {gfx::gui::SizeSpec(200.0f, gfx::gui::Units::Absolute),
//                                 gfx::gui::SizeSpec(100.0f, gfx::gui::Units::Absolute)}));

//    auto abs_test_el = abs_test_node1->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiElement() ) );

//    abs_test_node1->setGUIEventHandler([abs_test_el](const gfx::gui::GUIEvent &event){
//        switch (event.get_type())
//        {
//        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseButtonDownEvent>::value):
//            {
//                std::cout << "clicked me!" << std::endl;
//            }
//            break;
//        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseEnterEvent>::value):
//            {
//                abs_test_el->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiHighlight() );
//            }
//            break;
//        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseLeaveEvent>::value):
//            {
//                abs_test_el->get<gfx::gui::BackgroundElement>().setColor( gui::styling::colorGuiElement() );
//            }
//        }
//    });
}

} // namespace gui

#endif // MAINMENU_H
