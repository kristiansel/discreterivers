#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include "manager.h"

namespace Resource {

template<class Managed>
class RefCounted
{
    // how to static assert that refDestruct is implemented
public:
    inline RefCounted()
    {
        mResourceID = getManager().add();
        //std::cout << "RefCounted DEFAULT constructor" << std::endl;
    }

    inline RefCounted(const RefCounted &tx) : mResourceID(tx.mResourceID)
    {
        getManager().incr(mResourceID);
        //std::cout << "RefCounted COPY constructed" <<std::endl;
    }

    inline RefCounted(RefCounted &&tx) : mResourceID(tx.mResourceID)
    {
        tx.mResourceID = getManager().getInvalidResID();
        //std::cout << "RefCounted MOVE constructed: " << std::endl;
    }

    inline RefCounted& operator= (const RefCounted& tx)
    {
        destroyRef();
        mResourceID = tx.mResourceID;
        getManager().incr(mResourceID);
        return *this;
    }

    /** Move assignment operator */
    inline RefCounted& operator= (RefCounted&& tx) noexcept
    {
        destroyRef();
        mResourceID = tx.mResourceID;
        tx.mResourceID = getManager().getInvalidResID();
        return *this;
    }

    inline ~RefCounted()
    {
        destroyRef();
        //std::cout << "RefCounted DESTRUCTOR" << std::endl;
    }
private:
    ResID mResourceID;

    inline void destroyRef()
    {
        size_t new_count = getManager().decr(mResourceID);
        if (new_count == 0)
        {
            static_cast<Managed&>(*this).resourceDestruct();
        }
    }

    static Resource::Manager<Managed> &getManager()
    {
       static Resource::Manager<Managed> sResourceManager;
       return sResourceManager;
    }
};

}

#endif // REFCOUNTED_H
