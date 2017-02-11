#include "guinode.h"

namespace gfx {

namespace gui {

void onGUINodeStateUpdate(GUINode &gui_node)
{
    //std::cout << " on state update " << std::endl;
    gui_node.stateUpdate.invokeCallbacks();
    for (auto &child : gui_node.mChildren )
    {
        onGUINodeStateUpdate(child);
    }
}

bool GUINode::handleMouseClick(float x, float y) {
    gui::AABB aabb = mGUITransform.getAABB();
    if ( (x < aabb.xMax && x > aabb.xMin) &&
         (y < aabb.yMax && y > aabb.yMin) &&
          !mGUIFlags.checkFlag(GUIFlag::Hide) )
    {
        //std::cout << "clicked inside child " << x << ", " << y << std::endl;
        mouseClick.invokeCallbacks();

        float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
        float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

        bool result = !mGUIFlags.checkFlag(GUIFlag::ClickPassThru);
        for (auto &child : mChildren)
        {
            bool child_solid = child.handleMouseClick(x_rel, y_rel);
            result = result || child_solid;
        }
        return result;
    }
    else
    {
        return false;
    }
}

GUINodePtr GUINode::getDeepestHovered(float x, float y, bool debug)
{
    gui::AABB aabb = mGUITransform.getAABB();
    if ( (x < aabb.xMax && x > aabb.xMin) &&
         (y < aabb.yMax && y > aabb.yMin) &&
          !mGUIFlags.checkFlag(GUIFlag::Hide) )
    {
        //if (debug) std::cout << " inside: " << this << std::endl;
        float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
        float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

        //if (debug) std::cout << " # children: " << mChildren.size() << std::endl;
        GUINodePtr child_hovered = nullptr;
        for (auto &child : mChildren)
        {
            GUINodePtr result = child.getDeepestHovered(x_rel, y_rel);
            child_hovered = result ? result : child_hovered;
        }
        //if (debug) std::cout << " child_hovered: " << (child_hovered ? true : false) << std::endl;
        return (child_hovered != nullptr) ? child_hovered : this;
    }
    else
    {
        //if (debug) std::cout << " not inside: " << this << std::endl;
        return nullptr;
    }
}


} // gui

} // gfx
