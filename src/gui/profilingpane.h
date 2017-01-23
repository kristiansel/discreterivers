#ifndef PROFILINGPANE_H
#define PROFILINGPANE_H

#include "gui.h"
#include "guistyling.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

inline void createProfilingPane(GUI &gui, gfx::gui::GUINode &profiling_pane_root)
{
    int width = gui.getWidth();
    int height = gui.getHeight();
    const gfx::gui::GUIFontRenderer &font_renderer = gui.getFontRenderer();

    gfx::Texture font_atlas_tex = font_renderer.getTextureAtlas();

    gfx::gui::GUINodeHandle fps_counter_node = profiling_pane_root.addGUINode(
        gfx::gui::GUITransform( {gfx::gui::HorzPos(1.0f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Right),
                                 gfx::gui::VertPos(1.0f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Bottom)}, {0.15f, 0.05f} ));
    //fps_counter_node->addElement( gfx::gui::BackgroundElement( vmath::Vector4( 2*color_gui_base ) ) );
    gfx::gui::GUIElementHandle fps_text_element = fps_counter_node->addElement( gfx::gui::TextElement(
                               font_renderer.render("FPS:", width, height),
                               font_atlas_tex) );

    events::Immediate::add_callback<events::FPSUpdateEvent>(
    [fps_text_element, &font_renderer, width, height] (const events::FPSUpdateEvent &evt) {
        float fps_filtered_val = evt.fps;
        std::string fps_text = std::to_string((int)(fps_filtered_val))+std::string(" FPS");
        gfx::gui::GUITextVertices fps_text_verts = font_renderer.render(fps_text, width, height);
        fps_text_element->get<gfx::gui::TextElement>().setTextVertices(fps_text_verts);
    });
}

}

#endif // PROFILINGPANE_H
