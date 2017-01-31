#ifndef CREATEBUTTON_H
#define CREATEBUTTON_H

#include "gui.h"
#include "guistyling.h"

namespace gui {

gfx::gui::GUINodeHandle createButton(gfx::gui::GUINodeHandle &parent,
                    std::string &&text, const gfx::gui::GUIFont &font,
                    gfx::gui::HorzPos x, gfx::gui::VertPos y, gfx::gui::SizeSpec width,
                    std::function<void(void)> &&callback)
{
    vmath::Vector4 color_gui_base = styling::colorGuiBase();

    auto btn_node = parent->addGUINode( gfx::gui::GUITransform( {x, y}, {width, 0.05f} ));
    auto bg_element = btn_node->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiElement() ) );
    btn_node->addElement( gfx::gui::TextElement(text, font) );
    btn_node->mouseClick.addCallback( std::move(callback) );
    btn_node->mouseEnter.addCallback( [bg_element, color_gui_base]()
        {
            bg_element->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiHighlight() );
        }
    );
    btn_node->mouseLeave.addCallback( [bg_element, color_gui_base]()
        {
            bg_element->get<gfx::gui::BackgroundElement>().setColor( gui::styling::colorGuiElement() );
        }
    );
    return btn_node;
}

}

#endif // CREATEBUTTON_H
