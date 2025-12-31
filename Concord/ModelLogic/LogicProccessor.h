#pragma once

#include <PhysicsManager/PhysicsManager.h>

#include <MetadataChecker.h>
#include <PickableProp.h>
#include <mdControlled.h>
#include <mdControllers.h>

#include <Utils/Timer.h>

#include <vector>
#include <utility>
#include <unordered_map>
#include <memory>

namespace Crd
{
    namespace MdLogic
    {
        class LogicProcessor
        {
        public:
            void SetData(std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *data);
            void SetPickable(std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *pickable);
            bool Init(Az::physx::PhysicsManager *managerPtr);

            // Access controllers / controlleds
            std::vector<MdController *> GetControllers(uint32_t id);
            std::vector<MdControlled *> GetControlleds(uint32_t id);
            const std::unordered_map<uint32_t, std::vector<std::unique_ptr<MdController>>> &GetControllersMap() const
            {
                return m_Controllers;
            }
            const std::unordered_map<uint32_t, std::vector<std::unique_ptr<MdControlled>>> &GetControlledsMap() const
            {
                return m_Controlleds;
            }

            inline std::vector<std::unique_ptr<Crd::Object::Prop>> *GetProps() { return &m_Pickables; }

            void Update();

            void RaycastLogic(const btVector3 &from, const btVector3 &to, bool condition);
            Crd::Object::Prop *RaycastProp(const btVector3 &from, const btVector3 &to, bool condition);

        private:
            void m_CreateLogicObjects();
            void m_Binding();

            void m_CreatePickables();

        private:
            Az::physx::PhysicsManager *m_ManagerPtr;
            std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *m_Data = nullptr;
            std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *m_PickableObjects = nullptr;
            bool m_IsDataSet = false;
            bool m_IsPickableSet = false;

            // Multiple controllers & controlleds per id
            std::unordered_map<uint32_t, std::vector<std::unique_ptr<MdController>>> m_Controllers;
            std::unordered_map<uint32_t, std::vector<std::unique_ptr<MdControlled>>> m_Controlleds;
            std::vector<std::unique_ptr<Crd::Object::Prop>> m_Pickables;
        };
    }
}
