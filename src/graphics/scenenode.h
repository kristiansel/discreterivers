#ifndef SCENENODE_H
#define SCENENODE_H

#include <list>

#include "light.h"
#include "transform.h"
#include "sceneobject.h"
#include "../common/gfx_primitives.h"


namespace vmath = Vectormath::Aos;

namespace gfx
{

class SceneNode;
using SceneNodeHandle   = std::list<SceneNode>::iterator;
using SceneObjectHandle = std::list<SceneObject>::iterator;
using LightHandle       = std::list<Light>::iterator;


struct SceneNode
{
    Transform transform;

    inline SceneNodeHandle addSceneNode();

    inline SceneObjectHandle addSceneObject(const Geometry &geometry,
                                     const Material &material);

    inline LightHandle addLight(const vmath::Vector4 &color,
                         const Transform &transform = Transform());

    inline void clearChildren() { mChildren.clear(); }
    inline void clearSceneObjects() { mSceneObjects.clear(); }
    inline void clearLights() { mLights.clear(); }
    inline void clearAll() { clearChildren(); clearSceneObjects(); clearLights(); }

    //SceneObject * getSceneObjectPtr(sceneobject_id id);

    //Light * getLightPtr(light_id id);

    const std::list<SceneNode> &getChildren()       const { return mChildren; }
    const std::list<SceneObject> &getSceneObjects() const { return mSceneObjects; }
    const std::list<Light> &getLights()             const { return mLights; }

private:
    friend class OpenGLRenderer;
    std::list<SceneNode> mChildren;
    std::list<SceneObject> mSceneObjects;
    std::list<Light> mLights;

    //scenenode_id mParent;
};

// implementation

inline SceneNodeHandle SceneNode::addSceneNode()
{
    mChildren.emplace_back( SceneNode() );
    return (--mChildren.end());
}

inline SceneObjectHandle SceneNode::addSceneObject(const Geometry &geometry,
                                         const Material &material)
{
    mSceneObjects.emplace_back( material, geometry );
    return (--mSceneObjects.end());
}


inline LightHandle SceneNode::addLight(const vmath::Vector4 &color,
                     const Transform &transform)
{
    mLights.emplace_back( transform, color );
    return (--mLights.end());
}

}

#endif // SCENENODE_H
