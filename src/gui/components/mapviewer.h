#ifndef MAPVIEWER_H
#define MAPVIEWER_H


#include "../gui.h"
#include "../guistyling.h"
#include "textlabel.h"
#include "../../createscene.h"
#include "../../events/immediateevents.h"
#include "../../common/threads/threadpool.h"
#include "../../system/async.h"
#include "../../mechanics/mapcontroller.h"

namespace gui {

struct MapViewerState : public gfx::gui::GUIStateBase
{
    mech::MapController map_controller;
};


inline gfx::gui::GUINodeHandle createMapViewer(gfx::gui::GUINodeHandle &parent,
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
    //
    // hook up a create map function on the "world generated event"
    // need to include planet shape in planet data and use the getUV virtual function
    // convert the UV to regular vertices, normalize and fit with an orthogonal camera and use
    // old triangulation to display the sceneobject. Make sure back face culling is on and should
    // good to go. Choose a texture... Need to implement getUV for torus.. test with sphere first..
    // After that is good, create controller for map and keep it in state


    gfx::gui::GUINodeHandle map_scene_node = parent->addGUINode(
        gfx::gui::GUITransform( std::move(pos), std::move(size) ));

    map_scene_node->addElement( gfx::gui::BackgroundElement( styling::colorGuiElement() ) );

    gfx::gui::GUIStateHandle<MapViewerState> state_handle = map_scene_node->setState(MapViewerState());


    //gfx::Camera camera(gfx::PerspectiveProjection(1.0f, M_PI_4, 0.1f, 100.0f));
    gfx::Camera camera(gfx::OrthographicProjection(1.0f, 0.5f, -10.0f, 10.0f));
    camera.mTransform.position = vmath::Vector3(0.5f, 0.5f, 0.0f);
    gfx::gui::GUIElementHandle scene_element = map_scene_node->addElement( gfx::gui::SceneElement(camera));

    map_scene_node->setGUIEventHandler([state_handle, scene_element](const gfx::gui::GUIEvent &event) {
        switch (event.get_type())
        {
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseDragEvent>::value):
            {
                const gfx::gui::MouseDragEvent &drag_event = event.get_const<gfx::gui::MouseDragEvent>();
                gfx::gui::GUIStateWriter<MapViewerState> sw_no_update = state_handle.getStateWriterNoUpdate();
                float mouse_angle_x = static_cast<float>(drag_event.x_rel)*0.0062832f; // 2π/1000?
                float mouse_angle_y = static_cast<float>(drag_event.y_rel)*0.0062832f;
                sw_no_update->map_controller.sendTurnSignals({mouse_angle_x, mouse_angle_y});
            }
            break;
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseWheelScrollEvent>::value):
            {
                const gfx::gui::MouseWheelScrollEvent &mwscroll_event = event.get_const<gfx::gui::MouseWheelScrollEvent>();
                std::cout << "scrolled Map viewer by " << mwscroll_event.y_rel << std::endl;
                gfx::gui::GUIStateWriter<MapViewerState> sw_no_update = state_handle.getStateWriterNoUpdate();
                sw_no_update->map_controller.sendScrollSignal(mwscroll_event.y_rel);
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
        [scene_element/*, loading_msg_node*/] (const events::GenerateWorldEvent &evt) {
            scene_element->get<gfx::gui::SceneElement>().getSceneRoot().clearAll();
            //loading_msg_node->show(); // <--- Show loading message
    });

    events::Immediate::add_callback<events::FinishGenerateWorldEvent>(
        [scene_element, /*loading_msg_node,*/ state_handle] (const events::FinishGenerateWorldEvent &evt) {
            gfx::SceneNode &scene_node = scene_element->get<gfx::gui::SceneElement>().getSceneRoot();
            Ptr::ReadPtr<MacroState> scene_data = evt.scene_data;
            createMap(scene_node, scene_data);
            //loading_msg_node->hide(); // <--- Hide loading message
            gfx::gui::GUIStateWriter<MapViewerState> sw = state_handle.getStateWriter();
            sw->map_controller.setControlledNode(&scene_node);
    });

    return map_scene_node;
}

}


#endif // MAPVIEWER_H
