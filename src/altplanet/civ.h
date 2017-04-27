#ifndef CIV_H
#define CIV_H

#include "altplanet.h"
#include "watersystem.h"

namespace AltPlanet {

namespace Civ {

    // types
    enum class ResourceType : int {
        Clay, Iron, Copper, Tin, Silver, Gold, Gems, Salt, Coal
    };

    struct Resource
    {
        int point_index;
        ResourceType resource_type;
    };

    // functions
    std::vector<Resource> distributeResources(const std::vector<vmath::Vector3> &points,
                                              std::vector<LandWaterType> &land_water_types);

}

}

#endif // CIV_H
