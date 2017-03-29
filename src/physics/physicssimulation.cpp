#include "physicssimulation.h"

#include "extmotionstate.h"
#include "testmotionstate.h"
#include "../common/macro/debuglog.h"

PhysicsSimulation::PhysicsSimulation(Ptr::WritePtr<RigidBodyPool> actor_rigid_bodies_ptr,
                                     Ptr::WritePtr<RigidBodyPool> static_rigid_bodies_ptr) :
    mActorRigidBodiesPtr(actor_rigid_bodies_ptr),
    mStaticRigidBodiesPtr(static_rigid_bodies_ptr)
{
    DEBUG_LOG("Creating physics simulation")

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
    // change this later...

    // ghost collision callback
    mGhostPairCallback = new btGhostPairCallback(); // needed for hit-testing "non-physics" shapes
    mDynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(mGhostPairCallback); // why?

    // create debug drawer
    // debugDraw = new DebugDrawer;

    // attach debug draw
    // dynamicsWorld->setDebugDrawer(debugDraw);
}

void PhysicsSimulation::setGravity(const vmath::Vector3 &g)
{
    mDynamicsWorld->setGravity(btVector3(g.getX(), g.getY(), g.getZ()));
}

btCollisionShape * PhysicsSimulation::createMeshShape(const std::vector<vmath::Vector4> &pts, const std::vector<gfx::Triangle> &tris)
{
    mStaticMeshData.push_back(btTriangleMesh());
    btTriangleMesh * triangle_mesh = &(mStaticMeshData.back());
    for (int i=0; i<tris.size(); i++)
    {
        const vmath::Vector4 &p0 = pts[tris[i][0]];
        const vmath::Vector4 &p1 = pts[tris[i][1]];
        const vmath::Vector4 &p2 = pts[tris[i][2]];
        triangle_mesh->addTriangle(btVector4(p0.getX(), p0.getY(), p0.getZ(), 1.0f),
                                   btVector4(p1.getX(), p1.getY(), p1.getZ(), 1.0f),
                                   btVector4(p2.getX(), p2.getY(), p2.getZ(), 1.0f));
    }

    // why the fuark does this not work...

    btBvhTriangleMeshShape* col_shape = new btBvhTriangleMeshShape(triangle_mesh, true);
    col_shape->buildOptimizedBvh();

    return col_shape;
}

void PhysicsSimulation::addDynamicBody(const vmath::Vector3 &pos, const vmath::Quat &rot, Shape shape)
{
    // could use a pool allocator for all these 'new's
    // btCollisionShape* col_shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)); // set as input later...
    btCollisionShape* col_shape = nullptr;
    if (shape == Shape::Box)
    {
        col_shape = (btCollisionShape*)(new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)));

        //Procedural::Geometry box_geom = Procedural::boxPlanes(1.0f, 1.0f, 1.0f);
        //col_shape = createMeshShape(box_geom.points, box_geom.triangles);
    }
    else
    {
        col_shape = (btCollisionShape*)(new btSphereShape(2.0f));
    }


    mCollisionShapes.push_back(col_shape);

    // Create Dynamic Objects
    btTransform start_transform( btQuaternion(rot.getX(), rot.getY(), rot.getZ(), rot.getW()),
                                    btVector3(pos.getX(), pos.getY(), pos.getZ()));

    btScalar	mass(70.f); // change for input later... // 70 kg?

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool is_dynamic = (mass != 0.f);

    btVector3 local_inertia(0,0,0);
    if (is_dynamic)
    {
        col_shape->calculateLocalInertia(mass, local_inertia);
    }

    btMotionState* motion_state = new ExtMotionState(start_transform);

    DEBUG_LOG("ADDING RIGIDBODY");

    btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state, col_shape, local_inertia);
    rb_info.m_restitution = btScalar(0.33f); // wtf are these...
    rb_info.m_friction = btScalar(0.4f);     // set later..
    //rb_info.m_rollingFriction = btScalar(0.4f);
    DEBUG_LOG("rb_info.m_rollingFriction: " << rb_info.m_rollingFriction);


    RigidBodyPoolHandle rb_handle = mActorRigidBodiesPtr->create(rb_info);
    btRigidBody* body = rb_handle->get_ptr();
    //btRigidBody* body = new btRigidBody(rb_info);
    body->activate(); // try to force activate so that it doesn't just hang in the air...

    mDynamicsWorld->addRigidBody(body);

    body->setSleepingThresholds(0.2f, 0.25f);

    // give it some starting velocity to not hang in the air...
    //body->applyCentralForce(btVector3(0.1, 0.1, 0.1));
}


