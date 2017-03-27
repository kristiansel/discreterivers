#ifndef EULERPHYSICSSIM_H
#define EULERPHYSICSSIM_H

#include "../common/freelistset.h"

namespace Euler {

class PhysicsSim
{
public:
    PhysicsSim() {}

    void inline stepSimulation(float delta_time_sec);

};

void inline PhysicsSim::stepSimulation(float delta_time_sec)
{
    // do stuff
}


}

#endif // EULERPHYSICSSIM_H
