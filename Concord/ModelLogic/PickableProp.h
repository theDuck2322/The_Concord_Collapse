#pragma once

#include <Model/Model.h>

#include <glm/glm.hpp>
#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/btBulletDynamicsCommon.h>

namespace Crd
{
    namespace Object
    {
        class Prop
        {
        public:
            void SetMesh(Az::Mesh *mesh);
            void SetRigidBody(btRigidBody *body);

            // player position + prop position
            // if i do that every frame the prop would fly away i think
            void Move(const glm::vec3 &position);

            // if player presses z for example then the prop rotates along its origin
            // else it rotates along player position so for this to happens we substract player position
            // from the prop then rotate then translate back to player position or we make
            // prop position 0 while rotating
            // if pivot is nullptr, the prop uses its position as origin
            void Rotate(glm::vec3 *pivot);

        private:
            Az::Mesh *m_Mesh;
            btRigidBody *m_RigidBody;

            glm::mat4 m_OriginalModel;

            glm::mat4 m_Transform;
        };
    }
}