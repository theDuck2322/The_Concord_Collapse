#include <SimpleSlideDoor.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Timer.h>
#include <Bullet/btBulletDynamicsCommon.h>

namespace Crd
{
    namespace MdLogic
    {
        void SimpleSlideDoor::SetMesh(Az::Mesh *mesh)
        {
            m_Mesh = mesh;
            m_OriginalModel = mesh->modelMatrix;
            Update(false);
        }

        Az::Mesh *SimpleSlideDoor::GetMesh() { return m_Mesh; }

        void SimpleSlideDoor::Animate(float offsetX)
        {
            if (!m_Mesh)
                return;

            glm::vec3 scale(glm::length(m_OriginalModel[0]),
                            glm::length(m_OriginalModel[1]),
                            glm::length(m_OriginalModel[2]));

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

        void SimpleSlideDoor::Update(bool check)
        {
            if (!m_RigidBody || !m_Mesh)
                return;

            glm::vec3 min = m_Mesh->GetLocalAABBMin();
            glm::vec3 max = m_Mesh->GetLocalAABBMax();

            const float speed = 1.5f;
            const float doorDepth = max.z - min.z;
            const float maxDistance = (doorDepth * 2.0f) - 0.5f;

            float oldOpenAmount = m_OpenAmount;
            m_OpenAmount = m_IsActive ? glm::min(m_OpenAmount + speed * Az::Timer::fDeltaTime, 1.0f) : glm::max(m_OpenAmount - speed * Az::Timer::fDeltaTime, 0.0f);

            if (glm::epsilonEqual(oldOpenAmount, m_OpenAmount, 0.0001f) && m_Animated)
                return;

            Animate(m_OpenAmount * maxDistance);
            m_Animated = true;
        }
    }
}
