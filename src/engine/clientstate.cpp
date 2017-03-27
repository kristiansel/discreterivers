#include "clientstate.h"

ClientState::ClientState(state::SceneCreationInfo &new_game_info) :
   mGFXSceneManager(gfx::Camera(gfx::PerspectiveProjection(new_game_info.aspect_ratio, DR_M_PI_4, 0.0f, 1000000.0f)),
                    gfx::SceneNode(),
                    Ptr::WritePtr<PhysTransformContainer>(&mActorTransforms)),
   mPhysicsManager(Ptr::WritePtr<PhysTransformContainer>(&mActorTransforms)),
   mMechanicsManager(Ptr::WritePtr<gfx::Camera>(&mGFXSceneManager.mCamera), mPhysicsManager.getActorRigidBodyPoolWPtr()),
   mMacroStatePtr(std::move(new_game_info.macro_state_ptr))
{
    // ctor
    mGFXSceneManager.initScene(new_game_info.point_above, mMacroStatePtr.getReadPtr(), new_game_info.actors);
    mPhysicsManager.initScene(new_game_info.land_pos, mMacroStatePtr.getReadPtr(), new_game_info.actors);
    mMechanicsManager.initScene(new_game_info.land_pos, mMacroStatePtr.getReadPtr(), new_game_info.actors);

}
