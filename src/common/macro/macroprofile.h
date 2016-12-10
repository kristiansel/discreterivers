#ifndef MACROPROFILE_H
#define MACROPROFILE_H

#include <chrono>         // std::chrono::seconds
#include <iostream>

#define PROFILE_BEGIN(PROF_VAR_NAME) auto PROF_VAR_NAME ## _begin = std::chrono::high_resolution_clock::now();

#define PROFILE_END(PROF_VAR_NAME) \
        auto PROF_VAR_NAME ## _end = std::chrono::high_resolution_clock::now(); \
        std::cout << #PROF_VAR_NAME << ": " \
                  << std::chrono::duration_cast<std::chrono::milliseconds>(PROF_VAR_NAME ## _end - PROF_VAR_NAME ## _begin).count() \
                  << " milliseconds\n";

#endif // MACROPROFILE_H
