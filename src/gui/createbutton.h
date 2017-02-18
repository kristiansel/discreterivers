#ifndef CREATEBUTTON_H
#define CREATEBUTTON_H

#include "gui.h"
#include "guistyling.h"
#include "textlabel.h"

namespace gui {

inline gfx::gui::GUINodeHandle createButton(gfx::gui::GUINodeHandle &parent,
                    std::string &&text, const gfx::gui::GUIFont &font,
                    gfx::gui::HorzPos x, gfx::gui::VertPos y, float abs_width,
                    std::function<void(void)> &&onclick)
{
    auto btn_node = parent->addGUINode( gfx::gui::GUITransform( {x, y}, {gfx::gui::SizeSpec(abs_width, gfx::gui::Units::Absolute),
                                                                         gfx::gui::SizeSpec(30.0f, gfx::gui::Units::Absolute)} ));

    auto bg_element = btn_node->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiElement() ) );
    //btn_node->addElement( gfx::gui::TextElement(text, font) );
    textLabel(btn_node, text, font);

    btn_node->setGUIEventHandler([bg_element, onclick](const gfx::gui::GUIEvent &event){
        switch (event.get_type())
        {
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseButtonDownEvent>::value):
            {
                onclick();
            }
            break;
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseEnterEvent>::value):
            {
                bg_element->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiHighlight() );
            }
            break;
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseLeaveEvent>::value):
            {
                bg_element->get<gfx::gui::BackgroundElement>().setColor( gui::styling::colorGuiElement() );
            }
        }
    });

    return btn_node;
}

}

#endif // CREATEBUTTON_H
