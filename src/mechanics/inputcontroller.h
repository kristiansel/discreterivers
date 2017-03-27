#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <array>

#include "../common/flags.h"
//#include "../common/pointer.h"


namespace mech {

class InputController
{
public:
    enum Signal {
        Idle          = 0b00000000,

        Forward       = 0b00000001,
        Backward      = 0b00000010,
        Left          = 0b00000100,
        Right         = 0b00001000,

        SpeedUp       = 0b00010000,
        SlowDown      = 0b00100000,
        Up            = 0b01000000,
        Down          = 0b10000000
    };

    using MoveSignalFlags = stdext::Flags<Signal, Signal::Idle>;
    using TurnSignals = std::array<float, 2>;
    using ScrollSignal = int32_t;
protected:
    MoveSignalFlags mSignalFlags;
    TurnSignals     mTurnSignals;
    ScrollSignal    mScrollSignal;
public:
    // events
    enum Event {
        None          = 0b00000000,
        ToggleOrtho   = 0b00000001,
    };

    InputController() { clearSignals(); }
    virtual ~InputController() {}

    virtual void update(float delta_time_sec) = 0; // update something based on the received signals

    inline void sendSignal(Signal signal)
    {
        mSignalFlags.setFlag(signal);
    }

    inline void sendTurnSignals(const TurnSignals &turn_signals)
    {
        mTurnSignals[0] = turn_signals[0];
        mTurnSignals[1] = turn_signals[1];
    }

    inline void sendScrollSignal(const ScrollSignal &scroll_signal)
    {
        mScrollSignal = scroll_signal;
    }

    inline void clearSignals()
    {
        mSignalFlags.clearAll();
        mTurnSignals = {0.0f, 0.0f};
        mScrollSignal = 0.0f;
    }

};

} // namespace mech

#endif // INPUTCONTROLLER_H
