#ifndef DEBUGPANE_H
#define DEBUGPANE_H

#include "submenus.h"

#include "guistyling.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

// helper function to reduce decimals
#include <sstream>
std::string ftos(float f, int nd) {
   std::ostringstream ostr;
   int tens = std::stoi("1" + std::string(nd, '0'));
   ostr << std::round(f*tens)/tens;
   return ostr.str();
}

namespace gui {

using namespace gfx::gui;

void createDebugPane(GUI &gui, GUINode &debug_pane_root)
{
    const GUIFont &font = gui.getBodyFont();

    GUINodeHandle sim_status_node = debug_pane_root.addGUINode(
        GUITransform( {HorzPos(200.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                       VertPos(215.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},
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

    GUINodeHandle freecam_status_node = debug_pane_root.addGUINode(
        GUITransform( {HorzPos(200.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                       VertPos(230.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},
                      {SizeSpec(100.0f,  Units::Absolute),
                       SizeSpec(15.0f,  Units::Absolute)}));

    GUIElementHandle freecam_status_text_element = freecam_status_node->addElement(
                TextElement("Camera: ", font));

    events::Immediate::add_callback<events::ToggleFreeCamEvent>(
    [freecam_status_text_element, &font] (const events::ToggleFreeCamEvent& evt) {
        std::string freecam_status_text = std::string("Camera: ") +
                                      std::string(evt.freecam? "Free" : "Player");
        TextElement &text_element = freecam_status_text_element->get<TextElement>();
        text_element.updateText(freecam_status_text.c_str(), font, freecam_status_text.size());
    });

    GUINodeHandle player_pos_node = debug_pane_root.addGUINode(
        GUITransform( {HorzPos(200.0f, Units::Absolute, HorzAnchor::Right, HorzFrom::Right),
                       VertPos(245.0f, Units::Absolute, VertAnchor::Top, VertFrom::Top)},
                      {SizeSpec(100.0f,  Units::Absolute),
                       SizeSpec(15.0f,  Units::Absolute)}));

    GUIElementHandle player_pos_text_element = player_pos_node->addElement(
                TextElement("Player: ", font));

    events::Immediate::add_callback<events::PlayerUpdateEvent>(
    [player_pos_text_element, &font] (const events::PlayerUpdateEvent &evt) {
        std::string player_pos_text = std::string("Player: ") +
                                      ftos(evt.player_pos.getX(), 2)+std::string(", ")+
                                      ftos(evt.player_pos.getY(), 2)+std::string(", ")+
                                      ftos(evt.player_pos.getZ(), 2);
        TextElement &text_element = player_pos_text_element->get<TextElement>();
        text_element.updateText(player_pos_text.c_str(), font, player_pos_text.size());
    });

}

}

#endif // PROFILINGPANE_H
