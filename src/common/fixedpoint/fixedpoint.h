#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

#include <type_traits>
#include <iostream>

//template <typename T>
//using enable_if_arithmetic = typename std::enable_if<std::is_arithmetic<T>::value, T>::type;

// for example namespace fxp=fixedpoint
namespace fixedpoint {

template<
    typename T,
    T lshift,
    typename = typename std::enable_if<std::is_integral<T>::value, T>::type
    >
class fixed {
    static const T half_lshift = lshift/2;
    static_assert(2*half_lshift==lshift, "bitshift field `lshift` must be divisible by 2");
    static_assert(lshift<8*sizeof(T)-2, "bitshift field `lshift` must be less than 8*sizeof(T)-2");
public:
    // construction from numeric type
    template<
        typename A,
        typename = typename std::enable_if<std::is_arithmetic<A>::value, A>::type
        >
    inline fixed(A a) : val(static_cast<A>(a)*static_cast<A>(1 << lshift)) {}

    // conversion to normal numeric type
    template<
        typename A,
        typename = typename std::enable_if<std::is_arithmetic<A>::value, A>::type
        >
    inline operator A() const { return static_cast<A>(val)/static_cast<A>(1 << lshift); }

    fixed& operator+=(const fixed& x) { val += x.val; return *this; }
    fixed& operator-=(const fixed& x) { val -= x.val; return *this; }
    //fixed& operator*=(const fixed& x) { val >>= half_lshift; val *= (x.val >> half_lshift); return *this; }
    fixed& operator*=(const fixed& x) { int64_t tmp=(val*x.val)>>lshift; val=tmp; return *this; }
    fixed& operator/=(const fixed& x) { val /= x.val; val <<= lshift; return *this; }
    fixed& operator*=(T x) { val *= x; return *this; }
    fixed& operator/=(T x) { val /= x; return *this; }

    // (in)equality
    friend inline bool operator==(fixed a, fixed b) { return (a.val == b.val); }
    friend inline bool operator!=(fixed a, fixed b) { return (a.val != b.val); }
    friend inline bool operator <(fixed a, fixed b) { return (a.val < b.val); }
    friend inline bool operator >(fixed a, fixed b) { return (a.val > b.val); }

    // arithmetic
    friend inline fixed operator +(fixed a, fixed b) { return fromval(a.val + b.val); }
    friend inline fixed operator -(fixed a, fixed b) { return fromval(a.val - b.val); }
    //friend inline fixed operator *(fixed a, fixed b) { return fromval((a.val >> half_lshift) * (b.val >> half_lshift)); } // no temporary, bitshift arguments first to avoid overflow
    friend inline fixed operator *(fixed a, fixed b) { int64_t tmp=(a.val*b.val)>>lshift; return fromval(tmp); }
    friend inline fixed operator /(fixed a, fixed b) { return fromval((a.val / b.val) << lshift); }
    inline fixed operator-( ) { return fromval(-val); }

    fixed() {} // needed for allocation :(
private:
    // cannot declare fixed(T f) private in case implicit conversion from T is needed...
    // therefore "fromval"
    inline static fixed fromval(T &&v) {fixed out; out.val=v; return out;}

    T val;
};

}

namespace std {
    template<typename T, T d>
    ostream& operator<<(ostream& os, const fixedpoint::fixed<T, d>& fxp)
    {
        os << double(fxp);
        return os;
    }
}

#endif // FIXEDPOINT_H
