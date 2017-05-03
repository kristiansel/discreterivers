#include "civ.h"

#include "../../common/macro/debuglog.h"

namespace AltPlanet {

namespace Civ {

std::vector<Resource> distributeResources(const std::vector<vmath::Vector3> &points,
                                          std::vector<LandWaterType> &land_water_types)
{
    // create a vector of all land points,
    std::vector<int> land_point_indices;
    for (int i = 0; i<land_water_types.size(); i++)
    {
        if (land_water_types[i] == LandWaterType::Land)
        {
            land_point_indices.push_back(i);
        }
    }

    // give resources to a certain portion of them.... for example 1-5 % of points? See how it goes
    std::vector<Resource> resources_out;
    int n_res = static_cast<int>(land_point_indices.size()*0.02f);
    DEBUG_LOG("RESOURCE: Number total = " << n_res);
    DEBUG_ASSERT(n_res > 0);
    for (int i = 0; i<n_res; i++)
    {
        int land_pt_index = rand()%land_point_indices.size(); // woops is this correct...
        int res_type_int = rand()%static_cast<int>(ResourceType::Unobtainium); // Unobtainium = max resource type index + 1
        ResourceType res_type = static_cast<ResourceType>(res_type_int);
        resources_out.push_back({ land_pt_index, res_type });
    }

    return resources_out;
}


}

}
