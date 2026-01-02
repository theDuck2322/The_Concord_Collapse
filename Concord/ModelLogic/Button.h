#pragma once
#include <mdControllers.h>

namespace Crd
{
    namespace MdLogic
    {
        class Button : public MdController
        {
        public:
            void SetActive(bool val) override;
            void SetMesh(Az::Mesh *mesh);
            Az::Mesh *GetMesh();
            void SetOnDuration(float duration);
            void SetRigidBody(btRigidBody *body) override;
            void Update(bool check = true) override;

        protected:
            Az::Mesh *m_Mesh = nullptr;
            float m_ON_Duration = 2.0f;
            float m_CurrentTime = 0.0f;
        };
    }
}
