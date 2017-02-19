#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <functional>

#include "../gui.h"
#include "../guistyling.h"

namespace gui {

struct TextInputState : public gfx::gui::GUIStateBase
{
    bool focus;
    std::string text_content;

    TextInputState(const std::string &start_text) : text_content(start_text), focus(false) {}
};


inline gfx::gui::GUINodeHandle textInput(gfx::gui::GUINodeHandle &parent,
                    const std::string &starting_text, const gfx::gui::GUIFont &font,
                    const gfx::gui::GUITransform transf,
                    const std::function<bool(int32_t)> &key_filter = [](int32_t) { return true; })
{
    gfx::gui::GUINodeHandle text_node_hdl = parent->addGUINode(transf);
    gfx::gui::GUIStateHandle<TextInputState> state_handle = text_node_hdl->setState(TextInputState(starting_text));

    text_node_hdl->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiElement() ) );
    gfx::gui::GUIElementHandle text_element_hdl = text_node_hdl->addElement( gfx::gui::TextElement( starting_text, font) );

    text_node_hdl->setGUIEventHandler([text_element_hdl, state_handle, &font, key_filter](const gfx::gui::GUIEvent &event){
        switch (event.get_type())
        {
        case (gfx::gui::GUIEvent::is_a<gfx::gui::KeyPressEvent>::value):
            {
                const gfx::gui::KeyPressEvent &keypress_event = event.get_const<gfx::gui::KeyPressEvent>();
                int32_t key_code = keypress_event.key_code;

                if (key_code >= 32 && key_code <=122 && key_filter(key_code))
                {
                    gfx::gui::GUIStateWriter<TextInputState> sw = state_handle.getStateWriter();
                    sw->text_content += ((char)(keypress_event.key_code));
                    std::cout << "text content: " << sw->text_content << std::endl;
                }
                else if (key_code == SDLK_BACKSPACE)
                {
                    gfx::gui::GUIStateWriter<TextInputState> sw = state_handle.getStateWriter();
                    if (sw->text_content.size() > 0) sw->text_content.pop_back();
                    std::cout << "text content: " << sw->text_content << std::endl;
                }
            }
            break;
        }
    });

    text_node_hdl->stateUpdate.addCallback([state_handle, text_element_hdl, &font](){
        gfx::gui::GUIStateReader<TextInputState> sr = state_handle.getStateReader();
        gfx::gui::TextElement &text_element = text_element_hdl->get<gfx::gui::TextElement>();
        text_element.updateText(sr->text_content.c_str(), font, sr->text_content.size());
    });

    return text_node_hdl;
}

} // namespace gui
#endif // TEXTINPUT_H
