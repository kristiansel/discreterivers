#include "gfxscenemanager.h"

#include "../common/procedural/icosphere.h"


void GFXSceneManager::initScene(const vmath::Vector3 &point_above,
                                Ptr::ReadPtr<state::MacroState> scene_data,
                                const std::vector<state::Actor> &actors)
{
    // add stuff to scene
    gfx::SceneNodeHandle world_node = mGFXSceneRoot.addSceneNode();
    float cam_view_distance;
    createScene(world_node, scene_data, cam_view_distance);

    // set the sun light to be on this side of the planet
    vmath::Vector3 local_up = vmath::normalize(scene_data->planet_base_shape->getGradDir(point_above));
    vmath::Vector3 sun_pos = point_above + 50000.0f * local_up;
    gfx::SceneNodeHandle sun_node = mGFXSceneRoot.addSceneNode();
    sun_node->addLight(vmath::Vector4(sun_pos[0], sun_pos[1], sun_pos[2], 1.0f), vmath::Vector4(0.85f, 0.85f, 0.85f, 1.0f));

    DEBUG_LOG("local_up: " << local_up[0] << ", " << local_up[1] << ", " << local_up[2]);
    DEBUG_LOG("point_above: " << point_above[0] << ", " << point_above[1] << ", " << point_above[2]);

    for (int i = 0; i<actors.size(); i++)
    {
        const state::Actor &actor = actors[i];

        // add a box geometry..
        Procedural::Geometry proc_geom = actor.spec.type == state::Actor::Spec::Type::TestBox ?
                                            Procedural::boxPlanes(1.0f, 1.0f, 1.0f) :
                                            Procedural::icosahedron(1.0f);

        gfx::SceneNodeHandle actor_node = mGFXSceneRoot.addSceneNode();
        actor_node->addSceneObject(gfx::Geometry(gfx::Vertices(proc_geom.points, proc_geom.normals),
                                                 gfx::Primitives(proc_geom.triangles)),
                                    gfx::Material(vmath::Vector4(1.0f, 0.0f, 0.0f, 1.0f)));

        actor_node->transform.position = actor.pos;
        actor_node->transform.rotation = actor.rot;

        PhysTransformNode *pt_node = mActorTransformsPtr->create(PhysTransform{actor.pos, actor.rot, actor_node});

        if (actor.control == state::Actor::Control::Player)
        {
            vmath::Vector3 side_offset = 16.0f*vmath::cross(local_up, vmath::Vector3(1.0, 0.0, 0.0));
            mCamera.mTransform.lookAt(actor.pos+side_offset, actor.pos, local_up);
        }
    }
}
