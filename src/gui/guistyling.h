#ifndef GUISTYLING_H
#define GUISTYLING_H

#include "../graphics/gfxcommon.h"

namespace gui {

namespace styling {

//inline vmath::Vector4 colorGuiBase() { return vmath::Vector4{0.06, 0.09, 0.12, 0.6}; }

inline vmath::Vector4 colorGuiBase() { return vmath::Vector4{0.06, 0.09, 0.12, 1.0}; }

inline vmath::Vector4 colorGuiElement() { return 2.0f*colorGuiBase(); }

inline vmath::Vector4 colorGuiHighlight() { return 4.0f*colorGuiBase(); }

inline vmath::Vector4 colorGuiInactive() { return 1.3f*colorGuiBase(); }

inline vmath::Vector4 colorTextDefault() { return vmath::Vector4{1.0f, 1.0f, 1.0f, 1.0f};  }

inline vmath::Vector4 colorTextInactive() { return vmath::Vector4{0.3f, 0.3f, 0.3f, 1.0f};  }

}

}

#endif // GUISTYLING_H
