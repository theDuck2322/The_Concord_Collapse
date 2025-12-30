#include <PickableProp.h>

namespace Crd
{
    namespace Object
    {
        void Prop::SetMesh(Az::Mesh *mesh)
        {
            m_Mesh = mesh;
            m_OriginalModel = mesh->modelMatrix;
        }

        void Prop::SetRigidBody(btRigidBody *body)
        {
            m_RigidBody = body;
            if (body == nullptr)
                std::cout << "RigidBody is nullptr" << std::endl;
        }

        void Prop::Move(const glm::vec3 &position)
        {
            if (!m_RigidBody)
                return;

            btTransform transform = m_RigidBody->getWorldTransform();
            transform.setOrigin(btVector3(position.x, position.y, position.z));
            m_RigidBody->setWorldTransform(transform);
            m_RigidBody->activate(); // wake up the rigidbody
        }

        void Prop::Rotate(glm::vec3 *pivot)
        {
        }
    }
}