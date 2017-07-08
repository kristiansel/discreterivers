#ifndef DEBUGPANE_H
#define DEBUGPANE_H

#include "submenus.h"

#include "guistyling.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

namespace gui {

using namespace gfx::gui;

void createDebugPane(GUI &gui, GUINode &debug_pane_root)
{
    const GUIFont &font = gui.getDefaultFont();

    GUINodeHandle sim_status_node = debug_pane_root.addGUINode(
        GUITransform( {HorzPos(200.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                       VertPos(220.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},
                      {SizeSpec(100.0f,  Units::Absolute),
                       SizeSpec(15.0f,  Units::Absolute)}));

    GUIElementHandle sim_status_text_element = sim_status_node->addElement(
                TextElement("Simulation status: ", font));

    events::Immediate::add_callback<events::SimStatusUpdateEvent>(
    [sim_status_text_element, &font] (const events::SimStatusUpdateEvent &evt) {
        std::string sim_status_text = std::string("Simulation status: ") +
                                      std::string(evt.sim_paused ? "Paused" : "Running");
        TextElement &text_element = sim_status_text_element->get<TextElement>();
        text_element.updateText(sim_status_text.c_str(), font, sim_status_text.size());
    });

}

}

#endif // PROFILINGPANE_H
