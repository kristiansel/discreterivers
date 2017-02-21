#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <functional>

#include "../gui.h"
#include "../guistyling.h"

namespace gui {

/*struct TextInputState : public gfx::gui::GUIStateBase
{
    bool focus;
    std::string text_content;

    TextInputState(const std::string &start_text) : text_content(start_text), focus(false) {}
};*/


inline gfx::gui::GUINodeHandle textInput(gfx::gui::GUINodeHandle &parent,
                    const gfx::gui::GUIFont &font,
                    const gfx::gui::GUITransform transf,
                    const std::function<void(const std::string&)> &write_text,
                    const std::function<std::string(void)> &read_text,
                    const std::string &starting_text,

                    unsigned int n_chars_max,
                    const std::function<bool(int32_t)> &key_filter = [](int32_t) { return true; })
{
    gfx::gui::GUINodeHandle text_node_hdl = parent->addGUINode(transf);

    gfx::gui::GUIElementHandle bg_element_hdl = text_node_hdl->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiElement() ) );
    gfx::gui::GUIElementHandle text_element_hdl = text_node_hdl->addElement( gfx::gui::TextElement( starting_text, font) );


    text_node_hdl->setGUIEventHandler(
        [text_element_hdl, bg_element_hdl, /*state_handle,*/read_text, write_text, &font, key_filter, n_chars_max]
        (const gfx::gui::GUIEvent &event){
            switch (event.get_type())
            {
            case (gfx::gui::GUIEvent::is_a<gfx::gui::KeyPressEvent>::value):
                {
                    const gfx::gui::KeyPressEvent &keypress_event = event.get_const<gfx::gui::KeyPressEvent>();
                    int32_t key_code = keypress_event.key_code;

                    if (key_code >= 32 && key_code <=122 && key_filter(key_code))
                    {
                        std::string text = read_text();
                        if (text.size() < n_chars_max)
                        {
                            text += ((char)(keypress_event.key_code));
                        }
                        try { write_text(text); } catch (...) {}
                    }
                    else if (key_code == SDLK_BACKSPACE)
                    {
                        std::string text = read_text();
                        if (text.size() > 0) text.pop_back();
                        try { write_text(text); } catch (...) {}
                    }
                }
                break;

            case (gfx::gui::GUIEvent::is_a<gfx::gui::FocusGainedEvent>::value):
                {
                    bg_element_hdl->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiHighlight() );
                }
                break;
            case (gfx::gui::GUIEvent::is_a<gfx::gui::FocusLostEvent>::value):
                {
                    bg_element_hdl->get<gfx::gui::BackgroundElement>().setColor(gui::styling::colorGuiElement() );
                }
                break;
            }
        }
    );

    text_node_hdl->addStateUpdateCallback([read_text, text_element_hdl, &font](){
        std::string text = read_text();
        gfx::gui::TextElement &text_element = text_element_hdl->get<gfx::gui::TextElement>();
        text_element.updateText(text.c_str(), font, text.size());
    });


    return text_node_hdl;
}

} // namespace gui

#endif // TEXTINPUT_H
