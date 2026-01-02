#pragma once
#include <mdControlled.h>

namespace Crd
{
    namespace MdLogic
    {
        class Door : public MdControlled
        {
        public:
            void SetMesh(Az::Mesh *mesh);

            void Activate() override { m_IsOpen = true; }
            void Deactivate() override { m_IsOpen = false; }

            void Update(bool firstCheck = false) override;

        protected:
            Az::Mesh *m_Mesh = nullptr;
            glm::mat4 m_OriginalModel{1.0f};
            float m_OpenAmount = 0.0f; // 0 = closed, 1 = open
            bool m_IsOpen = false;

            // ------------------ Helper Functions -------------------
            glm::vec3 ExtractScale(const glm::mat4 &matrix) const;

            glm::mat4 RemoveScale(const glm::mat4 &matrix, const glm::vec3 &scale) const;

            virtual glm::mat4 ComputeTransform(float angle) const;

            void ApplyTransformToPhysics(const glm::mat4 &transform);

            void Animate(float angle);
        };
    }
}