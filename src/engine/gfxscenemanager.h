#ifndef GFXSCENEMANAGER_H
#define GFXSCENEMANAGER_H

#include "phystransformcontainer.h"
#include "../createscene.h"
#include "../graphics/camera.h"
#include "../graphics/scenenode.h"
#include "../events/immediateevents.h"
#include "../common/macro/debuglog.h"
#include "../common/procedural/boxgeometry.h"
#include "../common/procedural/planegeometry.h"

struct GFXSceneManager
{
    inline GFXSceneManager(gfx::Camera &&cam,
                           gfx::SceneNode &&scene_node,
                           Ptr::WritePtr<PhysTransformContainer> actor_transforms_ptr);

    gfx::Camera                 mCamera;              // init order important!
    gfx::SceneNode              mGFXSceneRoot;       // main game scene

    //PhysTransformContainer      mPhysTransforms;
    Ptr::WritePtr<PhysTransformContainer> mActorTransformsPtr;

    void initScene(const vmath::Vector3 &point_above,
                   Ptr::ReadPtr<state::MacroState> scene_data,
                   const std::vector<state::Actor> &actors);

    inline void updateGraphicsTransforms();
    //inline PhysTransformContainer * getActorTransformsPtr() { return &mPhysTransforms; }
    const gfx::Camera     inline &getActiveCamera() const { return mCamera; }
    const gfx::SceneNode  inline &getGFXSceneRoot() const { return mGFXSceneRoot; }
};

inline GFXSceneManager::GFXSceneManager(gfx::Camera &&cam,
                                        gfx::SceneNode &&scene_node,
                                        Ptr::WritePtr<PhysTransformContainer> actor_transforms_ptr) :
    mCamera(std::move(cam)),
    mGFXSceneRoot(std::move(scene_node)),
    mActorTransformsPtr(actor_transforms_ptr)
{
    //ctor
}

#endif // GFXSCENEMANAGER_H
