#include "guinode.h"

namespace gfx {

namespace gui {

GUINode::GUINode(const GUITransform &gui_transform) :
    mGUITransform(gui_transform), mGUIFlags(GUIFlag::Default), mGUIState(/*this*/), mGUIEventHandler([](const GUIEvent&){})
{
    addStateUpdateCallback([this](){
        for (auto &child : this->mChildren )
        {
            child.onStateUpdate();
        }
    });
}

GUINodePtr GUINode::getDeepestClicked(float x, float y, float w_abs, float h_abs)
{
    return GUINode::getDeepestIntersectingNode(x, y, w_abs, h_abs, GUIFlag::ClickPassThru);
}

GUINodePtr GUINode::getDeepestHovered(float x, float y, float w_abs, float h_abs, std::vector<GUINodePtr> &hovered)
{
    gui::AABB aabb = mGUITransform.getAABB(w_abs, h_abs);
    if ( (x < aabb.xMax && x > aabb.xMin) &&
         (y < aabb.yMax && y > aabb.yMin) &&
          !mGUIFlags.checkFlag(GUIFlag::Hide) )
    {


        float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
        float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

        float this_w_abs = mGUITransform.getSize().x.getRelative(w_abs) * w_abs;
        float this_h_abs = mGUITransform.getSize().y.getRelative(h_abs) * h_abs;

        //GUINodePtr deepest_intersected = mGUIFlags.checkAny(flag_mask) ? nullptr : this;
        GUINodePtr deepest_intersected = GUINodePtr(this);
        hovered.push_back(GUINodePtr(this));
        for (auto &child : mChildren)
        {
            GUINodePtr child_intersected = child.getDeepestHovered(x_rel, y_rel, this_w_abs, this_h_abs, hovered);

            deepest_intersected = child_intersected ? child_intersected : deepest_intersected;
        }
        return deepest_intersected;
    }
    else
    {
        return GUINodePtr(nullptr);
    }
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

        GUINodePtr deepest_intersected = mGUIFlags.checkAny(flag_mask) ? GUINodePtr(nullptr) : GUINodePtr(this);
        for (auto &child : mChildren)
        {
            GUINodePtr child_intersected = child.getDeepestIntersectingNode(x_rel, y_rel, this_w_abs, this_h_abs, flag_mask);

            deepest_intersected = child_intersected ? child_intersected : deepest_intersected;
        }
        return deepest_intersected;
    }
    else
    {
        return GUINodePtr(nullptr);
    }
}

inline bool GUINode::thisSizeChangesWithParent()
{
    return ((mGUITransform.getSize().x.units == Units::Relative || mGUITransform.getSize().x.mParentMinus) ||
            (mGUITransform.getSize().y.units == Units::Relative || mGUITransform.getSize().y.mParentMinus));
}

inline void GUINode::onStateUpdate()
{
    //std::cout << " on state update " << std::endl;
    mGUIState.invokeCallbacks();
    for (auto &child : mChildren )
    {
        child.onStateUpdate();
    }
}

void GUINode::forceStateUpdate() { onStateUpdate(); }

void GUINode::resize(float w_abs, float h_abs)
{
    //if (thisSizeChangesWithParent())
    //{
        float this_w_abs = mGUITransform.getSize().x.getRelative(w_abs) * w_abs;
        float this_h_abs = mGUITransform.getSize().y.getRelative(h_abs) * h_abs;

        handleEvent(ResizedEvent{this_w_abs, this_h_abs});

        for (auto &child : mChildren)
        {
            child.resize(this_w_abs, this_h_abs);
        }
    //}
}


} // gui

} // gfx
