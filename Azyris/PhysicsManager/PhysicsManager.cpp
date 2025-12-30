#include <PhysicsManager/PhysicsManager.h>
#include "PhysicsManager.h"
#include <Utils/Helpers.h>

namespace Az
{
    namespace physx
    {
        void PhysicsManager::InitBulletPhysics(const btVector3 &gravity)
        {
            m_Dispatcher.setCollisionConfiguration(&m_CollisionConfig);
            m_DynamicsWorld = btDiscreteDynamicsWorld(&m_Dispatcher, &m_Broadphase, &m_Solver, &m_CollisionConfig);
            m_DynamicsWorld.setGravity(gravity);
        }
        void PhysicsManager::Update(float dt, uint32_t iterations, float fixedTimestep)
        {
            m_DynamicsWorld.stepSimulation(dt, iterations, fixedTimestep);
        }
        btRigidBody *PhysicsManager::CreateBox(float mass, const glm::vec3 &pos, const glm::vec3 &halfExtents, bool disableDeactivation)
        {
            // Create shape
            btBoxShape *boxShape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
            m_Shapes.push_back(boxShape);

            // Calculate inertia
            btVector3 localInertia(0, 0, 0);
            if (mass != 0.0f)
            {
                boxShape->calculateLocalInertia(mass, localInertia);
            }

            // Create transform
            btTransform startTransform;
            startTransform.setIdentity();
            startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

            // Create motion state
            btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);

            // Create rigid body
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, boxShape, localInertia);
            btRigidBody *body = new btRigidBody(rbInfo);
            if (disableDeactivation)
                body->setActivationState(DISABLE_DEACTIVATION);

            // Add to world
            m_DynamicsWorld.addRigidBody(body);
            m_Bodies.push_back(body);

