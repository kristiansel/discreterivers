#ifndef MICROSTATECREATIONINFO_H
#define MICROSTATECREATIONINFO_H

#include <vector>

#include "../common/gfx_primitives.h"

struct ActorCreationInfo
{
    unsigned int id;

    vmath::Vector3 pos;
    vmath::Quat rot;

    // more info. All become boxes for now...
};

struct MicroStateCreationInfo
{
    vmath::Vector3 anchor_pos;
    std::vector<ActorCreationInfo> actors;
};

#endif // MICROSTATECREATIONINFO_H
