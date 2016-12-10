#ifndef PROFILE_H
#define PROFILE_H

#include "../macro/macroprofile.h"
#include "fixedpoint.h"
#include <iostream>

namespace fixedpoint {

const int32_t fxbitshift = 28;
typedef fixed<int32_t, fxbitshift> fix28;

void profile() {
    fix28 a = -3.456;
    fix28 pos_a = -a;
    std::cout << "a = " << a << std::endl;
    std::cout << "pos_a = " << pos_a << std::endl;
    fix28 b = a+a;
    std::cout << "b = " << b << std::endl;
    fix28 c = b/a;
    std::cout << "c = " << c << std::endl;
    fix28 d = c*a;
    std::cout << "d = " << d << std::endl;
    bool bisd = b==d;
    std::cout << "b==d: " << bisd << std::endl;
    d += 5;
    std::cout << "d = " << d << std::endl;
    d *= 2;
    std::cout << "d = " << d << std::endl;

    std::cout << "profiling normal floats " << std::endl;

    int nb = 10000000;
    int n = 4*nb;
    float *pfnums = new float [n];
    float *pfadds = new float [n];
    for (int i=0;i<n;i++) {
        pfnums[i]=i%100;
        pfadds[i]=100-(i%100);
    }

    PROFILE_BEGIN(addfloat)
    for (int i=0;i<n;i++) {
        pfnums[i]+=pfadds[i];
    }
    PROFILE_END(addfloat);


    PROFILE_BEGIN(mulfloat)
    for (int i=0;i<n;i++) {
        pfnums[i]*=pfadds[i];
    }
    PROFILE_END(mulfloat);


    PROFILE_BEGIN(divfloat)
    for (int i=0;i<n;i++) {
        pfnums[i]/=pfadds[i];
    }
    PROFILE_END(divfloat);


    std::cout << "profiling sse" << std::endl;
    for (int i=0;i<n;i++) {
        pfnums[i]=i%100;
        pfadds[i]=50;
    }

    PROFILE_BEGIN(addsse)
    for (int i=0;i<n;i+=4) {
        pfnums[i]+=pfadds[i];
        pfnums[i+1]+=pfadds[i+1];
        pfnums[i+2]+=pfadds[i+2];
        pfnums[i+3]+=pfadds[i+3];
    }
    PROFILE_END(addsse);


    PROFILE_BEGIN(mulsse)
    for (int i=0;i<n;i+=4) {
        pfnums[i]*=pfadds[i];
        pfnums[i+1]*=pfadds[i+1];
        pfnums[i+2]*=pfadds[i+2];
        pfnums[i+3]*=pfadds[i+3];
    }
    PROFILE_END(mulsse);


    PROFILE_BEGIN(divsse)
    for (int i=0;i<n;i+=4) {
        pfnums[i]/=pfadds[i];
        pfnums[i+1]/=pfadds[i+1];
        pfnums[i+2]/=pfadds[i+2];
        pfnums[i+3]/=pfadds[i+3];
    }
    PROFILE_END(divsse);

    std::cout << "profiling fix28" << std::endl;
    fix28 *pinums = new fix28 [n];
    fix28 *piadds = new fix28 [n];
    for (int i=0;i<n;i++) {
        pinums[i]=i%100;
        piadds[i]=50;
    }

    PROFILE_BEGIN(addint)
    for (int i=0;i<n;i++) {
        pinums[i]+=piadds[i];
    }
    PROFILE_END(addint);


    PROFILE_BEGIN(mulint)
    for (int i=0;i<n;i++) {
        pinums[i]*=piadds[i];
    }
    PROFILE_END(mulint);

    PROFILE_BEGIN(divint)
    for (int i=0;i<n;i++) {
        pinums[i]/=piadds[i];
    }
    PROFILE_END(divint);

    std::cout << "profiling sse" << std::endl;

    PROFILE_BEGIN(addintsse)
    for (int i=0;i<n;i+=4) {
        pinums[i]+=piadds[i];
        pinums[i+1]+=piadds[i+1];
        pinums[i+2]+=piadds[i+2];
        pinums[i+3]+=piadds[i+3];
    }
    PROFILE_END(addintsse);


    PROFILE_BEGIN(mulintsse)
    for (int i=0;i<n;i+=4) {
        pinums[i]*=piadds[i];
        pinums[i+1]*=piadds[i+1];
        pinums[i+2]*=piadds[i+2];
        pinums[i+3]*=piadds[i+3];
    }
    PROFILE_END(mulintsse);


    PROFILE_BEGIN(divintsse)
    for (int i=0;i<n;i+=4) {
        pinums[i]/=piadds[i];
        pinums[i+1]/=piadds[i+1];
        pinums[i+2]/=piadds[i+2];
        pinums[i+3]/=piadds[i+3];
    }
    PROFILE_END(divintsse);
}

}

#endif // PROFILE_H
