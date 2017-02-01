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

// bah, need intrusive list for this...
// Benefits:
//      When count goes to zero, remove element in constant time, vs. ever growing now
//      Adding a new resource is linear time complex

/*

using CountType = std::size_t;
using ResHandle = std::list<CountType>::iterator;

template<class T>
class Manager
{
private:
    const CountType dont_destruct = 999; // a resource with count 999 will not destruct
public:
    Manager() : mResources({dont_destruct}) {}

    inline ResHandle getInvalidHandle() const { return mResources.begin(); } // needs to be the beginning, since the end keeps moving

    inline ResHandle add()
    {
        auto id = mResources.size();
        mResources.push_back(1);
        //std::cout << "RES ADD count: " << 1 << std::endl;
        return (--mResources.end());
    }

    inline CountType incr(ResHandle res_handle)
    {
        //DEBUG_ASSERT((ResIDInternal(res_id)<mResources.size()));
        //DEBUG_ASSERT((res_id != ResID::invalid()));

        auto new_count = (++(*res_handle));
        //std::cout << "RES INCR count: " << new_count << std::endl;
        return new_count;

    }


    CountType decr(ResHandle res_handle)
    {
        if (res_handle == getInvalidHandle()) {
            return dont_destruct; // return a high count value that will not result in an attempt at destruction
        }
        else {
            //DEBUG_ASSERT((ResIDInternal(res_id)<mResources.size()));
            auto new_count = (--(*res_handle));
            //std::cout << "RES DECR count: " << new_count << std::endl;
            return new_count;
        }
    }

private:
    std::list<CountType> mResources;
};

*/
}
#endif // RESOURCE_H
