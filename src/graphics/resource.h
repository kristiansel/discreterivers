#ifndef RESOURCE_H
#define RESOURCE_H

#include <limits>

#include "gfxcommon.h"
#include "../common/typetag.h"
#include "../common/macro/macrodebugassert.h"

namespace gfx {

namespace Resource {

// ID would very much like the following syntax
// using ResID = typetag<size_t, 0>;

const size_t null_res_id = std::numeric_limits<size_t>::max();

struct resource_tag{};
typedef ID<resource_tag, size_t, null_res_id> ResID;

// Manager
template<class T>
class Manager
{
public:
    ResID add()
    {
        auto id = mResources.size();
        mResources.push_back(1); // ever growing
        std::cout << "RES ADD count: " << 1 << std::endl;
        return ResID(id);
    }

    size_t incr(ResID res_id)
    {
        DEBUG_ASSERT((size_t(res_id)<mResources.size()));
        DEBUG_ASSERT((res_id != ResID::invalid()));

        auto new_count = (++mResources[size_t(res_id)]);
        std::cout << "RES INCR count: " << new_count << std::endl;
        return new_count;

    }

    const size_t dont_destruct = 999; // a resource with count 999 will not destruct

    size_t decr(ResID res_id)
    {
        if (res_id == ResID::invalid()) {
            return dont_destruct; // return a high count value that will not result in an attempt at destruction
        }
        else {
            DEBUG_ASSERT((size_t(res_id)<mResources.size()));
            auto new_count = (--mResources[size_t(res_id)]);
            std::cout << "RES DECR count: " << new_count << std::endl;
            return new_count;
        }
    }

private:
    std::vector<size_t> mResources;
};

struct texture_tag{};
Manager<texture_tag> &TextureManager();


// HandleManger?

}

}
#endif // RESOURCE_H
