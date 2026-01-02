#include <SlideDoor.h>

namespace Crd
{
    namespace MdLogic
    {
        void SlideDoor::Update(bool firstCheck)
        {
            const float speed = 1.5f;

            glm::vec3 min = m_Mesh->GetLocalAABBMin();
            glm::vec3 max = m_Mesh->GetLocalAABBMax();

            float doorDepth = max.z - min.z;
            float maxDistance = (doorDepth * 2.0f) - 0.5f;

            float old = m_OpenAmount;
            m_OpenAmount = m_IsOpen ? glm::min(m_OpenAmount + speed * Az::Timer::fDeltaTime, 1.0f) : glm::max(m_OpenAmount - speed * Az::Timer::fDeltaTime, 0.0f);

            if (!firstCheck || !glm::epsilonEqual(old, m_OpenAmount, 0.0001f))
                Animate(m_OpenAmount * maxDistance);
        }

        glm::mat4 SlideDoor::ComputeTransform(float offset) const
        {
            // Slide along local Z-axis
            return glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -offset));
        }
    }
}