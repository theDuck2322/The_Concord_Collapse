#pragma once
#include <Model.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Utils/Timer.h>
#include <ModelInspector/MetadataChecker.h>
#include <Bullet/btBulletDynamicsCommon.h>
#include <iostream>

namespace Crd
{
    namespace MdLogic
    {
        // ------------------ Base Controlled Class -----------------------------
        class MdControlled
        {
        public:
            virtual void SetParsedInput(Crd::MdIsp::ParsedInput *input) { m_Input = input; }
            virtual void Activate() = 0;
            virtual void Deactivate() = 0;
            virtual void Update(bool firstCheck = false) = 0;

            inline void SetRigidBody(btRigidBody *body) { m_RigidBody = body; }
            inline btRigidBody *GetRigidBody() const { return m_RigidBody; }

            virtual ~MdControlled() = default;

        protected:
            btRigidBody *m_RigidBody = nullptr;
            Crd::MdIsp::ParsedInput *m_Input = nullptr;
        };

        // ------------------ SlideDoor Class -----------------------------

    }
}
