#include "physicssubsystem.h"

PhysicsSubsystem::PhysicsSubsystem()
{

}


void PhysicsSubsystem::onCreateMicroState()
{
    //collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    mCollisionConfiguration = new btDefaultCollisionConfiguration();

    //use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    mCollisionDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

    //btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    mOverlappingPairCache = new btDbvtBroadphase();
    //mOverlappingPairCache = new btAxisSweep3(btVector3(-1000,-1000,-1000), btVector3(1000,1000,1000));

    //the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    mSolver = new btSequentialImpulseConstraintSolver;

    // Initialize the world
    mDynamicsWorld = new btDiscreteDynamicsWorld(mCollisionDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);

    // Set gravity (have to be a little bit accurate :)
    mDynamicsWorld->setGravity(btVector3(0.0f,-9.81f, 0.0f));

    // ghost collision callback
    mGhostPairCallback = new btGhostPairCallback(); // needed for hit-testing "non-physics" shapes
    mDynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(mGhostPairCallback); // why?

    // create debug drawer
    // debugDraw = new DebugDrawer;

    // attach debug draw
    // dynamicsWorld->setDebugDrawer(debugDraw);
}
