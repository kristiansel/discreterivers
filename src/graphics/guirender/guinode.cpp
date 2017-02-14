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

GUINodePtr GUINode::handleMouseButtonDown(float x, float y, int32_t pix_x, int32_t pix_y) {
    gui::AABB aabb = mGUITransform.getAABB();
    if ( (x < aabb.xMax && x > aabb.xMin) &&
         (y < aabb.yMax && y > aabb.yMin) &&
          !mGUIFlags.checkFlag(GUIFlag::Hide) )
    {
        //std::cout << "clicked inside child " << x << ", " << y << std::endl;
        //mouseClick.invokeCallbacks();
        mGUIEventHandler(MouseButtonDownEvent{MouseButton::Left, pix_x, pix_y});

        float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
        float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

        GUINodePtr deepest_captured = mGUIFlags.checkFlag(GUIFlag::ClickPassThru) ? nullptr : this;
        for (auto &child : mChildren)
        {
            GUINodePtr child_captured = child.handleMouseButtonDown(x_rel, y_rel, pix_x, pix_y);
            deepest_captured = child_captured ? child_captured : deepest_captured;
        }
        return deepest_captured;
    }
    else
    {
        return nullptr;
    }
}

void GUINode::handleMouseButtonUp(float x, float y, int32_t pix_x, int32_t pix_y)
{
    gui::AABB aabb = mGUITransform.getAABB();

    //std::cout << "clicked inside child " << x << ", " << y << std::endl;
    //mouseRelease.invokeCallbacks();
    mGUIEventHandler(MouseButtonUpEvent{MouseButton::Left, pix_x, pix_y});

    float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
    float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

    for (auto &child : mChildren)
    {
        child.handleMouseButtonDown(x_rel, y_rel, pix_x, pix_y);
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
