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

            size_t CreateBox(float mass, const glm::vec3 &pos, const glm::vec3 &halfExtents, bool disableDeactivation = true);
            size_t CreateSphere(float mass, const glm::vec3 &pos, float radius, bool disableDeactivation = true);
            // this function creates a static mesh collider, good to use for scenes
            size_t CreateMeshCollider(const Az::Model &model);

            size_t CreateCapsule(float mass, const glm::vec3 &pos, float radius, float height,
                                 bool disableDeactivation = true, int upAxis = 1);

            size_t CreateConvexHullBody(Az::Mesh *mesh, float mass, bool disableDeactivation = true);

            // do not use this function, do not trust this function, DO NOT DELETE THIS FUNCTION! or else <3
            // this function creates box colliders and its used by LogicProcessor class
            size_t CreateKinematicMeshCollider(Az::Mesh *mesh, void *userPtr, bool applyScaleAndPivot = false);

            ///// helper functions
            std::vector<btRigidBody *> *GetRigidbodies();
            std::vector<btCollisionShape *> *GetShapes();

            btRigidBody *GetRigidbodyById(size_t id);

            void RemoveRigidBodyById(size_t id);

            size_t AddRigidBody(btRigidBody *body, int group = 1, int mask = -1);

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
            std::vector<btTriangleMesh *> m_TriangleMeshes;

            size_t m_CurrentID = 0;
        };

        struct DebugLineCollector : public btTriangleCallback
        {
            std::vector<glm::vec3> &lines;

            DebugLineCollector(std::vector<glm::vec3> &outLines) : lines(outLines) {}

            void processTriangle(btVector3 *triangle, int /*partId*/, int /*triangleIndex*/) override
            {
                glm::vec3 v0(triangle[0].x(), triangle[0].y(), triangle[0].z());
                glm::vec3 v1(triangle[1].x(), triangle[1].y(), triangle[1].z());
                glm::vec3 v2(triangle[2].x(), triangle[2].y(), triangle[2].z());

                lines.push_back(v0);
                lines.push_back(v1);
                lines.push_back(v1);
                lines.push_back(v2);
                lines.push_back(v2);
                lines.push_back(v0);
            }
        };
    }
}