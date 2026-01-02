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

            std::unique_ptr<Crd::Object::Prop> CreatePickable(Az::Mesh *mesh, Az::physx::PhysicsManager *mgr)
            {
                auto prop = std::make_unique<Crd::Object::Prop>();
                prop->SetMesh(mesh);
                size_t id = mgr->CreateConvexHullBody(mesh, 1.0f);
                auto body = mgr->GetRigidbodyById(id);
                body->setFriction(1.0f);
                body->setDamping(0.5f, 0.5f);
                prop->SetRigidBody(body);
                prop->Init();
                return prop;
            }
        }
    }
}