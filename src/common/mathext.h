#ifndef MATHEXT_H
#define MATHEXT_H

#include <algorithm>
#include <vector>
#include <iostream>

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

inline void normalizeFloatVec(std::vector<float> &float_vec, float to_max = 1.0f, float to_min = 0.0f)
{
    assert((max>min));

    float v_max = std::numeric_limits<float>::min();
    float v_min = std::numeric_limits<float>::max();
    for (int i = 0; i<float_vec.size(); i++)
    {

        if (float_vec[i] > v_max)
        {
            v_max = float_vec[i];
//            std::cout << "updating v_max with p" << i << std::endl;
//            std::cout << float_vec[i] << std::endl;
        }

        if (float_vec[i] < v_min)
        {
            v_min = float_vec[i];
//            std::cout << "updating v_min with p" << i << std::endl;
//            std::cout << float_vec[i] << std::endl;
        }
    }

//    std::cout << "in normalize float vec" << std::endl;
//    std::cout << "max: " << v_max << std::endl;
//    std::cout << "min: " << v_min << std::endl;

    for (int i = 0; i<float_vec.size(); i++)
    {
        float_vec[i] = (to_max-to_min)*(float_vec[i]-v_min)/(v_max-v_min)+to_min;
    }

}


}

#endif // MATHEXT_H
