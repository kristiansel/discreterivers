#ifndef CREATETOGGLE_H
#define CREATETOGGLE_H

#include "gui.h"
#include "guistyling.h"

namespace gui {

gfx::gui::GUINodeHandle createToggle(gfx::gui::GUINodeHandle &parent,
                    std::string &&text, const gfx::gui::GUIFont &font,
                    gfx::gui::HorzPos x, gfx::gui::VertPos y, gfx::gui::SizeSpec width,
                    std::function<void(void)> &&toggle_callback,
                    const std::function<bool(void)> &is_toggled)
{
    auto btn_node = parent->addGUINode( gfx::gui::GUITransform( {x, y}, {width, 0.05f} ));
    auto bg_element = btn_node->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiElement() ) );

    btn_node->addElement( gfx::gui::TextElement(text, font) );
    btn_node->stateUpdate.addCallback([is_toggled, bg_element](){
        if (is_toggled()) {
            bg_element->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiHighlight() );
        } else {
            bg_element->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiElement() );
        }
    });
    btn_node->setGUIEventHandler([bg_element, toggle_callback](const gfx::gui::GUIEvent &event){
        switch (event.get_type())
        {
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseButtonDownEvent>::value):
            {
                toggle_callback();
            }
            break;
        }
    });
    return btn_node;
}

}


#endif // CREATETOGGLE_H
