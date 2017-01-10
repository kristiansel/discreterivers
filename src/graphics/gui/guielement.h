#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include "../../common/either.h"
#include "textelement/textelement.h"
#include "bgelement/bgelement.h"

namespace gfx {

namespace gui {

using GUIElement = stdext::either<TextElement, BackgroundElement/*, ImageElement*/>;

} // gui

} // gfx

#endif // GUIELEMENT_H
