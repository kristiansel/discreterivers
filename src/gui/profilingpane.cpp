#ifndef PROFILINGPANE_H
#define PROFILINGPANE_H

#include "submenus.h"

#include "guistyling.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

using namespace gfx::gui;

void createProfilingPane(GUI &gui, GUINode &profiling_pane_root)
{
    const GUIFont &font = gui.getDefaultFont();

    GUINodeHandle fps_counter_node = profiling_pane_root.addGUINode(
        GUITransform( {HorzPos(30.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                       VertPos(0.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},
                      {SizeSpec(100.0f,  Units::Absolute),
                       SizeSpec(15.0f,  Units::Absolute)}));

    GUIElementHandle fps_text_element = fps_counter_node->addElement(
                TextElement("FPS:", font) );

    events::Immediate::add_callback<events::FPSUpdateEvent>(
    [fps_text_element, &font] (const events::FPSUpdateEvent &evt) {
        float fps_filtered_val = evt.fps;
        std::string fps_text = std::to_string((int)(fps_filtered_val))+std::string(" FPS");
        //GUITextVertices fps_text_verts = font.render(fps_text);
        //fps_text_element->get<TextElement>().setTextVertices(fps_text_verts);
        fps_text_element->get<TextElement>().updateText(fps_text.c_str(), font, fps_text.size());
    });

    GUINodeHandle frame_time_node = profiling_pane_root.addGUINode(
        GUITransform( {HorzPos(30.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                       VertPos(30.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},
                      {SizeSpec(100.0f,  Units::Absolute),
                       SizeSpec(15.0f,  Units::Absolute)}));

    GUIElementHandle frame_time_text_element = frame_time_node->addElement(
                TextElement("Frame time:", font) );

    events::Immediate::add_callback<events::FPSUpdateEvent>(
    [frame_time_text_element, &font] (const events::FPSUpdateEvent &evt) {
        float frame_time = 1000000.0f/(evt.fps); // microsec
        std::string frame_time_text = std::to_string((int)(frame_time))+std::string(" microsec.");
        //GUITextVertices frame_time_text_verts = font.render(frame_time_text);
        //frame_time_text_element->get<TextElement>().setTextVertices(frame_time_text_verts);
        frame_time_text_element->get<TextElement>().updateText(frame_time_text.c_str(), font, frame_time_text.size());
    });
}

}

#endif // PROFILINGPANE_H
