#pragma once
#include <MdControllers.h>

namespace Crd
{
    namespace MdLogic
    {
        class SimpleSlideDoor : public MdController
        {
        public:
            void SetMesh(Az::Mesh *mesh);
            Az::Mesh *GetMesh();
            void Animate(float offsetX);
            void AnimateCollider(const glm::mat4 &slideTransform);
            void Update(bool check = true) override;

        protected:
            Az::Mesh *m_Mesh = nullptr;
            glm::mat4 m_OriginalModel{1.0f};
            float m_OpenAmount = 0.0f;
            bool m_IsOpen = false;
            bool m_Animated = false;
        };
    }
}
