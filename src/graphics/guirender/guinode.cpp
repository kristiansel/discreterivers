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

/*
GUINodePtr GUINode::handleMouseButtonDown(float x, float y, float w_abs, float h_abs) {
    gui::AABB aabb = mGUITransform.getAABB(w_abs, h_abs);
    if ( (x < aabb.xMax && x > aabb.xMin) &&
         (y < aabb.yMax && y > aabb.yMin) &&
          !mGUIFlags.checkFlag(GUIFlag::Hide) )
    {

        float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
        float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

        float this_w_abs = mGUITransform.getSize().x.getRelative(w_abs) * w_abs;
        float this_h_abs = mGUITransform.getSize().y.getRelative(h_abs) * h_abs;

        GUINodePtr deepest_captured = mGUIFlags.checkFlag(GUIFlag::ClickPassThru) ? nullptr : this;
        for (auto &child : mChildren)
        {
            GUINodePtr child_captured = child.handleMouseButtonDown(x_rel, y_rel, this_w_abs, this_h_abs);

            deepest_captured = child_captured ? child_captured : deepest_captured;
        }
        return deepest_captured;
    }
    else
    {
        return nullptr;
    }
}*/

GUINodePtr GUINode::getDeepestClicked(float x, float y, float w_abs, float h_abs)
{
    return GUINode::getDeepestIntersectingNode(x, y, w_abs, h_abs, GUIFlag::ClickPassThru);
}

GUINodePtr GUINode::getDeepestHovered(float x, float y, float w_abs, float h_abs)
{
    return GUINode::getDeepestIntersectingNode(x, y, w_abs, h_abs);
}


inline GUINodePtr GUINode::getDeepestIntersectingNode(float x, float y, float w_abs, float h_abs, GUIFlag flag_mask) {
    gui::AABB aabb = mGUITransform.getAABB(w_abs, h_abs);
    if ( (x < aabb.xMax && x > aabb.xMin) &&
         (y < aabb.yMax && y > aabb.yMin) &&
          !mGUIFlags.checkFlag(GUIFlag::Hide) )
    {

        float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
        float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

        float this_w_abs = mGUITransform.getSize().x.getRelative(w_abs) * w_abs;
        float this_h_abs = mGUITransform.getSize().y.getRelative(h_abs) * h_abs;

        GUINodePtr deepest_intersected = mGUIFlags.checkAny(flag_mask) ? nullptr : this;
        for (auto &child : mChildren)
        {
            GUINodePtr child_intersected = child.getDeepestIntersectingNode(x_rel, y_rel, this_w_abs, this_h_abs, flag_mask);

            deepest_intersected = child_intersected ? child_intersected : deepest_intersected;
        }
        return deepest_intersected;
    }
    else
    {
        return nullptr;
    }
}

inline bool GUINode::thisSizeChangesWithParent()
{
    return ((mGUITransform.getSize().x.units == Units::Relative || mGUITransform.getSize().x.mParentMinus) ||
            (mGUITransform.getSize().y.units == Units::Relative || mGUITransform.getSize().y.mParentMinus));
}

void GUINode::resize(float w_abs, float h_abs)
{
    if (thisSizeChangesWithParent())
    {
        float this_w_abs = mGUITransform.getSize().x.getRelative(w_abs) * w_abs;
        float this_h_abs = mGUITransform.getSize().y.getRelative(h_abs) * h_abs;

        handleEvent(ResizedEvent{this_w_abs, this_h_abs});

        for (auto &child : mChildren)
        {
            child.resize(this_w_abs, this_h_abs);
        }
    }
}

/*
GUINodePtr GUINode::getDeepestHovered(float x, float y, float w_abs, float h_abs)
{
    gui::AABB aabb = mGUITransform.getAABB(w_abs, h_abs);
    if ( (x < aabb.xMax && x > aabb.xMin) &&
         (y < aabb.yMax && y > aabb.yMin) &&
          !mGUIFlags.checkFlag(GUIFlag::Hide) )
    {
        //if (debug) std::cout << " inside: " << this << std::endl;
        float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
        float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

        float this_w_abs = mGUITransform.getSize().x.getRelative(w_abs) * w_abs;
        float this_h_abs = mGUITransform.getSize().y.getRelative(h_abs) * h_abs;

        //if (debug) std::cout << " # children: " << mChildren.size() << std::endl;
        GUINodePtr child_hovered = nullptr;
        for (auto &child : mChildren)
        {
            GUINodePtr result = child.getDeepestHovered(x_rel, y_rel, this_w_abs, this_h_abs);
            child_hovered = result ? result : child_hovered;
        }
        //if (debug) std::cout << " child_hovered: " << (child_hovered ? true : false) << std::endl;
        return (child_hovered != nullptr) ? child_hovered : this;
    }
    else
    {
        return nullptr;
    }
}
*/

} // gui

} // gfx
