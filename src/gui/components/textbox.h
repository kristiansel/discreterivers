#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "../gui.h"
#include "../guistyling.h"

namespace gui {

inline gfx::gui::GUINodeHandle textBox(gfx::gui::GUINode &parent,
                    const std::string &text, const gfx::gui::GUIFont &font,
                    const gfx::gui::GUITransform transf)
{
    gfx::gui::GUINodeHandle text_node_hdl = parent.addGUINode(transf);

    text_node_hdl->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiBase() ) );
    gfx::gui::GUIElementHandle text_element_hdl = text_node_hdl->addElement( gfx::gui::TextElement( text, font) );

    text_node_hdl->setGUIEventHandler([text_element_hdl, text, &font](const gfx::gui::GUIEvent &event){
        switch (event.get_type())
        {
        case (gfx::gui::GUIEvent::is_a<gfx::gui::ResizedEvent>::value):
            {
                const gfx::gui::ResizedEvent &resized_event = event.get_const<gfx::gui::ResizedEvent>();
                gfx::gui::TextElement &text_element = text_element_hdl->get<gfx::gui::TextElement>();
                text_element.setTextRenderResult(font.render(text, resized_event.w_abs, resized_event.w_abs));
            }
            break;
        }
    });

    return text_node_hdl;
}

} // namespace gui

#endif // TEXTBOX_H