void PhysicsSimulation::addStaticBodyMesh(const vmath::Vector3 &pos, const vmath::Quat &rot,
                                          const std::vector<vmath::Vector3> &points, const std::vector<gfx::Triangle> &triangles)
{
    // could use a pool allocator for all these 'new's
    // LIFETIME: Points and triangles need to live longer than this collision-shape!!!
    // aiai... bullet is either not const-correct or wanting to reorganize my triangles...
    // therefore make a copy for the physics simulation to play with...
    mStaticMeshData.push_back(btTriangleMesh());
    btTriangleMesh * triangle_mesh = &(mStaticMeshData.back());
    for (int i=0; i<triangles.size(); i++)
    {
        const vmath::Vector3 &p0 = points[triangles[i][0]];
        const vmath::Vector3 &p1 = points[triangles[i][1]];
        const vmath::Vector3 &p2 = points[triangles[i][2]];
        triangle_mesh->addTriangle(btVector3(p0.getX(), p0.getY(), p0.getZ()),
                                   btVector3(p1.getX(), p1.getY(), p1.getZ()),
                                   btVector3(p2.getX(), p2.getY(), p2.getZ()));
    }

    // why the fuark does this not work...

    btBvhTriangleMeshShape* col_shape = new btBvhTriangleMeshShape(triangle_mesh, true);
    col_shape->buildOptimizedBvh();
    //btConvexTriangleMeshShape * col_shape = new btConvexTriangleMeshShape (triangle_mesh);
    mCollisionShapes.push_back(col_shape);

    btTransform transform( btQuaternion(rot.getX(), rot.getY(), rot.getZ(), rot.getW()),
                              btVector3(pos.getX(), pos.getY(), pos.getZ()));

    btScalar	mass(0.f); // zero mass = static...
    btVector3   local_inertia(0.0f, 0.0f, 0.0f);

    btMotionState* motion_state = new TestMotionState(transform);

    DEBUG_LOG("ADDING STATIC BODY");

    btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state, col_shape, local_inertia);
    rb_info.m_restitution = btScalar(0.33f); // wtf are these...
    rb_info.m_friction = btScalar(0.4f);     // set later..

    RigidBodyPoolHandle rb_handle = mStaticRigidBodiesPtr->create(rb_info);
    btRigidBody* body = rb_handle->get_ptr();

    mDynamicsWorld->addRigidBody(body);
}

void PhysicsSimulation::addStaticBodyPlane(const vmath::Vector3 &pos, const vmath::Quat &rot,
                                           const vmath::Vector3 &normal)
{
    btStaticPlaneShape *col_shape = new btStaticPlaneShape(btVector3(normal.getX(), normal.getY(), normal.getZ()), 0.0f);
    mCollisionShapes.push_back(col_shape);

    btTransform transform( btQuaternion(rot.getX(), rot.getY(), rot.getZ(), rot.getW()),
                              btVector3(pos.getX(), pos.getY(), pos.getZ()));

    btScalar	mass(0.f); // zero mass = static...
    btVector3   local_inertia(0.0f, 0.0f, 0.0f);

    btMotionState* motion_state = new TestMotionState(transform);

    DEBUG_LOG("ADDING STATIC PLANE");

    btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state, col_shape, local_inertia);
    rb_info.m_restitution = btScalar(0.33f); // wtf are these...
    rb_info.m_friction = btScalar(0.4f);     // set later..

    RigidBodyPoolHandle rb_handle = mStaticRigidBodiesPtr->create(rb_info);
    btRigidBody* body = rb_handle->get_ptr();

    mDynamicsWorld->addRigidBody(body);
}


/*
// void PhysicsWorld::addPhysicsObject(RigidBody::Collision shape, par1=0, par2=0, par3=0, par4=0)
void PhysicsSubsystem::addPhysicsDynamic(RigidBody* rigidbody, btCollisionShape* shape)
{
    //create a dynamic rigidbody

    //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
    // btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    btCollisionShape* colShape = shape;
    collisionShapes.push_back(colShape);

    // Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar	mass(10.f);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass,localInertia);

    // Set starting rotation
    btQuaternion bt_start_quat = btQuaternion(rigidbody->rot.w, rigidbody->rot.x,
                                              rigidbody->rot.y, rigidbody->rot.z);
    startTransform.setRotation( bt_start_quat ) ;

    // Set starting translation
    startTransform.setOrigin(btVector3(rigidbody->pos.x, rigidbody->pos.y, rigidbody->pos.z));

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
    rbInfo.m_restitution = btScalar(0.33f);
    rbInfo.m_friction = btScalar(0.4f);
    btRigidBody* body = new btRigidBody(rbInfo);

    rigidbody->setBody(body);

    dynamicsWorld->addRigidBody(body);
}

void PhysicsWorld::addPhysicsStatic(RigidBody* rigidbody, btCollisionShape* shape)
{
    //create a dynamic rigidbody

    //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
    // I think this puts the plane at -1 y?
    btCollisionShape* colShape = shape;
    shape->setLocalScaling(btVector3(rigidbody->scale.x, rigidbody->scale.y, rigidbody->scale.z) );
    collisionShapes.push_back(colShape);

    // Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    // Set scale


    btScalar	mass(0.f);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass,localInertia);

   // Set starting rotation
    btQuaternion bt_start_quat = btQuaternion(rigidbody->rot.w, rigidbody->rot.x,
                                              rigidbody->rot.y, rigidbody->rot.z);
    startTransform.setRotation( bt_start_quat ) ;

    // Set starting translation
    startTransform.setOrigin(btVector3(rigidbody->pos.x, rigidbody->pos.y, rigidbody->pos.z));



    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
//    rbInfo.m_restitution = btScalar(0.99f);
//    rbInfo.m_friction = btScalar(0.99f);
    rbInfo.m_restitution = btScalar(0.33f);
    rbInfo.m_friction = btScalar(0.4f);
    btRigidBody* body = new btRigidBody(rbInfo);

    rigidbody->setBody(body);

    dynamicsWorld->addRigidBody(body);
}
*/

PhysicsSimulation::~PhysicsSimulation()
{
    for (int i=mDynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
    {
        btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        mDynamicsWorld->removeCollisionObject( obj );
        //delete obj; // gets cleared up by owner of RigidBodyPools
    }

    //delete collision shapes
    for (int j=0;j<mCollisionShapes.size();j++)
    {
        btCollisionShape* shape = mCollisionShapes[j];
        mCollisionShapes[j] = 0;
        delete shape;
    }

    //delete debugDraw;

    delete mGhostPairCallback;

    delete mDynamicsWorld;

    delete mSolver;

    delete mOverlappingPairCache;

    delete mCollisionDispatcher;

    // this does not need delete? -- it does
    delete mCollisionConfiguration;

    mCollisionShapes.clear();
}
