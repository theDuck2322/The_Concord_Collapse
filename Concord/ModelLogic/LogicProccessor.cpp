#include <ModelLogic/LogicProccessor.h>
#include <iostream>
#include <Utils/Helpers.h>
#include "LogicProccessor.h"

#include <Button.h>
#include <Switch.h>
#include <Door.h>
#include <SlideDoor.h>
#include <SimpleDoor.h>
#include <SimpleSlideDoor.h>

namespace Crd
{
    namespace MdLogic
    {
        void LogicProcessor::SetData(std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *data)
        {
            m_Data = data;
            m_IsDataSet = (data != nullptr);
        }

        void LogicProcessor::SetPickable(std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *pickable)
        {
            m_PickableObjects = pickable;
            m_IsPickableSet = (pickable != nullptr);
        }

        bool LogicProcessor::Init(Az::physx::PhysicsManager *managerPtr)
        {
            if (!managerPtr)
                return true;

            m_ManagerPtr = managerPtr;

            m_CreateLogicObjects();
            m_Binding();

            m_CreatePickables();

            return false;
        }

        std::vector<MdController *> LogicProcessor::GetControllers(uint32_t id)
        {
            std::vector<MdController *> result;
            auto it = m_Controllers.find(id);
            if (it != m_Controllers.end())
            {
                for (auto &ctrl : it->second)
                    result.push_back(ctrl.get());
            }
            return result;
        }

        std::vector<MdControlled *> LogicProcessor::GetControlleds(uint32_t id)
        {
            std::vector<MdControlled *> result;
            auto it = m_Controlleds.find(id);
            if (it != m_Controlleds.end())
            {
                for (auto &obj : it->second)
                    result.push_back(obj.get());
            }
            return result;
        }

        void LogicProcessor::Update()
        {
            for (auto &[id, vec] : m_Controllers)
            {
                for (auto &ctrl : vec)
                    ctrl->Update();
            }
        }

        void LogicProcessor::RaycastLogic(const btVector3 &from, const btVector3 &to, bool condition)
        {
            btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
            m_ManagerPtr->GetWorld()->rayTest(from, to, rayCallback);

            if (rayCallback.hasHit())
            {
                const btRigidBody *hitBody = btRigidBody::upcast(rayCallback.m_collisionObject);
                if (hitBody)
                {
                    // Compare with controller rigid bodies
                    for (auto &[id, ctrlVec] : m_Controllers)
                    {
                        for (auto &ctrlPtr : ctrlVec)
                        {
                            if (ctrlPtr->GetRigidBody() == hitBody && condition)
                            {
                                ctrlPtr->SetActive(!ctrlPtr->IsActive());
#ifdef AZ_DEBUG
                                std::cout << "Ray hit controller ID: " << id << std::endl;
#endif
                            }
                        }
                    }
                }
            }
        }

        /// think on how to implement this
        Crd::Object::Prop *LogicProcessor::RaycastProp(
            const btVector3 &from, const btVector3 &to, bool condition)
        {
            if (!condition)
                return nullptr;

            btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
            m_ManagerPtr->GetWorld()->rayTest(from, to, rayCallback);

            if (!rayCallback.hasHit())
                return nullptr;

            const btRigidBody *hitBody =
                btRigidBody::upcast(rayCallback.m_collisionObject);

            if (!hitBody)
                return nullptr;

            for (auto &prop : m_Pickables)
            {
                if (prop->GetRigidbody() == hitBody)
                    return prop.get();
            }

            return nullptr;
        }

        void printInput(const Crd::MdIsp::ParsedInput &inp)
        {
            std::cout << inp.type << " " << inp.id1 << " " << inp.connector << " " << inp.id2 << std::endl;
        }

