#include "euler.h"

#include "../../common/macro/debuglog.h"
#include "../../common/macro/macrodebugassert.h"

namespace Euler {

void testAll()
{
    BasicShape::Point  p0{{5.0f, 2.0f, 3.0f}};
    BasicShape::Sphere s0{{4.0f, 2.0f, 3.0f}, 1.2f};
    BasicShape::Sphere s1{{4.0f, 2.0f, 3.0f}, 0.8f};

    DEBUG_LOG("BEGIN TESTING");
    DEBUG_LOG("1");
    DEBUG_ASSERT(BasicShape::intersectPointSphere(p0, s0));
    DEBUG_LOG("2");
    DEBUG_ASSERT(!BasicShape::intersectPointSphere(p0, s1));
    DEBUG_LOG("3");
    DEBUG_ASSERT(BasicShape::intersectSphereSphere(s0, s1));

    BasicShape::AABB ab0{{4.0f, 1.0f, 2.0f}, {6.0f, 3.1f, 4.0f}};
    BasicShape::AABB ab1{{6.0f, 2.9f, 4.0f}, {7.0f, 4.0f, 5.0f}};
    DEBUG_LOG("4");
    DEBUG_ASSERT(BasicShape::intersectPointAABB(p0, ab0));
    DEBUG_LOG("5");
    DEBUG_ASSERT(!BasicShape::intersectPointAABB(p0, ab1));


    BasicShape::AABB ab2{{6.1f, 3.2f, 4.1f}, {7.0f, 4.0f, 5.0f}};
    DEBUG_LOG("6");
    DEBUG_ASSERT(BasicShape::intersectAABBvAABB(ab0, ab1));
    DEBUG_LOG("7");
    DEBUG_ASSERT(!BasicShape::intersectAABBvAABB(ab0, ab2));

    BasicShape::AABB ab3{{4.0f, 2.0f, 3.0f}, {7.0f, 4.0f, 5.0f}};
    BasicShape::AABB ab4{{-4.0f, 2.0f, 3.0f}, {-7.0f, 4.0f, 5.0f}};
    DEBUG_LOG("8");
    DEBUG_ASSERT(BasicShape::intersectSphereAABB(s1, ab3));
    DEBUG_LOG("9");
    DEBUG_ASSERT(!BasicShape::intersectSphereAABB(s1, ab4));
}

}
