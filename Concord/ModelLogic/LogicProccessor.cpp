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

#include <ProcessHelper.h>

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
            if (!condition)
                return;

            btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
            m_ManagerPtr->GetWorld()->rayTest(from, to, rayCallback);

            if (!rayCallback.hasHit())
                return;

            const btRigidBody *hitBody = btRigidBody::upcast(rayCallback.m_collisionObject);
            if (!hitBody)
                return;

            for (auto &[id, ctrls] : m_Controllers)
                for (auto &ctrl : ctrls)
                    if (ctrl->GetRigidBody() == hitBody)
                        ctrl->SetActive(!ctrl->IsActive());
        }

        Crd::Object::Prop *LogicProcessor::RaycastProp(const btVector3 &from, const btVector3 &to, bool condition)
        {
            if (!condition)
                return nullptr;

            btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
            m_ManagerPtr->GetWorld()->rayTest(from, to, rayCallback);

            if (!rayCallback.hasHit())
                return nullptr;

            const btRigidBody *hitBody = btRigidBody::upcast(rayCallback.m_collisionObject);
            if (!hitBody)
                return nullptr;

            for (auto &prop : m_Pickables)
                if (prop->GetRigidbody() == hitBody)
                    return prop.get();

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

            for (auto &[mesh, input] : *m_Data)
            {
                if (!input.valid)
                    continue;

                if (input.controllType == CONTROLLER)
                {
                    std::unique_ptr<MdController> ctrl;
                    switch (input.funcType)
                    {
                    case Crd::MdIsp::FunctionalityType::BUTTON:
                        ctrl = Internal::CreateController<Button>(mesh, input, m_ManagerPtr);
                        break;
                    case Crd::MdIsp::FunctionalityType::SWITCH:
                        ctrl = Internal::CreateController<Switch>(mesh, input, m_ManagerPtr);
                        break;
                    case Crd::MdIsp::FunctionalityType::SIMPLE_DOOR:
                        ctrl = Internal::CreateController<SimpleDoor>(mesh, input, m_ManagerPtr, true);
                        break;
                    case Crd::MdIsp::FunctionalityType::SIMPLE_SLIDE_DOOR:
                        ctrl = Internal::CreateController<SimpleSlideDoor>(mesh, input, m_ManagerPtr, true);
                        break;
                    default:
                        break;
                    }
                    if (ctrl)
                        m_Controllers[input.id1].push_back(std::move(ctrl));
                }
                else if (input.controllType == CONTROLLED)
                {
                    std::unique_ptr<MdControlled> obj;
                    switch (input.funcType)
                    {
                    case Crd::MdIsp::FunctionalityType::DOOR:
                        obj = Internal::CreateControlled<Door>(mesh, input, m_ManagerPtr);
                        break;
                    case Crd::MdIsp::FunctionalityType::SLIDE_DOOR:
                        obj = Internal::CreateControlled<SlideDoor>(mesh, input, m_ManagerPtr);
                        break;
                    default:
                        break;
                    }
                    if (obj)
                        m_Controlleds[input.id1].push_back(std::move(obj));
                }
            }
        }

        void LogicProcessor::m_Binding()
        {
            if (!m_Data)
                return;

            for (auto &[mesh, input] : *m_Data)
            {
                if (!input.valid || input.controllType != CONTROLLER)
                    continue;

                auto itCtrl = m_Controllers.find(input.id1);
                if (itCtrl == m_Controllers.end())
                    continue;

                if (input.id2 != 0)
                {
                    auto itObj = m_Controlleds.find(input.id2);
                    if (itObj != m_Controlleds.end())
                    {
                        for (auto &ctrl : itCtrl->second)
                            for (auto &obj : itObj->second)
                                ctrl->AddControlled(obj.get());
#ifdef AZ_DEBUG
                        std::cout << "Controller " << input.id1 << " bound to Controlled " << input.id2 << std::endl;
#endif
                    }
#ifdef AZ_DEBUG
                    else
                    {
                        std::cout << "Controller " << input.id1 << " references missing Controlled " << input.id2 << std::endl;
                    }
#endif
                }
            }
        }

        void LogicProcessor::m_CreatePickables()
        {
            if (!m_PickableObjects)
                return;

            m_Pickables.clear();
            for (auto &[mesh, input] : *m_PickableObjects)
            {
                if (!input.valid)
                    continue;
                m_Pickables.push_back(Internal::CreatePickable(mesh, m_ManagerPtr));
            }
        }
    }
}
