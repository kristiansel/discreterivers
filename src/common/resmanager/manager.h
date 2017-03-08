#ifndef RESOURCE_H
#define RESOURCE_H

#include <limits>
#include <vector>
#include <iostream>
#include <list>

//#include "gfxcommon.h"
#include "../typetag.h"
#include "../macro/macrodebugassert.h"

namespace Resource {

// ID would very much like the following syntax
// using ResID = typetag<std::size_t, 0>;

using CountType = std::size_t;
using ResID = std::list<CountType>::iterator;

// Manager
template<class T>
class Manager
{
public:
    ResID add()
    {
        mResources.push_front(1);
        return mResources.begin();
    }

    CountType incr(ResID res_id)
    {
        CountType new_count = ++(*res_id);
        return new_count;
    }

    const CountType dont_destruct = 999; // a resource with count 999 will not destruct

    CountType decr(ResID res_id)
    {
        if (res_id == getInvalidResID()) {
            return dont_destruct; // return a high count value that will not result in an attempt at destruction
        }
        else
        {
            auto new_count = --(*res_id);
            if(new_count < 1)
            {
                //std::cout << "erasing refcount " << std::endl;
                mResources.erase(res_id);
            }
            return new_count;
        }
    }

    // could use a static completely new list here, but the end should not change if only push_front is used
    inline ResID getInvalidResID() { return mResources.end(); }


private:
    std::list<CountType> mResources;
};

}
#endif // RESOURCE_H
