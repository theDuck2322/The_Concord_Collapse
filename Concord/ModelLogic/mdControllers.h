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
        // base class for controllers, button, switch, levers etc;
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
            inline void RemoveObject()
            {
                m_Object = nullptr;
            }

            virtual void SetRigidBody(btRigidBody *body) { m_RigidBody = body; }
            inline btRigidBody *GetRigidBody() const { return m_RigidBody; }
            // Update controller per frame
            virtual void Update(bool check = true) = 0;

        protected:
            btRigidBody *m_RigidBody = nullptr;
            MdControlled *m_Object;
            Crd::MdIsp::ParsedInput *m_Input;
            bool m_IsActive = false;
        };

        // ----------------------- Button class --------------------------------------
        // it has a duration for about 2 seconds to be active
        class Button : public MdController
        {
        public:
            void SetActive(bool val) override;
            void SetMesh(Az::Mesh *mesh);
            Az::Mesh *GetMesh();
            void SetOnDuration(float duration);
            virtual void Update(bool check = true);

            // this override calculates the model matrix as it should withous
            // scaling problems also it runs only once, and this optimisation fastened the logicproccess.update by 39%
            void SetRigidBody(btRigidBody *body) override;

        protected:
            Az::Mesh *m_Mesh;
            float m_ON_Duration = 2.0f;
            float m_CurrentTime = 0.0f;
        };

        // ------------------------ Switch class ---------------------------
        // it is a toggleable, very nice for daily use
        class Switch : public Button
        {
        public:
            void Update(bool check = true) override;
        };

        class SimpleDoor : public MdController
        {
        public:
            void SetMesh(Az::Mesh *mesh);
            Az::Mesh *GetMesh();
            void Animate(float angle);
            void AnimateCollider(const glm::mat4 &hingeTransform);
            void Update(bool check = true) override;

        protected:
            Az::Mesh *m_Mesh;
            glm::mat4 m_OriginalModel{1.0f};
            float m_OpenAmount = 0.0f; // 0 = closed, 1 = open
            bool m_IsOpen = false;
            bool m_Animated = false;
        };

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