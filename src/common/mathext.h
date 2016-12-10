#ifndef MATHEXT_H
#define MATHEXT_H

#include <algorithm>
#include <vector>

namespace MathExt
{

inline float frand(float LO, float HI)
{
    return LO + static_cast <float> (rand())/( static_cast <float> (RAND_MAX/(HI-LO)));
}

inline bool isnan_lame(float x)
{
    return x != x;
}

inline int wrap(int index, int period)
{
    return index < 0 ? period + (index % period) : index % period;
}


template<typename T>
inline T clamp(const T& in, const T& min, const T& max)
{
    return std::max(std::min(in, max), min);
}

inline void normalizeFloatVec(std::vector<float> &float_vec)
{

}


}

#endif // MATHEXT_H
