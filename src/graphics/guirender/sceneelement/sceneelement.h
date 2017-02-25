#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "../../camera.h"
#include "../../scenenode.h"
#include "../../../common/pointer.h"

namespace gfx {

namespace gui {

class SceneElement
{
public:
    inline SceneElement(const Camera &camera) :
        mCamera(camera) {}

    inline Camera &getCamera() { return mCamera; }
    inline const Camera &getCameraConst() const { return mCamera; }

    inline Ptr::WritePtr<Camera> getCameraWriter()       {  return Ptr::WritePtr<Camera>(&mCamera);  }
    inline Ptr::ReadPtr<Camera>  getCameraReader() const {  return Ptr::ReadPtr<Camera>(&mCamera);   }

    inline const SceneNode &getSceneRootConst() const { return mSceneRoot; }
    inline SceneNode &getSceneRoot() { return mSceneRoot; }

private:
    Camera mCamera;
    SceneNode mSceneRoot;
};

}

}

#endif // SCENEELEMENT_H
