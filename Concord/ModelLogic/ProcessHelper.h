#pragma once

#include <MetadataChecker.h>
#include <PickableProp.h>
#include <mdControlled.h>
#include <mdControllers.h>

namespace Crd
{
    namespace MdLogic
    {
        namespace Internal
        {
            void PrintInput(const Crd::MdIsp::ParsedInput &inp);
            std::unique_ptr<Crd::Object::Prop> CreatePickable(Az::Mesh *mesh, Az::physx::PhysicsManager *mgr);

            template <typename T>
            std::unique_ptr<T> CreateController(Az::Mesh *mesh, Crd::MdIsp::ParsedInput &input, Az::physx::PhysicsManager *mgr, bool isDoor = false)
            {
                auto ctrl = std::make_unique<T>();
                ctrl->SetMesh(mesh);
                size_t id = mgr->CreateKinematicMeshCollider(mesh, ctrl.get(), isDoor);
                btRigidBody *body = mgr->GetRigidbodyById(id);
                if (!body)
                {
#ifdef AZ_DEBUG
                    std::cout << "Body was not created for type " << input.type << std::endl;
#endif
                }
                ctrl->SetRigidBody(body);
                ctrl->SetParsedInput(&input);

                PrintInput(input);
                return ctrl;
            }
            template <typename T>
            std::unique_ptr<T> CreateControlled(Az::Mesh *mesh, Crd::MdIsp::ParsedInput &input, Az::physx::PhysicsManager *mgr)
            {
                auto obj = std::make_unique<T>();
                obj->SetMesh(mesh);
                size_t id = mgr->CreateKinematicMeshCollider(mesh, obj.get());
                btRigidBody *body = mgr->GetRigidbodyById(id);
                if (!body)
                {
#ifdef AZ_DEBUG
                    std::cout << "Controlled Body was not created for type " << input.type << std::endl;
#endif
                }
                obj->SetRigidBody(body);
                obj->SetParsedInput(&input);

                PrintInput(input);
                return obj;
            }
        }
    }
}
