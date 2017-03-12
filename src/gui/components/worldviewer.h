#ifndef WORLDVIEWER_H
#define WORLDVIEWER_H

#include "../gui.h"
#include "../guistyling.h"
#include "textlabel.h"
#include "../../createscene.h"
#include "../../common/mathext.h"
#include "../../common/threads/threadpool.h"
#include "../../events/immediateevents.h"
#include "../../mechanics/rotatorcontroller.h"
#include "../../system/async.h"

namespace gui {

struct WorldViewerState : public gfx::gui::GUIStateBase
{
    mech::RotatorController world_controller;
};


inline gfx::gui::GUINodeHandle createWorldViewer( gfx::gui::GUINodeHandle &parent,
                                                  const gfx::gui::GUIFont &font,
                                                  gfx::gui::GUITransform::Position &&pos,
                                                  gfx::gui::GUITransform::Size &&size)
{
    //TODO: Register a callback here such that:
    //  When this gui node is mounted/visible
    //      Then it subscribes to updates in the macro-world
    //      With loading etc while updating
    //  When it becomes invisible etc
    //      Stop subscribing...

    gfx::gui::GUINodeHandle world_scene_node = parent->addGUINode(
        gfx::gui::GUITransform( std::move(pos), std::move(size) ));

    world_scene_node->addElement( gfx::gui::BackgroundElement( styling::colorGuiElement() ) );

    gfx::gui::GUIStateHandle<WorldViewerState> state_handle = world_scene_node->setState(WorldViewerState());

    gfx::gui::GUINodeHandle loading_msg_node = world_scene_node->addGUINode(
            gfx::gui::GUITransform( {gfx::gui::HorzPos(0.5f, gfx::gui::Units::Relative, gfx::gui::HorzAnchor::Middle),
                                     gfx::gui::VertPos(0.5f, gfx::gui::Units::Relative, gfx::gui::VertAnchor::Middle)}, {0.15f, 0.10f} ));
    loading_msg_node->hide();
    //loading_msg_node->addElement( gfx::gui::TextElement( "Generating world...", font ) );
    textLabel(loading_msg_node, "Generating world...", font);

    gfx::Camera camera(gfx::PerspectiveProjection(1.0f, DR_M_PI_4, 0.1f, 100.0f));
    camera.mTransform.position = vmath::Vector3(0.0, 0.0, 10.0f);
    gfx::gui::GUIElementHandle scene_element = world_scene_node->addElement( gfx::gui::SceneElement(camera));
    gfx::SceneNode &scene_root = scene_element->get<gfx::gui::SceneElement>().getSceneRoot();

    gfx::SceneNodeHandle world_node = scene_root.addSceneNode();

    gfx::SceneNodeHandle light_node = scene_root.addSceneNode();
    light_node->addLight(vmath::Vector4(10.0f, 10.0f, 10.0f, 0.0f), vmath::Vector4(0.85f, 0.85f, 0.85f, 1.0f));

    world_scene_node->setGUIEventHandler([state_handle, scene_element](const gfx::gui::GUIEvent &event) {
        switch (event.get_type())
        {
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseDragEvent>::value):
            {
                const gfx::gui::MouseDragEvent &drag_event = event.get_const<gfx::gui::MouseDragEvent>();
                gfx::gui::GUIStateWriter<WorldViewerState> sw_no_update = state_handle.getStateWriterNoUpdate();
                sw_no_update->world_controller.sendTurnSignals({drag_event.x_rel, drag_event.y_rel});
            }
            break;
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseWheelScrollEvent>::value):
            {
                const gfx::gui::MouseWheelScrollEvent &mwscroll_event = event.get_const<gfx::gui::MouseWheelScrollEvent>();
                std::cout << "scrolled world viewer by " << mwscroll_event.y_rel << std::endl;
                gfx::gui::GUIStateWriter<WorldViewerState> sw_no_update = state_handle.getStateWriterNoUpdate();
                sw_no_update->world_controller.sendScrollSignal(mwscroll_event.y_rel);
            }
            break;
        case (gfx::gui::GUIEvent::is_a<gfx::gui::ResizedEvent>::value):
            {
                const gfx::gui::ResizedEvent &resized_event = event.get_const<gfx::gui::ResizedEvent>();
                gfx::Camera &cam = scene_element->get<gfx::gui::SceneElement>().getCamera();
                cam.updateAspect(resized_event.w_abs/resized_event.h_abs);
                //std::cout << "RESIZED CAM" << std::endl;
            }
            break;
        }
    });

    events::Immediate::add_callback<events::GenerateWorldEvent>(
        [world_node, loading_msg_node] (const events::GenerateWorldEvent &evt) {
            world_node->clearAll();
            loading_msg_node->show(); // <--- Show loading message
    });

    events::Immediate::add_callback<events::FinishGenerateWorldEvent>(
        [world_node, loading_msg_node, state_handle] (const events::FinishGenerateWorldEvent &evt) {
            Ptr::ReadPtr<MacroState> scene_data = evt.scene_data;

            // add stuff to scene
            createScene(world_node, scene_data);

            // update gui
            gfx::gui::GUIStateWriter<WorldViewerState> sw = state_handle.getStateWriter();
            loading_msg_node->hide(); // <--- Hide loading message
            sw->world_controller.setControlledNode(Ptr::WritePtr<gfx::SceneNode>(&(*world_node)));
    });

    return world_scene_node;
}

}

#endif // WORLDVIEWER_H
