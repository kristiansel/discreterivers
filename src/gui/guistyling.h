#ifndef GUISTYLING_H
#define GUISTYLING_H

#include "../graphics/gfxcommon.h"

namespace gui {

namespace styling {

inline vmath::Vector4 colorGuiBase() { return vmath::Vector4{0.06, 0.09, 0.12, 0.6}; }

inline vmath::Vector4 colorGuiElement() { return 2.0f*colorGuiBase(); }

inline vmath::Vector4 colorGuiHighlight() { return 4.0f*colorGuiBase(); }

}

}

#endif // GUISTYLING_H
