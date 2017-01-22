#ifndef MEMOIZE_H
#define MEMOIZE_H

#include <functional>
#include <tuple>
#include <map>

namespace stdext {

template <typename ReturnType, typename... Args>
std::function<ReturnType (Args...)> memoize(std::function<ReturnType (Args...)> func)
{
    std::map<std::tuple<Args...>, ReturnType> cache;
    return ([=](Args... args) mutable {
            tuple<Args...> t(args...);
            return cache.find(t) == cache.end()
                ? cache[t] : cache[t] = func(args...);
    });
}

}



#endif // MEMOIZE_H
