#ifndef CIV_H
#define CIV_H

#include "../altplanet.h"
#include "../watersystem.h"

namespace AltPlanet {

namespace Civ {

    // types
    enum class ResourceType : int {
        Clay, Iron, Copper, Tin, Silver, Gold, Gems, Salt, Coal, Unobtainium // unobtainium should always be at the end!!!
    };

    struct Resource
    {
        int point_index;
        ResourceType resource_type;
    };

    struct City
    {
        // city data, name etc...
    };

    struct Road
    {
        // Road data, name, ends, lines etc... curve geometry?
    };

    struct Lake
    {
        // Road data, name, ends, lines etc... curve geometry?
    };

    struct River
    {
        // Road data, name, ends, lines etc... curve geometry?
    };

    struct Travellers
    {
        // list of pointers to travelling entities
    };

    struct Region
    {
        // region/province with name, for example mountain, lake area, highland, valley etc...
    };

    // hmm need to find out which are mutually exclusive and divide into layers...
    // for example city and resource can be there at the same time. Lake and city, not so much...
    // ??
    struct SparsePtFeatures
    {
        Resource *resource;
        City *city;
        Road *road;
        Lake *lake;
        River *river;
    };

    struct SparseLineFeatures
    {
        Resource *resource;
        City *city;
        Road *road;
    };

    struct SparseTriFeatures
    {
        Travellers *travellers; // parties intersecting this triangle...
    };

    // sparse features contained in a
    struct DensePointFeatures
    {
        // irradiance, humidity etc...
    };

    // need dense line features?

    struct DenseTriFeatures
    {
        Region *region;
    };

    // functions
    std::vector<Resource> distributeResources(const std::vector<vmath::Vector3> &points,
                                              std::vector<LandWaterType> &land_water_types);




    // place cities etc...

}

}

#endif // CIV_H
