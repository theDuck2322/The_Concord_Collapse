#pragma once
#include <MetadataChecker.h>
#include <vector>
#include <utility>
#include <mdControllers.h>
#include <unordered_map>
#include <memory>
#include <Utils/Timer.h>
#include <PhysicsManager/PhysicsManager.h>

namespace Crd
{
    namespace MdLogic
    {
        class LogicProcessor
        {
        public:
            void SetData(std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *data);
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
            void Update();

            void RaycastTest(const btVector3 &from, const btVector3 &to, bool condition);

        private:
            void m_Binding();

        private:
            Az::physx::PhysicsManager *m_ManagerPtr;
            std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *m_Data = nullptr;
            bool m_IsDataSet = false;

            // Multiple controllers & controlleds per id
            std::unordered_map<uint32_t, std::vector<std::unique_ptr<MdController>>> m_Controllers;
            std::unordered_map<uint32_t, std::vector<std::unique_ptr<MdControlled>>> m_Controlleds;
        };
    }
}
