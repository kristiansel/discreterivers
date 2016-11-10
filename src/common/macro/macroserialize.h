#ifndef MACROSERIALIZE_H
#define MACROSERIALIZE_H

#include "macroforeach.h"

#define SERIALIZE_MEMBER(MEMBER) serialize(obj.MEMBER, res);
#define DESERIALIZE_MEMBER(MEMBER) obj.MEMBER = deserialize<decltype(obj.MEMBER)>(it, res.cend());

/**
 * Serialization macro
 * @param  CLASS   the class name to implement serialize/deserialize
 * @param  ...     list of member variables to serialize/deserialize. They must be public for now...
 */
#define IMPL_SERIALIZABLE(CLASS, ...)                                           \
                                                                                \
namespace Serial                                                                \
{                                                                               \
                                                                                \
template<>                                                                      \
inline void serialize<CLASS>(const CLASS &obj, StreamType &res)                 \
{                                                                               \
    FOR_EACH(SERIALIZE_MEMBER,__VA_ARGS__)                                      \
}                                                                               \
                                                                                \
template<>                                                                      \
inline CLASS deserialize<CLASS>(const StreamType &res)                          \
{                                                                               \
    StreamType::const_iterator it = res.cbegin();                               \
    CLASS obj;                                                                  \
    FOR_EACH(DESERIALIZE_MEMBER,__VA_ARGS__)                                    \
    return obj;                                                                 \
}                                                                               \
                                                                                \
}


// The above macro is based on observing the following code:

/*
namespace Serial
{

template<>
inline void serialize<AltPlanet::PlanetGeometry>(const AltPlanet::PlanetGeometry &obj, StreamType &res)
{
    serialize(obj.points, res);
    serialize(obj.triangles, res);
}


template<>
inline AltPlanet::PlanetGeometry deserialize<AltPlanet::PlanetGeometry>(const StreamType &res)
{
    StreamType::const_iterator it = res.cbegin();
    AltPlanet::PlanetGeometry obj;
    obj.points = deserialize<std::vector<vmath::Vector3>>(it, res.cend());
    obj.triangles = deserialize<std::vector<gfx::Triangle>>(it, res.cend());
    return obj;
}

} // namespace Serial
*/

#endif // MACROSERIALIZE_H
