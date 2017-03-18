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

// to remove
#include "../../mechanics/cameracontroller.h"

namespace gui {

struct MapViewerState : public gfx::gui::GUIStateBase
{
    mech::MapController map_controller;
    //mech::CameraController map_controller;
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

    gfx::Camera camera(gfx::OrthographicProjection(1.0f, 0.5f, 0.0f, 2.0f));
    camera.mTransform.position = vmath::Vector3(0.5f, 0.5f, 1.0f);
    gfx::gui::GUIElementHandle scene_element = map_scene_node->addElement( gfx::gui::SceneElement(camera));
    Ptr::WritePtr<gfx::Camera> cam_ptr = scene_element->get<gfx::gui::SceneElement>().getCameraWriter();
    gfx::gui::GUIStateWriter<MapViewerState> sw = state_handle.getStateWriterNoUpdate();
    sw->map_controller.setControlled(cam_ptr);

    gfx::SceneNode &scene_root = scene_element->get<gfx::gui::SceneElement>().getSceneRoot();

    gfx::SceneNodeHandle map_node = scene_root.addSceneNode();
    map_node->transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);

    gfx::SceneNodeHandle light_node = scene_root.addSceneNode();
    light_node->addLight(vmath::Vector4(0.5f, 0.5f, 1.0f, 0.0f),
                         vmath::Vector4(0.85f, 0.85f, 0.85f, 1.0f));


    map_scene_node->setGUIEventHandler([state_handle, scene_element](const gfx::gui::GUIEvent &event) {
        switch (event.get_type())
        {
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseDragEvent>::value):
            {
                const gfx::gui::MouseDragEvent &drag_event = event.get_const<gfx::gui::MouseDragEvent>();
                gfx::gui::GUIStateWriter<MapViewerState> sw_no_update = state_handle.getStateWriterNoUpdate();
                sw_no_update->map_controller.sendTurnSignals({drag_event.x_rel, drag_event.y_rel});
                sw_no_update->map_controller.update();
            }
            break;
        case (gfx::gui::GUIEvent::is_a<gfx::gui::MouseWheelScrollEvent>::value):
            {
                const gfx::gui::MouseWheelScrollEvent &mwscroll_event = event.get_const<gfx::gui::MouseWheelScrollEvent>();
                std::cout << "scrolled Map viewer by " << mwscroll_event.y_rel << std::endl;
                gfx::gui::GUIStateWriter<MapViewerState> sw_no_update = state_handle.getStateWriterNoUpdate();
                sw_no_update->map_controller.sendScrollSignal(mwscroll_event.y_rel);
                sw_no_update->map_controller.update();
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
        [map_node/*, loading_msg_node*/] (const events::GenerateWorldEvent &evt) {
            map_node->clearAll();
            //loading_msg_node->show(); // <--- Show loading message
    });

    events::Immediate::add_callback<events::FinishGenerateWorldEvent>(
        [map_node, /*loading_msg_node,*/ state_handle] (const events::FinishGenerateWorldEvent &evt) {
            Ptr::ReadPtr<state::MacroState> scene_data = evt.scene_data;
            createMap(map_node, scene_data);
            //loading_msg_node->hide(); // <--- Hide loading message
    });

    return map_scene_node;
}

}


#endif // MAPVIEWER_H
