#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "../../camera.h"
#include "../../scenenode.h"

namespace gfx {

namespace gui {

class SceneElement
{
public:
    inline SceneElement(const Camera &camera) :
        mCamera(camera) {}

    inline Camera &getCamera() { return mCamera; }
    inline const Camera &getCameraConst() const { return mCamera; }

    inline const SceneNode &getSceneRootConst() const { return mSceneRoot; }
    inline SceneNode &getSceneRoot() { return mSceneRoot; }

private:
    Camera mCamera;
    SceneNode mSceneRoot;
};

}

}

#endif // SCENEELEMENT_H
