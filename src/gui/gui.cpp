#include "gui.h"

namespace gui {

void createGUI(gfx::gui::GUINodeHandle &gui_root_hdl, const gfx::gui::GUIFontRenderer &font_renderer, int width, int height)
{
    vmath::Vector4 color_gui_base = vmath::Vector4{0.06, 0.09, 0.12, 0.6};

    gfx::Texture font_atlas_tex = font_renderer.getTextureAtlas();
    gfx::Texture main_bg_tex = gfx::Texture("res/textures/tilling.jpg");

    gfx::gui::GUINodeHandle main_bg_node = gui_root_hdl->addGUINode( gfx::gui::GUITransform({0.50f, 0.50f}, {0.39f, 0.66f}) );
    main_bg_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 1*color_gui_base ) ) );

    gfx::gui::GUINodeHandle main_img_node = main_bg_node->addGUINode( gfx::gui::GUITransform({0.50f, 0.25f}, {0.80f, 0.33f}) );
    main_img_node->addElement( gfx::gui::ImageElement(main_bg_tex) );

    auto createButton = [&](std::string &&text, float x, float y, std::function<void(void)> &&callback)
    {
        auto btn_node = main_bg_node->addGUINode( gfx::gui::GUITransform( {gfx::gui::HorzPos(x, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Middle),
                                                      gfx::gui::VertPos(y)}, {0.5f, 0.05f} ));
        btn_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 2*color_gui_base ) ) );
        btn_node->addElement( gfx::gui::TextElement(
                             font_renderer.render(std::move(text), width, height),
                             font_atlas_tex) );
        btn_node->mouseClick.addCallback( std::move(callback) );
        return btn_node;
    };

    auto new_btn_node       = createButton("New",     0.5f, 0.500f, [](){ std::cout << "Clicked new game!"      << std::endl; });
    auto load_btn_node      = createButton("Load",    0.5f, 0.585f, [](){ std::cout << "Clicked load game!"     << std::endl; });
    auto options_btn_node   = createButton("Options", 0.5f, 0.670f, [](){ std::cout << "Clicked options game!"  << std::endl; });
    auto exit_btn_node      = createButton("Exit",    0.5f, 0.750f, [](){ std::cout << "Clicked exit game!"     << std::endl; });


    /*auto load_btn_node = main_bg_node->addGUINode( gfx::gui::GUITransform( {gfx::gui::HorzPos(0.5f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Middle),
                                                   gfx::gui::VertPos(0.585f)}, {0.5f, 0.05f} ));
    load_btn_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 2*color_gui_base ) ) );
    load_btn_node->addElement( gfx::gui::TextElement(
                         font_renderer.render("Load", width, height),
                         font_atlas_tex) );

    auto opts_btn_node = main_bg_node->addGUINode( gfx::gui::GUITransform( {gfx::gui::HorzPos(0.5f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Middle),
                                                   gfx::gui::VertPos(0.67f)}, {0.5f, 0.05f} ));
    opts_btn_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 2*color_gui_base ) ) );
    opts_btn_node->addElement( gfx::gui::TextElement(
                         font_renderer.render("Options", width, height),
                         font_atlas_tex) );

    auto exit_btn_node = main_bg_node->addGUINode( gfx::gui::GUITransform( {gfx::gui::HorzPos(0.5f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Middle),
                                                   gfx::gui::VertPos(0.75f)}, {0.5f, 0.05f} ));
    exit_btn_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 2*color_gui_base ) ) );
    exit_btn_node->addElement( gfx::gui::TextElement(
                         font_renderer.render("Exit", width, height),
                         font_atlas_tex) );
*/
    //gfx::gui::GUINodeHandle font_node = opengl_renderer.addGUINode( gfx::gui::GUITransform({0.15f, 0.15f}, {0.25f, 0.25f}) );
    //font_node->addElement( gfx::gui::ImageElement(font_atlas_tex));

    vmath::Vector4 todo_color = vmath::Vector4{0.75, 0.0, 0.0, 1.0};

    gfx::gui::GUINodeHandle todo_node = gui_root_hdl->addGUINode( gfx::gui::GUITransform({0.75f, 0.15f}, {0.15f, 0.15f}) );
    todo_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 2*color_gui_base ) ) );
    todo_node->addElement( gfx::gui::TextElement(
                               font_renderer.render("TODO:", width, height),
                               font_atlas_tex,
                               todo_color) );

    todo_node->addGUINode( gfx::gui::GUITransform({0.5f, 0.25f}, {0.10f, 0.10f} ))
        ->addElement( gfx::gui::TextElement(
                         font_renderer.render("Fix text color", width, height),
                         font_atlas_tex,
                         todo_color) );

    todo_node->addGUINode( gfx::gui::GUITransform({0.5f, 0.50f}, {0.1f, 0.1f} ))
        ->addElement( gfx::gui::TextElement(
                         font_renderer.render("Fix line wrap", width, height),
                         font_atlas_tex,
                         todo_color) );
}

}
