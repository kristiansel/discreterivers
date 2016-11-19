#ifndef MACROPROFILE_H
#define MACROPROFILE_H

#include <chrono>         // std::chrono::seconds
#include <iostream>

#define PROFILE_BEGIN() auto profile_begin = std::chrono::high_resolution_clock::now();

#define PROFILE_END(PROFILE_TEXT) \
        auto profile_finish = std::chrono::high_resolution_clock::now(); \
        std::cout << #PROFILE_TEXT << ": " \
                  << std::chrono::duration_cast<std::chrono::milliseconds>(profile_finish - profile_begin).count() \
                  << " milliseconds\n";

#endif // MACROPROFILE_H
