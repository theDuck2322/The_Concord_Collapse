#include "mdControllers.h"
#include <Timer.h>

namespace Crd
{
    namespace MdLogic
    {

        // ---------------------------- Button class logic -----------------------------------
        void Button::SetActive(bool val)
        {
            m_IsActive = val;
            if (val)
                m_Object->Activate();
            else
                m_Object->Deactivate();
        }

        void Button::SetMesh(Az::Mesh *mesh)
        {
            m_Mesh = mesh;
        }
        Az::Mesh *Button::GetMesh()
        {
            return m_Mesh;
        }
        void Button::SetOnDuration(float duration)
        {
            m_ON_Duration = duration;
        }
        void Button::Update(bool check)
        {

            if (IsActive() && m_CurrentTime < m_ON_Duration) // still active
            {
                m_CurrentTime += Az::Timer::fDeltaTime;
                m_Object->Activate();
            }
            else
            {
                m_Object->Deactivate();
                SetActive(false);
                m_CurrentTime = 0;
            }

            m_Object->Update();
        }

        void Button::SetRigidBody(btRigidBody *body)
        {
            m_RigidBody = body;
            if (!m_RigidBody || !m_Mesh)
                return;

            // --- Extract scale from original mesh ---
            glm::vec3 scale;
            scale.x = glm::length(m_Mesh->modelMatrix[0]);
            scale.y = glm::length(m_Mesh->modelMatrix[1]);
            scale.z = glm::length(m_Mesh->modelMatrix[2]);

            // --- Build unscaled model matrix ---
            glm::mat4 modelNoScale = m_Mesh->modelMatrix;
            modelNoScale[0] /= scale.x;
            modelNoScale[1] /= scale.y;
            modelNoScale[2] /= scale.z;

            // --- Optional: local rotation/pivot for buttons can go here ---
            // For a simple button/switch, we don't rotate, so identity
            glm::mat4 localTransform = glm::mat4(1.0f);

            // --- Reapply scale LAST ---
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

            // --- Final model matrix for the mesh ---
            m_Mesh->modelMatrix = modelNoScale * localTransform * scaleMat;

            // --- Update Bullet kinematic body ---
            btBoxShape *box = static_cast<btBoxShape *>(m_RigidBody->getCollisionShape());
            if (box)
                box->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

            btTransform t;
            t.setFromOpenGLMatrix(glm::value_ptr(modelNoScale * localTransform));
            m_RigidBody->getMotionState()->setWorldTransform(t);
            m_RigidBody->setWorldTransform(t);
        }
        // ---------------------------- Switch class logic -----------------------------------
        void Switch::Update(bool check)
        {
            if (!m_Object)
                return;

            if (m_IsActive)
            {
                m_Object->Activate();
            }
            else
            {
                m_Object->Deactivate();
            }
            if (!m_RigidBody || !m_Mesh)
                return;

            m_Object->Update();
        }

        void SimpleDoor::SetMesh(Az::Mesh *mesh)
        {
            m_Mesh = mesh;
            m_OriginalModel = mesh->modelMatrix;
            Update(false);
        }

        Az::Mesh *SimpleDoor::GetMesh()
        {
            return m_Mesh;
        }

        void SimpleDoor::Animate(float angle)
        {

            if (!m_Mesh)
                return;

            // --- Extract scale from original model ---
            glm::vec3 scale(
                glm::length(m_OriginalModel[0]),
                glm::length(m_OriginalModel[1]),
                glm::length(m_OriginalModel[2]));

            // --- Remove scale to work with rotation/translation only ---
            glm::mat4 modelNoScale = m_OriginalModel;
            modelNoScale[0] /= scale.x;
            modelNoScale[1] /= scale.y;
            modelNoScale[2] /= scale.z;

            // --- Local AABB of mesh ---
            glm::vec3 min = m_Mesh->GetLocalAABBMin();
            glm::vec3 max = m_Mesh->GetLocalAABBMax();

            // --- Hinge position (customizable) ---
            // Example: center along X, bottom along Y, slightly inward along Z
            float hingeXOffset = 0.0f; // adjust to move hinge along width
            bool hingeOnFront = true;  // hinge on front/back edge

            glm::vec3 hingeLocal(
                (min.x + max.x) * 0.5f + hingeXOffset,       // X offset from center
                min.y,                                       // bottom of door
                hingeOnFront ? (min.z - 0.5) : (max.z + 0.5) // front/back edge
            );

            // --- Rotation axis (door upright) ---
            glm::vec3 axis(0.0f, 1.0f, 0.0f); // Y-up

            // --- Signed angle if you want flipping based on hinge side ---
            float signedAngle = hingeOnFront ? -angle : angle;

            // --- Build hinge rotation matrix in LOCAL space ---
            glm::mat4 hingeMat = glm::translate(glm::mat4(1.0f), hingeLocal) * glm::rotate(glm::mat4(1.0f), signedAngle, axis) * glm::translate(glm::mat4(1.0f), -hingeLocal);

            // --- Apply final model matrix with scale ---
            m_Mesh->modelMatrix = modelNoScale * hingeMat * glm::scale(glm::mat4(1.0f), scale);

            // --- Update collider ---
            AnimateCollider(modelNoScale * hingeMat);
        }

