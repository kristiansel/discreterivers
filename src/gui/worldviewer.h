#ifndef WORLDVIEWER_H
#define WORLDVIEWER_H

#include "gui.h"
#include "guistyling.h"
#include "../createscene.h"
#include "../events/immediateevents.h"
#include "../common/threads/threadpool.h"
#include "../system/async.h"
#include "../mechanics/rotatorcontroller.h"

namespace gui {

struct WorldViewerState : public gfx::gui::GUIStateBase
{
    //bool hover;

    bool lmb_down;
    bool rmb_down;

    int32_t prev_mouse_x;
    int32_t prev_mouse_y;

    mech::RotatorController world_controller;

    WorldViewerState() :
        //hover(false),
        lmb_down(false), rmb_down(false),
        prev_mouse_x(0), prev_mouse_y(0) {}

};


gfx::gui::GUINodeHandle createWorldViewer(gfx::gui::GUINodeHandle &parent,
                                          const gfx::gui::GUIFont &font,
                                          gfx::gui::GUITransform::Position &&pos,
                                          gfx::gui::GUITransform::Size &&size)
{
    gfx::gui::GUINodeHandle world_scene_node = parent->addGUINode(
        gfx::gui::GUITransform( std::move(pos), std::move(size) ));

    world_scene_node->addElement( gfx::gui::BackgroundElement( styling::colorGuiElement() ) );

    gfx::gui::GUIStateHandle<WorldViewerState> state_handle = world_scene_node->setState(WorldViewerState());

    gfx::gui::GUINodeHandle loading_msg_node = world_scene_node->addGUINode(
            gfx::gui::GUITransform( {gfx::gui::HorzPos(0.5f, gfx::gui::Units::Relative, gfx::gui::HorzAnchor::Middle),
                                     gfx::gui::VertPos(0.5f, gfx::gui::Units::Relative, gfx::gui::VertAnchor::Middle)}, {0.15f, 0.10f} ));
    loading_msg_node->hide();
    loading_msg_node->addElement( gfx::gui::TextElement( "Generating world...", font ) );

    gfx::Camera camera(300, 300);
    camera.mTransform.position = vmath::Vector3(0.0, 0.0, 10.0f);
    gfx::gui::GUIElementHandle scene_element = world_scene_node->addElement( gfx::gui::SceneElement(camera));

    world_scene_node->setGUIEventHandler([state_handle, scene_element](const gfx::gui::GUIEvent &event) {
        gfx::gui::GUIStateWriter<WorldViewerState> sw = state_handle.getStateWriter();
        switch (event.get_type())
        {
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseDragEvent>::value):
            {
                const gfx::gui::MouseDragEvent &drag_event = event.get_const<gfx::gui::MouseDragEvent>();
                gfx::gui::GUIStateWriter<WorldViewerState> sw_no_update = state_handle.getStateWriterNoUpdate();
                float mouse_angle_x = static_cast<float>(drag_event.x_rel)*0.0062832f; // 2π/1000?
                float mouse_angle_y = static_cast<float>(drag_event.y_rel)*0.0062832f;
                sw_no_update->world_controller.sendTurnSignals({mouse_angle_x, mouse_angle_y});
            }
        }
    });


    events::Immediate::add_callback<events::GenerateWorldEvent>(
        [scene_element, loading_msg_node, state_handle] (const events::GenerateWorldEvent &evt) {
            scene_element->get<gfx::gui::SceneElement>().getSceneRoot().clearAll();
            loading_msg_node->show(); // <--- Show loading message
            sys::Async::addJob(
                // The asynchronous operation
                        [evt]()->SceneData{
                            return createPlanetData(evt.planet_shape, evt.planet_size);
                        },
                // Process the result on return
                        [scene_element, loading_msg_node, state_handle](const SceneData &scene_data)->void{
                            gfx::SceneNode &scene_node = scene_element->get<gfx::gui::SceneElement>().getSceneRoot();
                            createScene(scene_node, scene_data);
                            loading_msg_node->hide(); // <--- Hide loading message
                            gfx::gui::GUIStateWriter<WorldViewerState> sw = state_handle.getStateWriter();
                            sw->world_controller.setControlledNode(&scene_node);
                        });
        }
    );
    return world_scene_node;
}

}

#endif // WORLDVIEWER_H
