#ifndef DEFAULTINPUTCONTROLLER_H
#define DEFAULTINPUTCONTROLLER_H

#include "inputcontroller.h"

namespace mech {

class DefaultInputController : public InputController
{
public:
    void update() { clearSignals(); }
};

} // mech

#endif // DEFAULTINPUTCONTROLLER_H
