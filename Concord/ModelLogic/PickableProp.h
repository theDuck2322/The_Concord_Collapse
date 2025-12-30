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
            // because Prop is a physics object i will use its physics transform
            // to calculate the model matrix

            static void SetPhysicsManagerPtr(Az::physx::PhysicsManager *manager);

            void SetMesh(Az::Mesh *mesh);
            void SetRigidBody(btRigidBody *body);

            bool Init();

            void PickUp(const glm::vec3 &grabPoint);

            inline bool IsPickedUp() { return m_PickedUp; }
            // player position + prop position
            // if i do that every frame the prop would fly away i think
            void Move(const glm::vec3 &playerPos, const glm::vec3 &forwardVector, const glm::quat &playerRot);

            // if player presses z for example then the prop rotates along its origin
            // else it rotates along player position so for this to happens we substract player position
            // from the prop then rotate then translate back to player position or we make
            // prop position 0 while rotating
            // if pivot is nullptr, the prop uses its position as origin
            void Rotate(glm::vec3 *pivot);

            glm::mat4 *GetModelMatrix();

            inline glm::vec3 GetPosition() { return m_Position; }

        private:
            static Az::physx::PhysicsManager *s_PhysicsManager;

            Az::Mesh *m_Mesh;
            btRigidBody *m_RigidBody;
            glm::mat4 m_OriginalModel{1.0f}; // maybe i'll need it sometime

            btPoint2PointConstraint *m_GrabConstraint = nullptr;

            glm::mat4 m_OverrideModel{1.0f};

            glm::vec3 m_Position;

            bool m_PickedUp = false;
        };
    }
}