        void LogicProcessor::m_CreateLogicObjects()
        {
            if (!m_Data)
                return;

            m_Controllers.clear();
            m_Controlleds.clear();

            for (auto &p : *m_Data)
            {
                Az::Mesh *mesh = p.first;
                Crd::MdIsp::ParsedInput &input = p.second;

                if (!input.valid)
                    continue;

                if (input.controllType == CONTROLLER)
                {
                    std::unique_ptr<MdController> ctrl = nullptr;

                    switch (input.funcType)
                    {
                    case Crd::MdIsp::FunctionalityType::BUTTON:
                    {
                        auto button = std::make_unique<Button>();
                        button->SetMesh(mesh);

                        size_t id = m_ManagerPtr->CreateKinematicMeshCollider(mesh, button.get());

                        btRigidBody *body = m_ManagerPtr->GetRigidbodyById(id);
                        if (!body)
                        {
#ifdef AZ_DEBUG
                            std::cout << "BUTTON Body was not created" << std::endl;
#endif
                        }

#ifdef AZ_DEBUG
                        printInput(input);
#endif
                        button->SetRigidBody(body);
                        ctrl = std::move(button);

                        break;
                    }
                    case Crd::MdIsp::FunctionalityType::SWITCH:
                    {
                        auto swtch = std::make_unique<Switch>();
                        swtch->SetMesh(mesh);
                        size_t id = m_ManagerPtr->CreateKinematicMeshCollider(mesh, swtch.get());
                        btRigidBody *body = m_ManagerPtr->GetRigidbodyById(id);
                        if (!body)
                        {
#ifdef AZ_DEBUG
                            std::cout << "SWITCH Body was not created" << std::endl;
#endif
                        }

                        swtch->SetRigidBody(body);
                        ctrl = std::move(swtch);

                        break;
                    }
                    case Crd::MdIsp::FunctionalityType::SIMPLE_DOOR:
                    {
                        auto sdor = std::make_unique<SimpleDoor>();
                        sdor->SetMesh(mesh);
                        size_t id = m_ManagerPtr->CreateKinematicMeshCollider(mesh, sdor.get(), true);
                        btRigidBody *body = m_ManagerPtr->GetRigidbodyById(id);
                        if (!body)
                        {
#ifdef AZ_DEBUG
                            std::cout << "SIMPLE_DOOR Body was not created" << std::endl;
#endif
                        }
#ifdef AZ_DEBUG
                        printInput(input);
#endif
                        sdor->SetRigidBody(body);
                        ctrl = std::move(sdor);
                        break;
                    }
                    case Crd::MdIsp::FunctionalityType::SIMPLE_SLIDE_DOOR:
                    {
                        auto ssdor = std::make_unique<SimpleSlideDoor>();
                        ssdor->SetMesh(mesh);
                        size_t id = m_ManagerPtr->CreateKinematicMeshCollider(mesh, ssdor.get(), true);
                        btRigidBody *body = m_ManagerPtr->GetRigidbodyById(id);
                        if (!body)
                        {
#ifdef AZ_DEBUG
                            std::cout << "SIMPLE_SLIDE_DOOR Body was not created" << std::endl;
#endif
                        }
#ifdef AZ_DEBUG
                        printInput(input);
#endif
                        ssdor->SetRigidBody(body);
                        ctrl = std::move(ssdor);

                        break;
                    }

                    default:
                        break;
                    }

                    if (ctrl)
                    {
                        ctrl->SetParsedInput(&input);
                        m_Controllers[input.id1].push_back(std::move(ctrl));
                    }
                }
                else if (input.controllType == CONTROLLED)
                {
                    std::unique_ptr<MdControlled> obj = nullptr;

                    switch (input.funcType)
                    {
                    case Crd::MdIsp::FunctionalityType::DOOR:
                    {
                        auto door = std::make_unique<Door>();
                        door->SetMesh(mesh);
                        size_t id = m_ManagerPtr->CreateKinematicMeshCollider(mesh, door.get());
                        btRigidBody *body = m_ManagerPtr->GetRigidbodyById(id);
                        if (!body)
                        {
#ifdef AZ_DEBUG
                            std::cout << "DOOR Body was not created" << std::endl;
#endif
                        }
#ifdef AZ_DEBUG
                        printInput(input);
#endif
                        door->SetRigidBody(body);
                        obj = std::move(door);

                        break;
                    }
                    case Crd::MdIsp::FunctionalityType::SLIDE_DOOR:
                    {
                        auto slDoor = std::make_unique<SlideDoor>();
                        slDoor->SetMesh(mesh);
                        size_t id = m_ManagerPtr->CreateKinematicMeshCollider(mesh, slDoor.get());
                        btRigidBody *body = m_ManagerPtr->GetRigidbodyById(id);
                        if (!body)
                        {
#ifdef AZ_DEBUG
                            std::cout << "SLIDE_DOOR Body was not created" << std::endl;
#endif
                        }
#ifdef AZ_DEBUG
                        printInput(input);
#endif
                        slDoor->SetRigidBody(body);
                        obj = std::move(slDoor);

                        break;
                    }
                    default:
                        break;
                    }

                    if (obj)
                    {
                        obj->SetParsedInput(&input);
                        m_Controlleds[input.id1].push_back(std::move(obj));
                    }
                }
            }
        }

        void LogicProcessor::m_Binding()
        {
            if (!m_Data)
                return;

            for (auto &p : *m_Data)
            {
                Crd::MdIsp::ParsedInput &input = p.second;
                if (!input.valid || input.controllType != CONTROLLER)
                    continue;

                auto itCtrls = m_Controllers.find(input.id1);
                if (itCtrls == m_Controllers.end())
                    continue;

                auto itObjs = m_Controlleds.find(input.id2);

                if (input.id2 != 0 && itObjs != m_Controlleds.end())
                {
                    for (auto &ctrl : itCtrls->second)
                    {
                        for (auto &obj : itObjs->second)
                        {
                            ctrl->AddControlled(obj.get());
                        }
                    }
#ifdef AZ_DEBUG
                    std::cout << "Connection was made" << std::endl;
#endif
                }
#ifdef AZ_DEBUG

                else if (input.id2 != 0)
                {
                    std::cout << "Warning: Controller id " << input.id1
                              << " references missing controlled id " << input.id2 << std::endl;
                }
#endif
            }
        }
        void LogicProcessor::m_CreatePickables()
        {
            if (!m_PickableObjects)
                return;

            m_Pickables.clear();

            for (auto &p : *m_PickableObjects)
            {
                Az::Mesh *mesh = p.first;
                Crd::MdIsp::ParsedInput &input = p.second;

                if (!input.valid)
                    continue;

                auto prop = std::make_unique<Crd::Object::Prop>();
                prop->SetMesh(mesh);
                size_t id = m_ManagerPtr->CreateConvexHullBody(mesh, 1.0f);
                auto body = m_ManagerPtr->GetRigidbodyById(id);
                body->setFriction(1.0f);
                body->setDamping(0.5f, 0.5f);
                prop->SetRigidBody(body);
                prop->Init();

                m_Pickables.emplace_back(std::move(prop));
            }
        }
    }
}
