#ifndef WORLDVIEWER_H
#define WORLDVIEWER_H

#include "gui.h"
#include "guistyling.h"
#include "../createscene.h"
#include "../events/immediateevents.h"
#include "../common/threads/threadpool.h"
#include "../system/async.h"

namespace gui {

struct WorldViewerState : public gfx::gui::GUIStateBase
{
    bool hover;

    bool lmb_down;
    bool rmb_down;

    int32_t prev_mouse_x;
    int32_t prev_mouse_y;

    WorldViewerState() :
        hover(false),
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
            gfx::gui::GUITransform( {gfx::gui::HorzPos(0.5f, gfx::gui::Units::Percentage, gfx::gui::HorzAnchor::Middle),
                                     gfx::gui::VertPos(0.5f, gfx::gui::Units::Percentage, gfx::gui::VertAnchor::Middle)}, {0.15f, 0.10f} ));
    loading_msg_node->hide();
    loading_msg_node->addElement( gfx::gui::TextElement( "Generating world...", font ) );

    gfx::Camera camera(300, 300);
    camera.mTransform.position = vmath::Vector3(0.0, 0.0, 10.0f);
    gfx::gui::GUIElementHandle scene_element = world_scene_node->addElement( gfx::gui::SceneElement(camera));

    //world_scene_node->mouseClick.addCallback( std::move(onclick) );
    world_scene_node->mouseEnter.addCallback( [state_handle]()
        {
            gfx::gui::GUIStateWriter<WorldViewerState> sw = state_handle.getStateWriter();
            sw->hover = true;
        }
    );
    world_scene_node->mouseLeave.addCallback( [state_handle]()
        {
            gfx::gui::GUIStateWriter<WorldViewerState> sw = state_handle.getStateWriter();
            sw->hover = false;
        }
    );
    world_scene_node->mouseClick.addCallback( [state_handle]()
        {
            gfx::gui::GUIStateWriter<WorldViewerState> sw = state_handle.getStateWriter();
            sw->lmb_down = true;
        }
    );
    world_scene_node->mouseRelease.addCallback( [state_handle]()
        {
            gfx::gui::GUIStateWriter<WorldViewerState> sw = state_handle.getStateWriter();
            sw->lmb_down = false;
        }
    );
    /*world_scene_node->mouseMove.addCallback( [state_handle]()
        {
            gfx::gui::GUIStateWriter<WorldViewerState> sw = state_handle.getStateWriter();
            sw->lmb_down = false;
        }
    );*/


    events::Immediate::add_callback<events::GenerateWorldEvent>(
        [scene_element, loading_msg_node] (const events::GenerateWorldEvent &evt) {
            scene_element->get<gfx::gui::SceneElement>().getSceneRoot().clearAll();
            loading_msg_node->show(); // <--- Show loading message
            sys::Async::addJob(
                // The asynchronous operation
                        [evt]()->SceneData{
                            return createPlanetData(evt.planet_shape, evt.planet_size);
                        },
                // Process the result on return
                        [scene_element, loading_msg_node](const SceneData &scene_data)->void{
                            gfx::SceneNode &scene_node = scene_element->get<gfx::gui::SceneElement>().getSceneRoot();
                            createScene(scene_node, scene_data);
                            loading_msg_node->hide(); // <--- Hide loading message
                        });
        }
    );
    return world_scene_node;
}

}

#endif // WORLDVIEWER_H
