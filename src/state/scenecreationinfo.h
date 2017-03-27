#ifndef SCENECREATIONINFO_H
#define SCENECREATIONINFO_H

#include <vector>
#include "macrostate.h"
#include "../common/pointer.h"
#include "../common/gfx_primitives.h"

namespace state {

struct Actor
{
    //unsigned int id;

    vmath::Vector3 pos;
    vmath::Quat rot;

    struct Spec {
        enum class Type { TestBox, TestIco };
        Type type;
    } spec;

    enum class Control { Player, AI/*, Network*/ };
    Control control;

    // more info. All become boxes for now...
};

class SceneCreationInfo
{
    SceneCreationInfo() = delete;
public:
    Ptr::OwningPtr<MacroState> macro_state_ptr;
    float aspect_ratio;
    vmath::Vector3 land_pos;
    vmath::Vector3 point_above;
    std::vector<Actor> actors;

    SceneCreationInfo(Ptr::OwningPtr<MacroState> &&msp_in,
                      const vmath::Vector3 &land_pos_in,
                      const vmath::Vector3 &point_above_in) :
        macro_state_ptr(std::move(msp_in)),
        aspect_ratio(1.0f),
        land_pos(land_pos_in),
        point_above(point_above_in),
        actors()
    {
        // ctor...
    }

};

}

#endif // SCENECREATIONINFO_H
