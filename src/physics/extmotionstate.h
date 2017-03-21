#ifndef EXTMOTIONSTATE_H
#define EXTMOTIONSTATE_H

#include "btBulletDynamicsCommon.h"
#include "../graphics/scenenode.h"
#include "../common/macro/debuglog.h"
#include "../engine/phystransformcontainer.h"

class ExtMotionState : public btMotionState
{
protected:
    PhysTransformNode* mPhysTransfNodePtr;
    btTransform mInitialPosition;

public:
    ExtMotionState(const btTransform &initialPosition, PhysTransformNode* phys_transf_node_ptr)
    {
        mPhysTransfNodePtr = phys_transf_node_ptr;
        mInitialPosition = initialPosition;
    }

    virtual ~ExtMotionState()
    {
    }


    virtual void getWorldTransform(btTransform &worldTrans) const
    {
        DEBUG_LOG("MOTIONSTATE GET");
        worldTrans = mInitialPosition;
    }

    virtual void setWorldTransform(const btTransform &worldTrans)
    {
        /*if(mSceneNode == nullptr)
            return; // silently return before we set a node
        */
        btQuaternion rot = worldTrans.getRotation();
        mPhysTransfNodePtr->get().rot = vmath::Quat(rot.x(), rot.y(), rot.z(), rot.w());

        btVector3 pos = worldTrans.getOrigin();
        mPhysTransfNodePtr->get().pos = vmath::Vector3(pos.x(), pos.y(), pos.z());

        //DEBUG_LOG("MOTIONSTATE SET");
        //vmath::print(mPhysTransfNodePtr->get().pos);
    }
};

#endif // EXTMOTIONSTATE_H
