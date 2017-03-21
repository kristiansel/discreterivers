#ifndef TESTMOTIONSTATE_H
#define TESTMOTIONSTATE_H

#include "btBulletDynamicsCommon.h"
#include "../graphics/scenenode.h"
#include "../common/macro/debuglog.h"
#include "../engine/phystransformcontainer.h"


class TestMotionState : public btMotionState
{
protected:
    btTransform mInitialPosition;

public:
    TestMotionState(const btTransform &initialPosition)
    {
        mInitialPosition = initialPosition;
    }

    virtual ~TestMotionState()
    {
    }

    virtual void getWorldTransform(btTransform &worldTrans) const
    {
        DEBUG_LOG("TEST MOTIONSTATE GET");
        worldTrans = mInitialPosition;
    }

    virtual void setWorldTransform(const btTransform &worldTrans)
    {
        /*if(mSceneNode == nullptr)
            return; // silently return before we set a node
        */
        btQuaternion rot = worldTrans.getRotation();

        btVector3 pos = worldTrans.getOrigin();

        DEBUG_LOG("TEST MOTIONSTATE SET: " << pos.x() << ", " << pos.y() << ", " << pos.z());
        //vmath::print(mPhysTransfNodePtr->get().pos);
    }
};

#endif // TESTMOTIONSTATE_H
