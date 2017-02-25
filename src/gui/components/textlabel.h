#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include "../gui.h"
#include "../guistyling.h"

namespace gui {

inline gfx::gui::GUIElementHandle textLabel(gfx::gui::GUINodeHandle &parent,
                    const std::string &text, const gfx::gui::GUIFont &font)
{
    auto render_result = font.render(text);

    float w_abs = render_result.text_size.w_abs;
    float h_abs = render_result.text_size.h_abs;

    std::cout << "textLabel: h_abs = " << h_abs << std::endl;

    gfx::gui::GUINodeHandle text_node_hdl = parent->addGUINode(
        gfx::gui::GUITransform({0.5f, 0.5f},
            {gfx::gui::SizeSpec(w_abs, gfx::gui::Units::Absolute),
             gfx::gui::SizeSpec(h_abs*1.2f, gfx::gui::Units::Absolute)}));

    text_node_hdl->clickPassThru();

    //text_node_hdl->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiBase() ) );
    gfx::gui::GUIElementHandle bg_debug_el = text_node_hdl->addElement( gfx::gui::BackgroundElement( gui::styling::colorGuiDebug() ) );
    gfx::gui::GUIElementHandle text_element_hdl = text_node_hdl->addElement( gfx::gui::TextElement( render_result ) );

    return text_element_hdl;
}

} // namespace gui

#endif // TEXTLABEL_H
