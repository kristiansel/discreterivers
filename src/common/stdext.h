#ifndef STDEXT_H
#define STDEXT_H

#include <vector>
#include <functional>
#include "../common/gfx_primitives.h"

namespace StdExt {

//template<typename T>
inline void remove_index(std::vector<vmath::Vector3>& vector, const std::vector<int>& to_remove)
{
    auto vector_base = vector.begin();
    std::vector<vmath::Vector3>::size_type down_by = 0;

    for (auto iter = to_remove.cbegin();
         iter < to_remove.cend();
         iter++, down_by++)
    {
        std::vector<vmath::Vector3>::size_type next = (iter + 1 == to_remove.cend()
                                          ? vector.size()
                                          : *(iter + 1));

        std::move(vector_base + *iter + 1,
                  vector_base + next,
                  vector_base + *iter - down_by);
    }
    vector.resize(vector.size() - to_remove.size());
}


//template<typename A, typename B, typename A (*F)(const &B)>
template<typename A, typename B>
inline std::vector<B> vector_map(const std::vector<A> &v_in, std::function<B(const A&)> func)
{
    std::vector<B> v_out;
    v_out.reserve(v_in.size());

    for (const A &el : v_in) v_out.push_back(func(el));

    return v_out;
}

}

#endif // STDEXT_H
