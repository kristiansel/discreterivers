#ifndef WORLDVIEWER_H
#define WORLDVIEWER_H


#include "gui.h"
#include "guistyling.h"

namespace gui {

/*
gfx::gui::GUINodeHandle createWorldViewer(gfx::gui::GUINodeHandle &parent,
                    std::string &&text, const gfx::gui::GUIFont &font,
                    gfx::gui::HorzPos x, gfx::gui::VertPos y, gfx::gui::SizeSpec width,
                    std::function<void(void)> &&toggle_callback,
                    const std::function<bool(void)> &is_toggled)
{
    vmath::Vector4 color_gui_base = styling::colorGuiBase();

    auto btn_node = parent->addGUINode( gfx::gui::GUITransform( {x, y}, {width, 0.05f} ));
    auto bg_element = btn_node->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiElement() ) );
    btn_node->addElement( gfx::gui::TextElement(text, font) );
    btn_node->mouseClick.addCallback( std::move(toggle_callback) );
    btn_node->stateUpdate.addCallback([is_toggled, bg_element](){
        if (is_toggled()) {
            bg_element->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiHighlight() );
        } else {
            bg_element->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiElement() );
        }
    });
    return btn_node;
}*/

}

#endif // WORLDVIEWER_H
