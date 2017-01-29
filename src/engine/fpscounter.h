#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include <chrono>

namespace engine {

class FPSCounter
{
    float mFPSFilteredVal;
    float mFPSFilterWeight;

    std::chrono::time_point<std::chrono::system_clock> mFrameStartTime;

    FPSCounter() = delete;
public:
    inline FPSCounter(float fps_filter_weight) :
        mFPSFilteredVal(0.0f),
        mFPSFilterWeight(fps_filter_weight),
        mFrameStartTime(std::chrono::system_clock::now())
    { /* default constructor */ }

    inline void startFrame() {
        mFrameStartTime = std::chrono::system_clock::now();
    }

    inline float getFrameFPSFiltered() {
        auto frame_end_time = std::chrono::system_clock::now();
        auto busy_frame_time =  std::chrono::duration<double, std::micro>(frame_end_time - mFrameStartTime);

        double busy_frame_microsecs = busy_frame_time.count();
        double working_fps          = 1000000.0/busy_frame_microsecs;
        mFPSFilteredVal = (1.0f-mFPSFilterWeight) * mFPSFilteredVal + mFPSFilterWeight * working_fps;

        return mFPSFilteredVal;
    }
};

} // namespace Engine

#endif // FPSCOUNTER_H
