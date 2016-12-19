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
        tx.mResourceID = Resource::ResID::invalid();
        //std::cout << "RefCounted MOVE constructed: " << std::endl;
    }

    inline RefCounted& operator= (const RefCounted& tx)
    {
        //std::cout << "RefCounted COPY ASSIGN operator: " << std::endl;
        return *this;
    }

    /** Move assignment operator */
    inline RefCounted& operator= (RefCounted&& tx) noexcept
    {
        destroyRef();
        mResourceID = tx.mResourceID; //data = other.data;
        tx.mResourceID = Resource::ResID::invalid(); //other.data = nullptr;
        //std::cout << "RefCounted MOVE ASSIGN operator: " << std::endl;
        return *this;
    }

    inline ~RefCounted()
    {
        destroyRef();
        //std::cout << "RefCounted DESTRUCTOR" << std::endl;
    }
private:
    Resource::ResID mResourceID;

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
