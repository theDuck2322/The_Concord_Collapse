#include <Door.h>

namespace Crd
{
    namespace MdLogic
    {
        void Door::SetMesh(Az::Mesh *mesh)
        {
            if (!mesh)
            {
                std::cout << "Mesh is nullptr" << std::endl;
                return;
            }
            m_Mesh = mesh;
            m_OriginalModel = mesh->modelMatrix;
            Update(false);
        }
        void Door::Update(bool firstCheck)
        {
            const float speed = 1.5f;
            const float maxAngle = glm::radians(90.0f);

            float old = m_OpenAmount;
            m_OpenAmount = m_IsOpen ? glm::min(m_OpenAmount + speed * Az::Timer::fDeltaTime, 1.0f) : glm::max(m_OpenAmount - speed * Az::Timer::fDeltaTime, 0.0f);

            if (!firstCheck || !glm::epsilonEqual(old, m_OpenAmount, 0.0001f))
                Animate(m_OpenAmount * maxAngle);
        }
        glm::vec3 Door::ExtractScale(const glm::mat4 &matrix) const
        {
            return glm::vec3(glm::length(matrix[0]), glm::length(matrix[1]), glm::length(matrix[2]));
        }

        glm::mat4 Door::RemoveScale(const glm::mat4 &matrix, const glm::vec3 &scale) const
        {
            glm::mat4 result = matrix;
            result[0] /= scale.x;
            result[1] /= scale.y;
            result[2] /= scale.z;
            return result;
        }
        glm::mat4 Door::ComputeTransform(float angle) const
        {
            // Default hinge on left Y-axis
            glm::vec3 min = m_Mesh->GetLocalAABBMin();
            glm::vec3 max = m_Mesh->GetLocalAABBMax();
            bool hingeOnLeft = true;

            glm::vec3 hingeLocal(
                (min.x + max.x) * 0.5f,
                min.y,
                hingeOnLeft ? (min.z - 0.5f) : (max.z + 0.5f));

            float signedAngle = hingeOnLeft ? -angle : angle;
            glm::vec3 up(0, 1, 0);

            return glm::translate(glm::mat4(1.0f), hingeLocal) *
                   glm::rotate(glm::mat4(1.0f), signedAngle, up) *
                   glm::translate(glm::mat4(1.0f), -hingeLocal);
        }
        void Door::ApplyTransformToPhysics(const glm::mat4 &transform)
        {
            if (!m_RigidBody)
                return;

            glm::vec3 scale = ExtractScale(m_OriginalModel);
            btBoxShape *box = static_cast<btBoxShape *>(m_RigidBody->getCollisionShape());
            box->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

            btTransform t;
            t.setFromOpenGLMatrix(glm::value_ptr(transform));
            m_RigidBody->getMotionState()->setWorldTransform(t);
            m_RigidBody->setWorldTransform(t);
        }
        void Door::Animate(float angle)
        {
            glm::vec3 scale = ExtractScale(m_OriginalModel);
            glm::mat4 modelNoScale = RemoveScale(m_OriginalModel, scale);
            glm::mat4 relative = ComputeTransform(angle);
            glm::mat4 finalTransform = modelNoScale * relative * glm::scale(glm::mat4(1.0f), scale);

            m_Mesh->modelMatrix = finalTransform;
            ApplyTransformToPhysics(modelNoScale * relative);
        }
    }
}