#pragma once
#include <Model/Model.h>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#define UNDEFINED 0
#define CONTROLLER 1
#define CONTROLLED 2

namespace Crd
{

    namespace MdIsp
    {

        enum class FunctionalityType
        {
            BUTTON = 1,
            SWITCH,
            SIMPLE_DOOR,
            SIMPLE_SLIDE_DOOR,
            LEVER,
            DOOR,
            SLIDE_DOOR,
            ROTARY,
            ELEVATOR,
            BRIDGE,
            GATE,
            _UNDEFINED_
        };
        struct FunctionalityEntry
        {
            FunctionalityType funcType;
            uint32_t controlType;
        };

        struct ParsedInput
        {
            std::string type;      // to id1
            std::string connector; // to id2

            uint32_t id1 = 0;
            uint32_t id2 = 0;

            uint32_t controllType = UNDEFINED;
            FunctionalityType funcType = FunctionalityType::_UNDEFINED_;

            bool valid = false;
        };

        class ModelInspector
        {
        public:
            ModelInspector() = default;
            void CheckMeta(Az::Model &model);
            void CheckMultipleModelsMeta(std::vector<Az::Model *> models);
            inline std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *GetData() { return &m_Data; }
            inline std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> *GetPickableObjects() { return &m_PickableObjects; }

        private:
            ParsedInput m_ParseMeta(Az::Mesh &mesh);

            // if returns true, then it means that the mesh is not pickable
            ParsedInput m_ParseForPickable(Az::Mesh &mesh);

            FunctionalityEntry m_ParseType(const std::string &str);

        private:
            std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> m_Data;
            std::vector<std::pair<Az::Mesh *, Crd::MdIsp::ParsedInput>> m_PickableObjects;
        };
    }
}