#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "transform.h"
#include "material.h"
#include "geometry.h"
#include "renderflags.h"

namespace gfx
{

class SceneObject
{
public:
    SceneObject(const Material &material, const Geometry &geometry)
        : mMaterial(material), mGeometry(geometry) {}

    void toggleVisible() { mFlags.toggleFlag(RenderFlags::Hidden); }
    void setWireframe(bool w) { w ? mFlags.setFlag(RenderFlags::Wireframe) : mFlags.clearFlag(RenderFlags::Wireframe); }

    RenderFlags getRenderFlags() const { return mFlags; }

    Material mMaterial;
    Geometry mGeometry;
    RenderFlags mFlags;
private:
    SceneObject();
};

} // namespace gfx

#endif // SCENEOBJECT_H
