#ifndef RESOURCE_H
#define RESOURCE_H

#include <limits>
#include <vector>
#include <iostream>

//#include "gfxcommon.h"
#include "../typetag.h"
#include "../macro/macrodebugassert.h"

namespace Resource {

// ID would very much like the following syntax
// using ResID = typetag<std::size_t, 0>;

const std::size_t null_res_id = std::numeric_limits<std::size_t>::max();

struct resource_tag{};
typedef ID<resource_tag, std::size_t, null_res_id> ResID;

// Manager
template<class T>
class Manager
{
public:
    ResID add()
    {
        auto id = mResources.size();
        mResources.push_back(1); // ever growing
        //std::cout << "RES ADD count: " << 1 << std::endl;
        return ResID(id);
    }

    std::size_t incr(ResID res_id)
    {
        DEBUG_ASSERT((std::size_t(res_id)<mResources.size()));
        DEBUG_ASSERT((res_id != ResID::invalid()));

        auto new_count = (++mResources[std::size_t(res_id)]);
        //std::cout << "RES INCR count: " << new_count << std::endl;
        return new_count;

    }

    const std::size_t dont_destruct = 999; // a resource with count 999 will not destruct

    std::size_t decr(ResID res_id)
    {
        if (res_id == ResID::invalid()) {
            return dont_destruct; // return a high count value that will not result in an attempt at destruction
        }
        else {
            DEBUG_ASSERT((std::size_t(res_id)<mResources.size()));
            auto new_count = (--mResources[std::size_t(res_id)]);
            //std::cout << "RES DECR count: " << new_count << std::endl;
            return new_count;
        }
    }

private:
    std::vector<std::size_t> mResources;
};

}
#endif // RESOURCE_H
