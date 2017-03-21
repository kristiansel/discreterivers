#ifndef PHYSTRANSFORMCONTAINER_H
#define PHYSTRANSFORMCONTAINER_H

#include "../common/freelistset.h"
#include "../common/gfx_primitives.h"
#include "../graphics/scenenode.h"


struct PhysTransform
{
    vmath::Vector3 pos;
    vmath::Quat    rot;
    gfx::SceneNodeHandle scene_node_hdl;
};

using PhysTransformContainer = stdext::freelist_set<PhysTransform, 1000>;
using PhysTransformNode = typename PhysTransformContainer::node;


#endif // PHYSTRANSFORMCONTAINER_H
