#include "mdControlled.h"
#include <btBulletDynamicsCommon.h>
#include <DEBUG_Renderer.h>

namespace Crd
{
    namespace MdLogic
    {

        // ------------------------- DOOR functionality ---------------------------

        void Door::Activate()
        {
            m_IsOpen = true;
        }

        void Door::Deactivate()
        {
            m_IsOpen = false;
        }

        void Door::Animate(float angle)
        {
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

            // --- Local AABB hinge (mesh-local space) ---
            glm::vec3 min = m_Mesh->GetLocalAABBMin();
            glm::vec3 max = m_Mesh->GetLocalAABBMax();

            bool hingeOnLeft = true; // or metadata
            float hingeX = hingeOnLeft ? min.x : max.x;

            // --- Correct local hinge position ---
            glm::vec3 hingeLocal(
                (min.x + max.x) * 0.5f,                       // center along X
                min.y,                                        // bottom
                hingeOnLeft ? (min.z - 0.5f) : (max.z + 0.5f) // front/back edge along Z
            );

            // --- Signed rotation ---
            float signedAngle = hingeOnLeft ? -angle : angle;

            // --- Rotation axis (Y-up in local space) ---
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

            // --- Build hinge rotation matrix in LOCAL space ---
            glm::mat4 hingeMat = glm::translate(glm::mat4(1.0f), hingeLocal) * glm::rotate(glm::mat4(1.0f), signedAngle, up) * glm::translate(glm::mat4(1.0f), -hingeLocal);

            // --- Reapply scale ---
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

            // --- Final model matrix ---
            m_Mesh->modelMatrix = modelNoScale * hingeMat * scaleMat;

            // --- Update collider ---
            AnimateCollider(scale, modelNoScale * hingeMat);
        }

        void Door::AnimateCollider(const glm::vec3 &scale, const glm::mat4 &noScaleTimesHinge)
        {
            if (!m_RigidBody)
                return;

            btBoxShape *box =
                static_cast<btBoxShape *>(m_RigidBody->getCollisionShape());

            box->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

            btTransform t;
            t.setFromOpenGLMatrix(glm::value_ptr(noScaleTimesHinge));

            m_RigidBody->getMotionState()->setWorldTransform(t);
            m_RigidBody->setWorldTransform(t);
        }

        void Door::Update(bool Check)
        {
            const float speed = 1.5f;
            const float maxAngle = glm::radians(90.0f);

            float oldOpenAmount = m_OpenAmount;

            // Update open amount
            if (m_IsOpen)
                m_OpenAmount = glm::min(m_OpenAmount + speed * Az::Timer::fDeltaTime, 1.0f);
            else
                m_OpenAmount = glm::max(m_OpenAmount - speed * Az::Timer::fDeltaTime, 0.0f);

            // Skip animation if nothing changed
            if (glm::epsilonEqual(oldOpenAmount, m_OpenAmount, 0.0001f) && Check)
                return;

            Animate(m_OpenAmount * maxAngle);
        }

        // ------------------------- SLIDE DOOR functionality ---------------------------
        void SlideDoor::Animate(float offset)
        {
            // --- Extract scale ---
            glm::vec3 scale;
            scale.x = glm::length(m_OriginalModel[0]);
            scale.y = glm::length(m_OriginalModel[1]);
            scale.z = glm::length(m_OriginalModel[2]);

            // --- Build unscaled model matrix ---
            glm::mat4 modelNoScale = m_OriginalModel;
            modelNoScale[0] /= scale.x;
            modelNoScale[1] /= scale.y;
            modelNoScale[2] /= scale.z;

            // --- Slide along local Z ---
            glm::mat4 slide =
                glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -offset));

            // --- Reapply scale ---
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

            // --- Final model matrix ---
            m_Mesh->modelMatrix = modelNoScale * slide * scaleMat;

            // --- Sync physics ---
            AnimateCollider(scale, modelNoScale * slide);
        }

        void SlideDoor::AnimateCollider(const glm::vec3 &scale, const glm::mat4 &noScaleTimesSlide)
        {
            if (!m_RigidBody)
                return;

            btBoxShape *box =
                static_cast<btBoxShape *>(m_RigidBody->getCollisionShape());

            box->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

            btTransform t;
            t.setFromOpenGLMatrix(glm::value_ptr(noScaleTimesSlide));

            m_RigidBody->getMotionState()->setWorldTransform(t);
            m_RigidBody->setWorldTransform(t);
        }

        void SlideDoor::Update(bool Check)
        {
            const float speed = 1.5f;

            glm::vec3 min = m_Mesh->GetLocalAABBMin();
            glm::vec3 max = m_Mesh->GetLocalAABBMax();

            float doorDepth = max.z - min.z;
            float maxDistance = (doorDepth * 2.0f) - 0.5f;

            float old = m_OpenAmount;

            if (m_IsOpen)
                m_OpenAmount = glm::min(m_OpenAmount + speed * Az::Timer::fDeltaTime, 1.0f);
            else
                m_OpenAmount = glm::max(m_OpenAmount - speed * Az::Timer::fDeltaTime, 0.0f);

            if (Check && glm::epsilonEqual(old, m_OpenAmount, 0.0001f))
                return;

            Animate(m_OpenAmount * maxDistance);
        }
    }
}
