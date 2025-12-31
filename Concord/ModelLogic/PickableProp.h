#pragma once

#include <Model/Model.h>
#include <PhysicsManager/PhysicsManager.h>
#include <Utils/Helpers.h>

#include <glm/glm.hpp>

namespace Crd
{
    namespace Object
    {
        class Prop
        {
        public:
            static void SetPhysicsManagerPtr(Az::physx::PhysicsManager *manager);

            void SetMesh(Az::Mesh *mesh);
            void SetRigidBody(btRigidBody *body);

            bool Init();

            void PickUp(const glm::vec3 &playerPos, const glm::quat &playerRot);
            void Move(const glm::vec3 &playerPos, const glm::vec3 &forwardVector, const glm::quat &playerRot);
            void Rotate();
            inline Az::Mesh *GetMesh() { return m_Mesh; }
            glm::mat4 *GetModelMatrix();

            inline bool IsPickedUp() { return m_PickedUp; }
            inline glm::vec3 GetPosition() { return m_Position; }

            inline btRigidBody *GetRigidbody() { return m_RigidBody; }

        private:
            static Az::physx::PhysicsManager *s_PhysicsManager;

            Az::Mesh *m_Mesh = nullptr;
            btRigidBody *m_RigidBody = nullptr;
            btPoint2PointConstraint *m_GrabConstraint = nullptr;

            glm::mat4 m_OriginalModel{1.0f};
            glm::mat4 m_OverrideModel{1.0f};

            glm::vec3 m_Position;
            glm::quat m_HeldRotationOffset = glm::quat(1, 0, 0, 0); // rotation while held

            bool m_PickedUp = false;
        };
    }
}
