#include <PickableProp.h>
#include <Utils/Timer.h>
#include <core/Input.h>

namespace Crd
{
    namespace Object
    {
        Az::physx::PhysicsManager *Prop::s_PhysicsManager = nullptr;

        void Prop::SetPhysicsManagerPtr(Az::physx::PhysicsManager *manager)
        {
            s_PhysicsManager = manager;
        }

        void Prop::SetMesh(Az::Mesh *mesh)
        {
            m_Mesh = mesh;
            m_OriginalModel = mesh->modelMatrix;
        }

        void Prop::SetRigidBody(btRigidBody *body)
        {
            m_RigidBody = body;
            if (!body)
                std::cout << "RigidBody is nullptr" << std::endl;
        }

        bool Prop::Init()
        {
            if (!s_PhysicsManager)
            {
                std::cout << "Physics Manager is nullptr" << std::endl;
                return true;
            }
            if (!m_Mesh || !m_RigidBody)
            {
                std::cout << "Prop failed to initialize" << std::endl;
                return true;
            }

            btTransform transform;
            m_RigidBody->getMotionState()->getWorldTransform(transform);
            m_Position = Az::ConvertBTVec3(transform.getOrigin());
            glm::quat quaternion = Az::ConvertBTQuat(transform.getRotation());
            m_OverrideModel = glm::translate(glm::mat4(1.0f), m_Position) * glm::mat4_cast(quaternion) * m_OriginalModel;

            return false;
        }

        void Prop::PickUp(const glm::vec3 &playerPos, const glm::quat &playerRot)
        {

            if (!m_RigidBody)
                return;

            m_PickedUp = !m_PickedUp;

            if (m_PickedUp)
            {
                // Picking up
                m_RigidBody->setGravity(btVector3(0, 0, 0));
                m_RigidBody->setActivationState(ACTIVE_TAG);

                btVector3 currentPos = m_RigidBody->getWorldTransform().getOrigin();
                btVector3 localPivot = m_RigidBody->getCenterOfMassTransform().inverse() * currentPos;
                m_GrabConstraint = new btPoint2PointConstraint(*m_RigidBody, localPivot);
                m_GrabConstraint->m_setting.m_tau = 0.05f;
                m_GrabConstraint->m_setting.m_impulseClamp = 10.0f;
                s_PhysicsManager->GetWorld()->addConstraint(m_GrabConstraint);

                // Compute offset relative to player
                glm::quat propRot = Az::ConvertBTQuat(m_RigidBody->getWorldTransform().getRotation());
                m_HeldRotationOffset = glm::inverse(playerRot) * propRot;
            }
            else
            {
                // Dropping
                m_RigidBody->setGravity(btVector3(0, -9.81f, 0));

                if (m_GrabConstraint)
                {
                    s_PhysicsManager->GetWorld()->removeConstraint(m_GrabConstraint);
                    delete m_GrabConstraint;
                    m_GrabConstraint = nullptr;
                }

                // KEEP current world rotation as-is
                btTransform trans = m_RigidBody->getWorldTransform();
                // no multiplication by playerRot or anything
                trans.setRotation(trans.getRotation());
                m_RigidBody->setWorldTransform(trans);
                m_RigidBody->getMotionState()->setWorldTransform(trans);
            }
        }

        void Prop::Move(const glm::vec3 &playerPos, const glm::vec3 &forwardVector, const glm::quat &playerRot)
        {
            if (!m_RigidBody || !m_PickedUp || !m_GrabConstraint)
                return;

            // Move smoothly
            glm::vec3 targetPos = playerPos + forwardVector;
            btVector3 currentPivot = m_GrabConstraint->getPivotInB();
            btVector3 targetPivot = Az::ConvertGLMVec3(targetPos);
            m_GrabConstraint->setPivotB(currentPivot.lerp(targetPivot, 0.2f));

            // Compute look-at rotation towards player
            glm::vec3 dirToPlayer = glm::normalize(playerPos - m_Position);
            glm::quat lookAtPlayer = glm::quatLookAt(-dirToPlayer, glm::vec3(0, 1, 0));

            // Apply manual rotation offset on top
            glm::quat finalRot = lookAtPlayer * m_HeldRotationOffset;

            // Apply rotation to physics body
            btTransform trans = m_RigidBody->getWorldTransform();
            trans.setRotation(Az::ConvertGLMQuat(finalRot));
            m_RigidBody->setWorldTransform(trans);
            m_RigidBody->getMotionState()->setWorldTransform(trans);
        }

        void Prop::Rotate()
        {
            if (!m_PickedUp)
                return;

            glm::vec2 mouseDelta = Az::Input::GetMouseDelta();
            float sensitivity = 0.01f;

            glm::quat yaw = glm::angleAxis(-mouseDelta.x * sensitivity, glm::vec3(0, 1, 0));
            glm::quat pitch = glm::angleAxis(-mouseDelta.y * sensitivity, glm::vec3(1, 0, 0));

            // Modify rotation offset
            m_HeldRotationOffset = glm::normalize(yaw * pitch * m_HeldRotationOffset);
        }

        glm::mat4 *Prop::GetModelMatrix()
        {
            btTransform trans;
            m_RigidBody->getMotionState()->getWorldTransform(trans);
            m_Position = Az::ConvertBTVec3(trans.getOrigin());
            glm::quat physicsRot = Az::ConvertBTQuat(trans.getRotation());

            m_OverrideModel = glm::translate(glm::mat4(1.0f), m_Position) * glm::mat4_cast(physicsRot);
            return &m_OverrideModel;
        }
    }
}
