#include <PickableProp.h>
#include <Utils/Timer.h>

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
            if (body == nullptr)
                std::cout << "RigidBody is nullptr" << std::endl;
        }

        bool Prop::Init()
        {
            if (s_PhysicsManager == nullptr)
            {
                std::cout << "Physics Manager is nullptr" << std::endl;
                return true;
            }
            if (m_Mesh == nullptr || m_RigidBody == nullptr)
            {
                std::cout << "Prop failed to initialize" << std::endl;
                return true;
            }

            btTransform transform;
            m_RigidBody->getMotionState()->getWorldTransform(transform);
            m_Position = Az::ConvertBTVec3(transform.getOrigin());
            glm::quat quaternion = Az::ConvertBTQuat(transform.getRotation());
            glm::mat4 rotMat = glm::mat4_cast(quaternion);
            m_OverrideModel = glm::translate(glm::mat4(1.0f), m_Position) * rotMat * m_OriginalModel;
            return false;
        }

        void Prop::PickUp(const glm::vec3 &grabPoint)
        {
            if (!m_RigidBody)
                return;

            m_PickedUp = !m_PickedUp;

            if (m_PickedUp)
            {
                m_RigidBody->setGravity(btVector3(0, 0, 0));
                m_RigidBody->setActivationState(ACTIVE_TAG);

                // Create constraint at current object position
                btVector3 currentPos = m_RigidBody->getWorldTransform().getOrigin();
                btVector3 localPivot = m_RigidBody->getCenterOfMassTransform().inverse() * currentPos;

                m_GrabConstraint = new btPoint2PointConstraint(*m_RigidBody, localPivot);

                // Soft constraint settings
                m_GrabConstraint->m_setting.m_tau = 0.05f;          // softer spring
                m_GrabConstraint->m_setting.m_impulseClamp = 10.0f; // limit max force

                s_PhysicsManager->GetWorld()->addConstraint(m_GrabConstraint);
            }
            else
            {

                m_RigidBody->setGravity(btVector3(0, -9.81f, 0));
                if (m_GrabConstraint)
                {
                    s_PhysicsManager->GetWorld()->removeConstraint(m_GrabConstraint);
                    delete m_GrabConstraint;
                    m_GrabConstraint = nullptr;
                }
            }
        }

        // position param is player position;
        void Prop::Move(const glm::vec3 &playerPos, const glm::vec3 &forwardVector, const glm::quat &playerRot)
        {
            if (!m_RigidBody || !m_PickedUp)
                return;

            // Compute target position in front of the player
            glm::vec3 targetPos = playerPos + forwardVector;

            // Smoothly move the pivot
            btVector3 currentPivot = m_GrabConstraint->getPivotInB();
            btVector3 targetPivot = Az::ConvertGLMVec3(targetPos);
            float alpha = 0.2f;
            btVector3 newPivot = currentPivot.lerp(targetPivot, alpha);
            m_GrabConstraint->setPivotB(newPivot);

            // Rotate smoothly to face the player
            btTransform trans;
            m_RigidBody->getMotionState()->getWorldTransform(trans);

            glm::quat currentRot = Az::ConvertBTQuat(trans.getRotation());
            glm::quat targetRot = glm::quatLookAt(glm::normalize(playerPos - m_Position), glm::vec3(0, 1, 0));

            // Compute relative rotation
            glm::quat deltaRot = targetRot * glm::inverse(currentRot);

            // Extract axis and angle
            deltaRot = glm::normalize(deltaRot); // make sure quaternion is normalized
            glm::vec3 axis = glm::axis(deltaRot);
            float angle = glm::angle(deltaRot);

            // Angular velocity: axis * angle * speed factor
            float rotationSpeed = 5.0f; // tweak for smoothness
            btVector3 angVel = Az::ConvertGLMVec3(axis * angle * rotationSpeed);
            m_RigidBody->setAngularVelocity(angVel);
        }

        void Prop::Rotate(glm::vec3 *pivot)
        {
        }

        glm::mat4 *Prop::GetModelMatrix()
        {
            btTransform transform;
            m_RigidBody->getMotionState()->getWorldTransform(transform);
            m_Position = Az::ConvertBTVec3(transform.getOrigin());
            glm::quat quaternion = Az::ConvertBTQuat(transform.getRotation());
            glm::mat4 rotMat = glm::mat4_cast(quaternion);
            m_OverrideModel = glm::translate(glm::mat4(1.0f), m_Position) * rotMat;

            return &m_OverrideModel;
        }
    }
}