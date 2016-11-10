#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#include <cmath>
#include <iostream>
#include <assert.h>

// TODO: Test n_cubic
// TODO: Templetize on interpolation order O:1 = Linear, O:2 = Quadratic, O:3 = Cubic etc.
//       interpolate<N, O>::eval(...)
// TODO: Combine with noise generator to create som funky 4d noise for effects/clouds etc...

class interpolate
{
public:
    template<typename T>
    static T cubic(T p[4], T x) {
        return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
    }

    template<typename T>
    static T bicubic(T p[4][4], T x, T y) {
        T arr[4];
        arr[0] = cubic(p[0], y);
        arr[1] = cubic(p[1], y);
        arr[2] = cubic(p[2], y);
        arr[3] = cubic(p[3], y);
        return cubic(arr, x);
    }

    /**
     * @brief tricubic
     * @param p: Cell corner function values. Cubic interpolation needs neighbor cells as well,
     *           therefore p[4] not p[2].
     * @param x: X-position within center cell. Scaled to between 0 and 1
     * @param y: Y-position within center cell. Scaled to between 0 and 1
     * @param z: Z-position within center cell. Scaled to between 0 and 1
     * @return
     */
    template<typename T>
    static T tricubic (T p[4][4][4], T x, T y, T z) {
        T arr[4];
        arr[0] = bicubic(p[0], y, z);
        arr[1] = bicubic(p[1], y, z);
        arr[2] = bicubic(p[2], y, z);
        arr[3] = bicubic(p[3], y, z);
        return cubic(arr, x);
    }

    template<typename T>
    static T n_cubic (int n, T* p, T coordinates[]) {
        assert(n > 0);
        if (n == 1) {
            return cubic(p, *coordinates);
        }
        else {
            T arr[4];
            int skip = 1 << (n - 1) * 2;
            arr[0] = n_cubic(n - 1, p, coordinates + 1);
            arr[1] = n_cubic(n - 1, p + skip, coordinates + 1);
            arr[2] = n_cubic(n - 1, p + 2*skip, coordinates + 1);
            arr[3] = n_cubic(n - 1, p + 3*skip, coordinates + 1);
            return cubic(arr, *coordinates);
        }
    }
};

#endif // INTERPOLATION_HPP


