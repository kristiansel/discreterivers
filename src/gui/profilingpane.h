#ifndef PROFILINGPANE_H
#define PROFILINGPANE_H

#include "gui.h"
#include "guistyling.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

inline void createProfilingPane(GUI &gui, gfx::gui::GUINode &profiling_pane_root)
{
    const gfx::gui::GUIFontRenderer &font_renderer = gui.getFontRenderer();

    gfx::Texture font_atlas_tex = font_renderer.getTextureAtlas();

    gfx::gui::GUINodeHandle fps_counter_node = profiling_pane_root.addGUINode(
        gfx::gui::GUITransform( {gfx::gui::HorzPos(1.0f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Right),
                                 gfx::gui::VertPos(1.0f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Bottom)}, {0.15f, 0.05f} ));

    gfx::gui::GUIElementHandle fps_text_element = fps_counter_node->addElement( gfx::gui::TextElement(
                               font_renderer.render("FPS:"),
                               font_atlas_tex) );

    events::Immediate::add_callback<events::FPSUpdateEvent>(
    [fps_text_element, &font_renderer] (const events::FPSUpdateEvent &evt) {
        float fps_filtered_val = evt.fps;
        std::string fps_text = std::to_string((int)(fps_filtered_val))+std::string(" FPS");
        gfx::gui::GUITextVertices fps_text_verts = font_renderer.render(fps_text);
        fps_text_element->get<gfx::gui::TextElement>().setTextVertices(fps_text_verts);
    });

    gfx::gui::GUINodeHandle frame_time_node = profiling_pane_root.addGUINode(
        gfx::gui::GUITransform( {gfx::gui::HorzPos(1.0f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Right),
                                 gfx::gui::VertPos(1.0f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Bottom)}, {0.15f, 0.10f} ));

    gfx::gui::GUIElementHandle frame_time_text_element = frame_time_node->addElement( gfx::gui::TextElement(
                               font_renderer.render("FPS:"),
                               font_atlas_tex) );

    events::Immediate::add_callback<events::FPSUpdateEvent>(
    [frame_time_text_element, &font_renderer] (const events::FPSUpdateEvent &evt) {
        float frame_time = 1000000.0f/(evt.fps); // microsec
        std::string frame_time_text = std::to_string((int)(frame_time))+std::string(" microsec.");
        gfx::gui::GUITextVertices frame_time_text_verts = font_renderer.render(frame_time_text);
        frame_time_text_element->get<gfx::gui::TextElement>().setTextVertices(frame_time_text_verts);
    });
}

}

#endif // PROFILINGPANE_H
