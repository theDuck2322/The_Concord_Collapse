#pragma once
#include <Model/Model.h>
#include <glm/glm.hpp>
#include <mdControlled.h>
#include <ModelInspector/MetadataChecker.h>

namespace Crd
{
    namespace MdLogic
    {
        // ----------------------------- MdController class -----------------------------
        class MdController
        {
        public:
            virtual ~MdController() = default;

            virtual void SetActive(bool val) { m_IsActive = val; }
            virtual bool IsActive() const { return m_IsActive; }
            virtual void SetParsedInput(Crd::MdIsp::ParsedInput *input) { m_Input = input; }
            virtual Crd::MdIsp::ParsedInput *GetParsedInput() { return m_Input; }

            inline void AddControlled(MdControlled *obj)
            {
                if (obj)
                    m_Object = obj;
            }
            inline MdControlled *GetControlled() { return m_Object; }
            inline void RemoveObject() { m_Object = nullptr; }

            virtual void SetRigidBody(btRigidBody *body) { m_RigidBody = body; }
            inline btRigidBody *GetRigidBody() const { return m_RigidBody; }

            virtual void Update(bool check = true) = 0;

        protected:
            btRigidBody *m_RigidBody = nullptr;
            MdControlled *m_Object = nullptr;
            Crd::MdIsp::ParsedInput *m_Input = nullptr;
            bool m_IsActive = false;
        };
    }
}