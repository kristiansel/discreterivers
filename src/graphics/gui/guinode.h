#ifndef GUINODE_H
#define GUINODE_H

#include <vector>
#include <iostream>

#include "guitransform.h"
#include "guielement.h"
#include "../../common/resmanager/refcounted.h"


namespace gfx {

namespace gui {

class GUINode : public Resource::RefCounted<GUINode>
{
public:
    GUINode(GUITransform && gui_transform) : mGUITransform(gui_transform) {}

    // Resource::RefCounted<GUINode>
    void resourceDestruct() { std::cout << "deleting gui node" << std::endl; }

private:
    GUINode();

    //std::vector<GUIElement> mGUIElements;

    GUITransform mGUITransform;
};

} // gui

} // gfx

#endif // GUINODE_H
