#ifndef GFXRESOURCES_H
#define GFXRESOURCES_H

#include <vector>
#include "gfxcommon.h"

namespace gfx {

class Resource
{
public:
    typedef int KeyType;

    struct Info { KeyType key; GLuint id; };
    Info create();
    void destroy();

private:
    Resource();

    typedef int RefCountType;
    std::vector<RefCountType> mRefCounts;
};





}

#endif // GFXRESOURCES_H
