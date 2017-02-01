#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include "../../common/either.h"
#include "bgelement/bgelement.h"
#include "imageelement/imageelement.h"
#include "textelement/textelement.h"
#include "textelement/monospacetextelement.h"


namespace gfx {

namespace gui {

using GUIElement = stdext::either<TextElement, MonospaceTextElement, ImageElement, BackgroundElement>;

} // gui

} // gfx

#endif // GUIELEMENT_H
