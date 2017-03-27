#ifndef EXTMOTIONSTATE_H
#define EXTMOTIONSTATE_H

#include "btBulletDynamicsCommon.h"
#include "../common/macro/debuglog.h"

class ExtMotionState : public btMotionState
{
protected:
    btTransform mInitialPosition;

public:
    ExtMotionState(const btTransform &initialPosition)
    {
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
        //btQuaternion rot = worldTrans.getRotation();
        //btVector3 pos = worldTrans.getOrigin();
        //DEBUG_LOG("MOTIONSTATE SET");
        //vmath::print(mPhysTransfNodePtr->get().pos);
    }
};

#endif // EXTMOTIONSTATE_H