        void SimpleDoor::AnimateCollider(const glm::mat4 &hingeTransform)
        {
            if (m_RigidBody)
            {
                // Use the original shape scale only once
                btTransform t;
                t.setFromOpenGLMatrix(glm::value_ptr(hingeTransform));
                m_RigidBody->getMotionState()->setWorldTransform(t);
                m_RigidBody->setWorldTransform(t);
            }
        }

        void SimpleDoor::Update(bool check)
        {
            if (!m_RigidBody || !m_Mesh)
            {
                return;
            }

            const float speed = 1.5f;
            const float maxAngle = glm::radians(90.0f);

            float oldOpenAmount = m_OpenAmount;

            // Update open amount
            if (m_IsActive)
                m_OpenAmount = glm::min(m_OpenAmount + speed * Az::Timer::fDeltaTime, 1.0f);
            else
                m_OpenAmount = glm::max(m_OpenAmount - speed * Az::Timer::fDeltaTime, 0.0f);

            if (glm::epsilonEqual(oldOpenAmount, m_OpenAmount, 0.0001f) && m_Animated)
                return;

            const float angle = m_OpenAmount * maxAngle;

            Animate(angle);

            m_Animated = true;
        }

        void SimpleSlideDoor::Update(bool check)
        {
            if (!m_RigidBody || !m_Mesh)
                return;

            glm::vec3 min = m_Mesh->GetLocalAABBMin();
            glm::vec3 max = m_Mesh->GetLocalAABBMax();

            const float speed = 1.5f; // units per second

            const float doorDepth = max.z - min.z;
            const float maxDistance = (doorDepth * 2.0f) - 0.5f;

            float oldOpenAmount = m_OpenAmount;

            // Update open amount
            if (m_IsActive)
                m_OpenAmount = glm::min(m_OpenAmount + speed * Az::Timer::fDeltaTime, 1.0f);
            else
                m_OpenAmount = glm::max(m_OpenAmount - speed * Az::Timer::fDeltaTime, 0.0f);

            if (glm::epsilonEqual(oldOpenAmount, m_OpenAmount, 0.0001f) && m_Animated)
                return;

            float offsetX = m_OpenAmount * maxDistance;

            Animate(offsetX);
            m_Animated = true; // first Animate done
        }

        void SimpleSlideDoor::SetMesh(Az::Mesh *mesh)
        {
            m_Mesh = mesh;
            m_OriginalModel = mesh->modelMatrix;
            Update(false);
        }

        Az::Mesh *SimpleSlideDoor::GetMesh()
        {
            return m_Mesh;
        }

        void SimpleSlideDoor::Animate(float offsetX)
        {
            if (!m_Mesh)
                return;
            glm::vec3 scale;
            scale.x = glm::length(m_OriginalModel[0]);
            scale.y = glm::length(m_OriginalModel[1]);
            scale.z = glm::length(m_OriginalModel[2]);

            glm::mat4 modelNoScale = m_OriginalModel;
            modelNoScale[0] /= scale.x;
            modelNoScale[1] /= scale.y;
            modelNoScale[2] /= scale.z;

            glm::mat4 slideMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, offsetX));

            m_Mesh->modelMatrix = modelNoScale * slideMat * glm::scale(glm::mat4(1.0f), scale);
            AnimateCollider(modelNoScale * slideMat);
        }
        void SimpleSlideDoor::AnimateCollider(const glm::mat4 &slideTransform)
        {
            if (m_RigidBody)
            {
                btTransform t;
                t.setFromOpenGLMatrix(glm::value_ptr(slideTransform));
                m_RigidBody->getMotionState()->setWorldTransform(t);
                m_RigidBody->setWorldTransform(t);
            }
        }
    }
}
