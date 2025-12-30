#pragma once

#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <Model/Model.h>

namespace Az
{
    namespace physx
    {
        class PhysicsManager
        {
        public:
            PhysicsManager()
                : m_CollisionConfig(), m_Dispatcher(&m_CollisionConfig),
                  m_Broadphase(), m_Solver(), m_DynamicsWorld(&m_Dispatcher, &m_Broadphase, &m_Solver, &m_CollisionConfig)
            {
                m_DynamicsWorld.setGravity(btVector3(0, -9.81f, 0));
            }
            ~PhysicsManager() = default;

            void InitBulletPhysics(const btVector3 &gravity = btVector3(0, -9.81f, 0));
            void Update(float dt = (1.0f / 60.0f), uint32_t iterations = 4, float fixedTimestep = (1.0f / 60.0f));

            btDiscreteDynamicsWorld *GetWorld() { return &m_DynamicsWorld; }

            ///////--------------------------------------------------------/////////////////////
            ///////------------      Rigid body creation        -----------/////////////////////
            ///////--------------------------------------------------------/////////////////////

            btRigidBody *CreateBox(float mass, const glm::vec3 &pos, const glm::vec3 &halfExtents, bool disableDeactivation = true);
            btRigidBody *CreateSphere(float mass, const glm::vec3 &pos, float radius, bool disableDeactivation = true);
            // this function creates a static mesh collider, good to use for scenes
            btRigidBody *CreateMeshCollider(const Az::Model &model);

            btRigidBody *CreateCapsule(float mass, const glm::vec3 &pos, float radius, float height,
                                       bool disableDeactivation = true, int upAxis = 1);

            // do not use this function, do not trust this function, DO NOT DELETE THIS FUNCTION! or else <3
            // this function creates box colliders and its used by LogicProcessor class
            btRigidBody *CreateKinematicMeshCollider(Az::Mesh *mesh, void *userPtr, bool applyScaleAndPivot = false);

            btRigidBody *CreateConvexHullBody(Az::Mesh *mesh, float mass, bool disableDeactivation = true);

            ///// helper functions
            std::vector<btRigidBody *> *GetRigidbodies();
            std::vector<btCollisionShape *> *GetShapes();

            void AddRigidBody(btRigidBody *body, int group = 1, int mask = -1);
            void RemoveRigidBody(btRigidBody *body);

            // use this function on your own risk
            void PushBackBody(btRigidBody *body);
            // use this function on your own risk
            void PushBackShape(btCollisionShape *shape);

            void Cleanup();

        private: // ----------------- Bullet Setup --------------------------
            btDefaultCollisionConfiguration m_CollisionConfig;
            btCollisionDispatcher m_Dispatcher;
            btDbvtBroadphase m_Broadphase;
            btSequentialImpulseConstraintSolver m_Solver;
            btDiscreteDynamicsWorld m_DynamicsWorld;

        private:
            // --- managed objects ---
            std::vector<btRigidBody *> m_Bodies;
            std::vector<btCollisionShape *> m_Shapes;
        };

        btRigidBody *CreateKinematicMeshCollider(btDiscreteDynamicsWorld *world,
                                                 Az::Mesh *mesh, void *userPtr, bool applyScaleAndPivot = false);
    }
}