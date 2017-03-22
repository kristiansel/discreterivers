#include "mechanicsmanager.h"

#include "../common/macro/debuglog.h"
#include "../events/immediateevents.h"

MechanicsManager::MechanicsManager(Ptr::WritePtr<gfx::Camera> camera_ptr) :
    mActiveInputCtrl( new mech::CameraController(camera_ptr) )
{
    events::Immediate::add_callback<events::CreateSceneEvent>(
        [this] (const events::CreateSceneEvent &evt) {

            const vmath::Vector3 &point_above = evt.scene_creation_info->anchor_pos;
            Ptr::ReadPtr<state::MacroState> scene_data = evt.scene_creation_info->macro_state_ptr;
            vmath::Vector3 local_up = vmath::normalize(scene_data->planet_base_shape->getGradDir(point_above));

            mech::CameraController *cam_ctrl = dynamic_cast<mech::CameraController*>(mActiveInputCtrl);
            if (cam_ctrl)
            {
                DEBUG_LOG("setting cam ctrler up direction");
                cam_ctrl->setUpDir(local_up);
            }
            else
            {
                DEBUG_LOG("active input controller is not a camera controller");
            }
    });
}

MechanicsManager::~MechanicsManager()
{
    delete mActiveInputCtrl;
}