            return body;
        }
        btRigidBody *PhysicsManager::CreateSphere(float mass, const glm::vec3 &pos, float radius, bool disableDeactivation)
        {
            btSphereShape *sphereShape = new btSphereShape(radius);
            m_Shapes.push_back(sphereShape);

            btVector3 localInertia(0, 0, 0);
            if (mass != 0)
            {
                sphereShape->calculateLocalInertia(mass, localInertia);
            }

            btTransform startTransform;
            startTransform.setIdentity();
            startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

            btMotionState *motionState = new btDefaultMotionState(startTransform);

            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, sphereShape, localInertia);
            btRigidBody *body = new btRigidBody(rbInfo);
            if (disableDeactivation)
                body->setActivationState(DISABLE_DEACTIVATION);

            m_DynamicsWorld.addRigidBody(body);
            m_Bodies.push_back(body);

            return body;
        }
        btRigidBody *PhysicsManager::CreateMeshCollider(const Az::Model &model)
        {
            btTriangleMesh *triangleMesh = new btTriangleMesh();
            for (auto &mesh : model.meshes)
            {
                glm::mat4 model = mesh.modelMatrix; // GLB node transform

                for (size_t i = 0; i < mesh.indices.size(); i += 3)
                {
                    glm::vec3 v0 = glm::vec3(model * glm::vec4(mesh.vertices[mesh.indices[i + 0]].Position, 1.0f));
                    glm::vec3 v1 = glm::vec3(model * glm::vec4(mesh.vertices[mesh.indices[i + 1]].Position, 1.0f));
                    glm::vec3 v2 = glm::vec3(model * glm::vec4(mesh.vertices[mesh.indices[i + 2]].Position, 1.0f));

                    triangleMesh->addTriangle(
                        btVector3(v0.x, v0.y, v0.z),
                        btVector3(v1.x, v1.y, v1.z),
                        btVector3(v2.x, v2.y, v2.z),
                        true);
                }
            }
            btBvhTriangleMeshShape *meshShape = new btBvhTriangleMeshShape(triangleMesh, true);
            btDefaultMotionState *meshMotionState =
                new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
            btRigidBody::btRigidBodyConstructionInfo meshCI(0.0f, meshMotionState, meshShape, btVector3(0, 0, 0));
            btRigidBody *meshRigidBody = new btRigidBody(meshCI);

            m_DynamicsWorld.addRigidBody(meshRigidBody);
            m_Bodies.push_back(meshRigidBody);
            m_Shapes.push_back(meshShape);
            return meshRigidBody;
        }

        btRigidBody *PhysicsManager::CreateCapsule(float mass, const glm::vec3 &pos, float radius, float height, bool disableDeactivation, int upAxis)
        {
            // Create capsule shape
            btCapsuleShape *capsuleShape = nullptr;

            switch (upAxis)
            {
            case 0: // X-axis
                capsuleShape = new btCapsuleShapeX(radius, height);
                break;
            case 1: // Y-axis (default/vertical)
                capsuleShape = new btCapsuleShape(radius, height);
                break;
            case 2: // Z-axis
                capsuleShape = new btCapsuleShapeZ(radius, height);
                break;
            default:
                // Default to Y-axis if invalid
                capsuleShape = new btCapsuleShape(radius, height);
                break;
            }

            m_Shapes.push_back(capsuleShape);

            // Calculate inertia
            btVector3 localInertia(0, 0, 0);
            if (mass != 0.0f)
            {
                capsuleShape->calculateLocalInertia(mass, localInertia);
            }

            // Create transform
            btTransform startTransform;
            startTransform.setIdentity();
            startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

            // Create motion state
            btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);

            // Create rigid body
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, capsuleShape, localInertia);
            btRigidBody *body = new btRigidBody(rbInfo);

            if (disableDeactivation)
                body->setActivationState(DISABLE_DEACTIVATION);

            // Set additional properties for capsules
            body->setSleepingThresholds(0.1f, 0.1f);             // Better sleep thresholds for capsules
            body->setAngularFactor(btVector3(1.0f, 0.0f, 1.0f)); // Prevent rolling sideways

            // Add to world
            m_DynamicsWorld.addRigidBody(body);
            m_Bodies.push_back(body);

            return body;
        }

        btRigidBody *PhysicsManager::CreateKinematicMeshCollider(Az::Mesh *mesh, void *userPtr, bool applyScaleAndPivot)
        {
            // Get local AABB
            glm::vec3 halfExtents = mesh->GetLocalHalfExtents();
            glm::vec3 center = mesh->GetCenter();

            if (applyScaleAndPivot)
            {
                // Apply mesh model matrix scale
                glm::vec3 scale;
                scale.x = glm::length(mesh->modelMatrix[0]);
                scale.y = glm::length(mesh->modelMatrix[1]);
                scale.z = glm::length(mesh->modelMatrix[2]);
                halfExtents *= scale;

                // Compute pivot offset
                glm::vec4 meshCenter = mesh->modelMatrix * glm::vec4(center, 1.0f);
                center = glm::vec3(meshCenter);
            }

            // Create Bullet box shape
            btBoxShape *shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));

            // Compute world transform from final center
            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(btVector3(center.x, center.y, center.z));

            btDefaultMotionState *motion = new btDefaultMotionState(transform);
            btRigidBody::btRigidBodyConstructionInfo ci(0.0f, motion, shape, btVector3(0, 0, 0));
            btRigidBody *body = new btRigidBody(ci);
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            body->setActivationState(DISABLE_DEACTIVATION);

            body->setUserPointer(userPtr);
            m_DynamicsWorld.addRigidBody(body);
            m_Bodies.push_back(body);
            m_Shapes.push_back(shape);

            return body;
        }

        btRigidBody *PhysicsManager::CreateConvexHullBody(Az::Mesh *mesh, float mass, bool disableDeactivation)
        {
            btConvexHullShape *hull = new btConvexHullShape();
            for (const auto &vertex : mesh->vertices)
            {
                hull->addPoint(Az::ConvertGLMVec3(vertex.Position));
            }
            hull->optimizeConvexHull();
            hull->initializePolyhedralFeatures();

            btVector3 inertia(0, 0, 0);
            if (mass != 0.0f)
                hull->calculateLocalInertia(mass, inertia);

            btDefaultMotionState *motionState = new btDefaultMotionState(btTransform::getIdentity());
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, hull, inertia);
            btRigidBody *body = new btRigidBody(rbInfo);

            if (disableDeactivation)
                body->setActivationState(DISABLE_DEACTIVATION);

            m_DynamicsWorld.addRigidBody(body);

            m_Bodies.push_back(body);
            m_Shapes.push_back(hull);

            return body;
        }

        std::vector<btRigidBody *> *PhysicsManager::GetRigidbodies()
        {
            return &m_Bodies;
        }

        std::vector<btCollisionShape *> *PhysicsManager::GetShapes()
        {
            return &m_Shapes;
        }

        void PhysicsManager::AddRigidBody(btRigidBody *body, int group, int mask)
        {
            m_DynamicsWorld.addRigidBody(body, group, mask);
            m_Bodies.push_back(body);
        }

        void PhysicsManager::RemoveRigidBody(btRigidBody *body)
        {
            m_DynamicsWorld.removeRigidBody(body);

            auto it = std::find(m_Bodies.begin(), m_Bodies.end(), body);
            if (it != m_Bodies.end())
            {
                m_Bodies.erase(it);
            }
        }

        void PhysicsManager::PushBackBody(btRigidBody *body)
        {
            m_Bodies.push_back(body);
        }

        void PhysicsManager::PushBackShape(btCollisionShape *shape)
        {
            m_Shapes.push_back(shape);
        }

        void PhysicsManager::Cleanup()
        {
            // Remove all bodies from world
            for (auto *body : m_Bodies)
            {
                m_DynamicsWorld.removeRigidBody(body);

                // Cleanup body
                if (body && body->getMotionState())
                {
                    delete body->getMotionState();
                }
                delete body;
            }
            m_Bodies.clear();

            // Cleanup shapes
            for (auto *shape : m_Shapes)
            {
                delete shape;
            }
            m_Shapes.clear();
        }

        btRigidBody *CreateKinematicMeshCollider(btDiscreteDynamicsWorld *world, Az::Mesh *mesh, void *userPtr, bool applyScaleAndPivot)
        {
            // Get local AABB
            glm::vec3 halfExtents = mesh->GetLocalHalfExtents();
            glm::vec3 center = mesh->GetCenter();

            if (applyScaleAndPivot)
            {
                // Apply mesh model matrix scale
                glm::vec3 scale;
                scale.x = glm::length(mesh->modelMatrix[0]);
                scale.y = glm::length(mesh->modelMatrix[1]);
                scale.z = glm::length(mesh->modelMatrix[2]);
                halfExtents *= scale;

                // Compute pivot offset
                glm::vec4 meshCenter = mesh->modelMatrix * glm::vec4(center, 1.0f);
                center = glm::vec3(meshCenter);
            }

            // Create Bullet box shape
            btBoxShape *shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));

            // Compute world transform from final center
            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(btVector3(center.x, center.y, center.z));

            btDefaultMotionState *motion = new btDefaultMotionState(transform);
            btRigidBody::btRigidBodyConstructionInfo ci(0.0f, motion, shape, btVector3(0, 0, 0));
            btRigidBody *body = new btRigidBody(ci);
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            body->setActivationState(DISABLE_DEACTIVATION);

            body->setUserPointer(userPtr);
            world->addRigidBody(body);
            return body;
        }
    }
}
