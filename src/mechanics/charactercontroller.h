#ifndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include "inputcontroller.h"
#include "../common/pointer.h"
#include "../common/gfx_primitives.h"
#include "../physics/rigidbody.h"
#include "../common/macro/debuglog.h"
#include "../common/flags.h"

namespace mech {

struct PIController
{
    float p, i, set, acc;

    PIController(float p, float i, float set) :
        p(p), i(i), set(set), acc(0.0f) { /* ctor */ }

    float evalUpdate(float m, float dt)
    {
        float diff = m - set;
        float out = p * diff + i * acc;
        acc = acc + diff * dt;
        return out;
    }
};

class CharacterController : public InputController
{
    static constexpr float walk_speed =  5.0f; // m/s
    static constexpr float run_speed  = 10.0f; // m/s

    Ptr::WritePtr<RigidBody> mRigidBodyPtr;

    vmath::Quat mTargetOrientation;
    vmath::Quat mActualOrientation;
    float       mMouseTurnSpeed;

    PIController mForwardCtrl;
    PIController mSideCtrl;

public:
    CharacterController(Ptr::WritePtr<RigidBody> rigid_body_ptr = Ptr::WritePtr<RigidBody>(nullptr),
                        const vmath::Quat &start_orientation = vmath::Quat(0.0f, 0.0f, 0.0f, 1.0f),
                        float mouse_turn_speed = 2.0f) :
        mRigidBodyPtr(rigid_body_ptr),
        mTargetOrientation(start_orientation),
        mActualOrientation(start_orientation),
        mMouseTurnSpeed(mouse_turn_speed),
        mForwardCtrl(-600.0f, 2.0f, 0.0f),
        mSideCtrl(-600.0f, 2.0f, 0.0f)
        // many more to come
    {
        // clearSignals(); // done by base...
    }

    // mutators
    inline void setControlled(Ptr::WritePtr<RigidBody> rigid_body_ptr)
    {
        mRigidBodyPtr = rigid_body_ptr;
    }

    void update(float delta_time_sec);

    // read
    inline vmath::Quat getTargetOrientation() { return mTargetOrientation; }
};

inline void CharacterController::update(float delta_time_sec)
{
    if (mRigidBodyPtr)
    {
        // get some directions
        vmath::Vector3 forward = vmath::Matrix3(mActualOrientation) * vmath::Vector3(0.0f, 0.0f, -1.0f);    // should be unit
        btVector3 bt_gravity = mRigidBodyPtr->getGravity();
        vmath::Vector3 gravity(bt_gravity.getX(), bt_gravity.getY(), bt_gravity.getZ());
        vmath::Vector3 up = -vmath::normalize(gravity);                                                     // should be unit
        vmath::Vector3 right = vmath::cross(forward, up);                                                   // does not need norm

        float speed_forw_set = 0.0f
                          + mSignalFlags.checkFlag(Signal::Forward)   ?
                                (mSignalFlags.checkFlag(Signal::SpeedUp) ? run_speed : walk_speed) : 0.0f
                          + mSignalFlags.checkFlag(Signal::Backward)  ? -walk_speed : 0.0f;

        float speed_side_set = 0.0f
                          + mSignalFlags.checkFlag(Signal::Right) ?  walk_speed : 0.0f
                          + mSignalFlags.checkFlag(Signal::Left)  ? -walk_speed : 0.0f;

        DEBUG_LOG("set speed forward and right " << speed_forw_set << ", " << speed_side_set);

        if (speed_forw_set > 0.01f || speed_side_set > 0.01f)
        {
            mRigidBodyPtr->activate(true);
        }

        btVector3 v = mRigidBodyPtr->getLinearVelocity();
        float vel_forw = vmath::dot({v[0], v[1], v[2]}, forward);
        float vel_right = vmath::dot({v[0], v[1], v[2]}, right);

        DEBUG_LOG("vel_forw and vel_right: " << vel_forw << ", " << vel_right);

        mForwardCtrl.set = speed_forw_set;
        float forw_contr_force = mForwardCtrl.evalUpdate(vel_forw, delta_time_sec);

        mSideCtrl.set = speed_side_set;
        float side_contr_force = mSideCtrl.evalUpdate(vel_right, delta_time_sec);

        DEBUG_LOG("forw and side forces: " << forw_contr_force << ", " << side_contr_force);

        vmath::Vector3 result_force_vector = forward * forw_contr_force + right * side_contr_force;
        mRigidBodyPtr->applyCentralForce((const btVector3 &)(result_force_vector));

        /*if (mSignalFlags.checkFlag(Signal::Forward))
        {
            DEBUG_LOG("moving character forward! ");
            mRigidBodyPtr->applyCentralForce(20.0f*(const btVector3 &)(forward));
        }*/

        mTargetOrientation =
                vmath::Quat::rotation(mMouseTurnSpeed*mTurnSignals[0], up ) *
                vmath::Quat::rotation(mMouseTurnSpeed*mTurnSignals[1], right) *
                mTargetOrientation;

        // change this after a while...
        mActualOrientation = mTargetOrientation;

        //DEBUG_LOG("turn signals: " << mTurnSignals[0] << ", " << mTurnSignals[1]);
    }

    clearSignals();
}

}


#endif // CHARACTERCONTROLLER_H
