#include "spatialindex3d.h"

SpatialIndex3d::SpatialIndex3d()
{
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mCollisionDispatcher    = new btCollisionDispatcher(mCollisionConfiguration);
    mBroadPhase             = new btDbvtBroadphase();
    mCollisionWorld         = new btCollisionWorld(mCollisionDispatcher, mBroadPhase, mCollisionConfiguration);

}

SpatialIndex3d::~SpatialIndex3d()
{
    for (int i=mCollisionWorld->getNumCollisionObjects()-1; i>=0 ;i--)
    {
        btCollisionObject* obj = mCollisionWorld->getCollisionObjectArray()[i];
        mCollisionWorld->removeCollisionObject( obj );
        delete obj;
    }

    //delete collision shapes
    for (int j=0;j<mCollisionShapes.size();j++)
    {
        btCollisionShape* shape = mCollisionShapes[j];
        mCollisionShapes[j] = 0;
        delete shape;
    }

    delete mCollisionWorld;

    delete mBroadPhase;

    delete mCollisionDispatcher;

    delete mCollisionConfiguration;

    mCollisionShapes.clear();
}
