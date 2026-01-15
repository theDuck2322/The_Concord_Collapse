#include <ProcessHelper.h>

namespace Crd
{
    namespace MdLogic
    {
        namespace Internal
        {
            void PrintInput(const Crd::MdIsp::ParsedInput &inp)
            {
#ifdef AZ_DEBUG
                std::cout << inp.type << " " << inp.id1 << " " << inp.connector << " " << inp.id2 << std::endl;
#endif
            }

            std::unique_ptr<Crd::Object::Prop> CreatePickable(Az::Mesh *mesh, Az::physx::PhysicsManager *manager)
            {
                // Save GLB transform
                glm::mat4 glbTransform = mesh->localMatrix;
                glm::vec3 glbPos = glm::vec3(glbTransform[3]);
                glm::quat glbRot = glm::quat_cast(glbTransform);

                auto prop = std::make_unique<Crd::Object::Prop>();
                prop->SetMesh(mesh); // this will reset mesh->localMatrix to identity

                size_t id = manager->CreateConvexHullBody(mesh, 1.0f);
                auto body = manager->GetRigidbodyById(id);

                btTransform tr;
                tr.setIdentity();
                tr.setOrigin(Az::ConvertGLMVec3(glbPos));
                tr.setRotation(Az::ConvertGLMQuat(glbRot));

                body->setWorldTransform(tr);
                body->getMotionState()->setWorldTransform(tr);

                // body->setFriction(0.15f);
                body->setDamping(0.15f, 0.15f);

                prop->SetRigidBody(body);
                prop->Init();
                return prop;
            }
        }
    }
}