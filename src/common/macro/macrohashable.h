#ifndef MACROHASHABLE_H
#define MACROHASHABLE_H

#include "../stdext.h"

#define MAKE_HASHABLE(type, ...)                        \
namespace std {                                         \
    template<> struct hash<type> {                      \
        std::size_t operator()(const type &t) const {   \
            std::size_t ret = 0;                        \
            StdExt::hash_combine(ret, __VA_ARGS__);             \
            return ret;                                 \
        }                                               \
    };                                                  \
}

//Usage:

//struct SomeHashKey {
//    std::string key1;
//    std::string key2;
//    bool key3;
//};

//MAKE_HASHABLE(SomeHashKey, t.key1, t.key2, t.key3)
//// now you can use SomeHashKey as key of an std::unordered_map

#endif // MACROHASHABLE_H
