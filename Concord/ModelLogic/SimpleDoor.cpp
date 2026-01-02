#include <SimpleDoor.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Timer.h>
#include <Bullet/btBulletDynamicsCommon.h>

namespace Crd
{
    namespace MdLogic
    {
        void SimpleDoor::SetMesh(Az::Mesh *mesh)
        {
            m_Mesh = mesh;
            m_OriginalModel = mesh->modelMatrix;
            Update(false);
        }

        Az::Mesh *SimpleDoor::GetMesh() { return m_Mesh; }

        void SimpleDoor::Animate(float angle)
        {
            if (!m_Mesh)
                return;

            glm::vec3 scale(
                glm::length(m_OriginalModel[0]),
                glm::length(m_OriginalModel[1]),
                glm::length(m_OriginalModel[2]));

            glm::mat4 modelNoScale = m_OriginalModel;
            modelNoScale[0] /= scale.x;
            modelNoScale[1] /= scale.y;
            modelNoScale[2] /= scale.z;

            glm::vec3 min = m_Mesh->GetLocalAABBMin();
            glm::vec3 max = m_Mesh->GetLocalAABBMax();

            glm::vec3 hingeLocal((min.x + max.x) * 0.5f, min.y, min.z - 0.5);
            glm::vec3 axis(0.0f, 1.0f, 0.0f);
            float signedAngle = -angle;

            glm::mat4 hingeMat = glm::translate(glm::mat4(1.0f), hingeLocal) * glm::rotate(glm::mat4(1.0f), signedAngle, axis) * glm::translate(glm::mat4(1.0f), -hingeLocal);

            m_Mesh->modelMatrix = modelNoScale * hingeMat * glm::scale(glm::mat4(1.0f), scale);
            AnimateCollider(modelNoScale * hingeMat);
        }

        void SimpleDoor::AnimateCollider(const glm::mat4 &hingeTransform)
        {
            if (m_RigidBody)
            {
                btTransform t;
                t.setFromOpenGLMatrix(glm::value_ptr(hingeTransform));
                m_RigidBody->getMotionState()->setWorldTransform(t);
                m_RigidBody->setWorldTransform(t);
            }
        }

        void SimpleDoor::Update(bool check)
        {
            if (!m_RigidBody || !m_Mesh)
                return;

            const float speed = 1.5f;
            const float maxAngle = glm::radians(90.0f);

            float oldOpenAmount = m_OpenAmount;
            m_OpenAmount = m_IsActive ? glm::min(m_OpenAmount + speed * Az::Timer::fDeltaTime, 1.0f) : glm::max(m_OpenAmount - speed * Az::Timer::fDeltaTime, 0.0f);

            if (glm::epsilonEqual(oldOpenAmount, m_OpenAmount, 0.0001f) && m_Animated)
                return;

            Animate(m_OpenAmount * maxAngle);
            m_Animated = true;
        }
    }
}
