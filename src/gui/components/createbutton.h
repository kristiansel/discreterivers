#ifndef CREATEBUTTON_H
#define CREATEBUTTON_H

#include "../gui.h"
#include "../guistyling.h"
#include "textlabel.h"

namespace gui {

inline gfx::gui::GUINodeHandle createButton(gfx::gui::GUINodeHandle &parent,
                    std::string &&text, const gfx::gui::GUIFont &font,
                    gfx::gui::HorzPos x, gfx::gui::VertPos y, float abs_width,
                    std::function<void(void)> &&on_click,
                    std::function<bool(void)> &&is_active = [](){ return true; })
{
    auto btn_node = parent->addGUINode( gfx::gui::GUITransform( {x, y}, {gfx::gui::SizeSpec(abs_width, gfx::gui::Units::Absolute),
                                                                         gfx::gui::SizeSpec(30.0f, gfx::gui::Units::Absolute)} ));

    auto bg_element = btn_node->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiElement() ) );

    auto text_el = textLabel(btn_node, text, font);

    btn_node->addStateUpdateCallback([is_active, bg_element, text_el](){
        if (is_active()) {
            bg_element->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiElement() );
            text_el->get<gfx::gui::TextElement>().setColor(gui::styling::colorTextDefault());
        } else {
            bg_element->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiInactive() );
            text_el->get<gfx::gui::TextElement>().setColor(gui::styling::colorTextInactive());
        }
    });

    btn_node->setGUIEventHandler([bg_element, on_click, is_active](const gfx::gui::GUIEvent &event){
        if (is_active())
        {
            switch (event.get_type())
            {
            case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseButtonDownEvent>::value):
                {
                    on_click();
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
        }
    });

    return btn_node;
}

}

#endif // CREATEBUTTON_H
