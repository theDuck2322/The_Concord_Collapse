#pragma once
#include <Model.h>
#include <glm/glm.hpp>
#include <Utils/Timer.h>
#include <ModelInspector/MetadataChecker.h>
#include <Bullet/btBulletDynamicsCommon.h>

namespace Crd
{
    namespace MdLogic
    {

        // ------------------ Md_Controlled class -----------------------------
        // base class for functionality targets
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
            Crd::MdIsp::ParsedInput *m_Input;
        };

        // ------------------------- Door class ------------------------------
        // regular door, nothing soo impressive
        class Door : public MdControlled
        {
        public:
            inline void SetMesh(Az::Mesh *mesh)
            {
                m_Mesh = mesh;
                m_OriginalModel = mesh->modelMatrix; // ← initialize here
                Update(false);
            }
            void Activate();
            void Deactivate();
            void Animate(float angle);
            void AnimateCollider(const glm::vec3 &scale, const glm::mat4 &noScaleTimesHinge);

            virtual void Update(bool Check = true);

        protected:
            Az::Mesh *m_Mesh;
            glm::mat4 m_OriginalModel{1.0f};
            float m_OpenAmount = 0.0f; // 0 = closed, 1 = open
            bool m_IsOpen = false;
        };

        // ------------------------- SLIDE DOOR class -----------------------
        // maybe the only kind of doors, just kidding
        class SlideDoor : public Door
        {
        public:
            void Animate(float offsetX);
            void AnimateCollider(const glm::vec3 &scale, const glm::mat4 &noScaleTimesSlide);
            void Update(bool Check = true) override;

        private:
        };
    }
}