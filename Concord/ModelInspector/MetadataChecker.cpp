#include <MetadataChecker.h>

namespace Crd
{
    namespace MdIsp
    {
        void ModelInspector::CheckMeta(Az::Model &model)
        {
            for (auto &mesh : model.meshes)
            {
                ParsedInput result = m_ParseMeta(mesh);
                if (result.valid == true)
                {
                    m_Data.push_back(std::make_pair(&mesh, result));
                }
                else
                {
                    result = ParsedInput();
                    result = m_ParseForPickable(mesh);
                    if (result.valid == true)
                        m_PickableObjects.push_back(std::make_pair(&mesh, result));
                    else
                        continue;
                }
            }
        }
        void ModelInspector::CheckMultipleModelsMeta(std::vector<Az::Model *> models)
        {
            for (auto model : models)
            {
                for (auto &mesh : model->meshes)
                {
                    ParsedInput result = m_ParseMeta(mesh);
                    if (result.valid == true)
                    {
                        m_Data.push_back(std::make_pair(&mesh, result));
                    }
                    else
                    {
                        result = ParsedInput();
                        result = m_ParseForPickable(mesh);
                        if (result.valid == true)
                            m_PickableObjects.push_back(std::make_pair(&mesh, result));
                        else
                            continue;
                    }
                }
            }
        }
        ParsedInput ModelInspector::m_ParseMeta(Az::Mesh &mesh)
        {
            ParsedInput result;
            std::stringstream ss(mesh.nodeName);
            std::string id1s, id2s;

            if (!(std::getline(ss, result.type, '_') &&
                  std::getline(ss, id1s, '_')))
            {
                return result;
            }

            auto entry = m_ParseType(result.type);
            result.funcType = entry.funcType;
            result.controllType = entry.controlType;
            if (result.funcType != FunctionalityType::_UNDEFINED_)
            {
                result.valid = true;
            }
            else
            {
                std::cout << "Functionality of mesh: " << mesh.nodeName << " is invalid" << std::endl;
            }

            if (result.controllType == (uint32_t)CONTROLLER)
            {
                if (!(std::getline(ss, result.connector, '_') &&
                      std::getline(ss, id2s, '_')))
                {
                    return result;
                }
            }

            try
            {
                result.id1 = static_cast<uint32_t>(std::stoul(id1s));
                result.id2 = id2s.empty() ? 0 : static_cast<uint32_t>(std::stoul(id2s));
            }
            catch (...)
            {
                return ParsedInput{}; // empty
            }

            return result;
        }

        ParsedInput ModelInspector::m_ParseForPickable(Az::Mesh &mesh)
        {
            ParsedInput result;
            std::stringstream ss(mesh.nodeName);
            std::string name, id1s;

            if (!(std::getline(ss, result.type, '_') &&
                  std::getline(ss, id1s, '_')))
            {
                return result; // not pickable
            }

            if (result.type != "pickable")
            {
                return result; // not pickable
            }
            else
                result.valid = true;

            result.id1 = static_cast<uint32_t>(std::stoul(id1s));

            return result;
        }

        FunctionalityEntry ModelInspector::m_ParseType(const std::string &str)
        {
            static const std::unordered_map<std::string, FunctionalityEntry> funcMap = {
                {"btn", {FunctionalityType::BUTTON, CONTROLLER}},
                {"swt", {FunctionalityType::SWITCH, CONTROLLER}},
                {"sdor", {FunctionalityType::SIMPLE_DOOR, CONTROLLER}},
                {"ssdor", {FunctionalityType::SIMPLE_SLIDE_DOOR, CONTROLLER}},
                {"lvr", {FunctionalityType::LEVER, CONTROLLER}},
                {"dor", {FunctionalityType::DOOR, CONTROLLED}},
                {"sld", {FunctionalityType::SLIDE_DOOR, CONTROLLED}},
                {"rot", {FunctionalityType::ROTARY, CONTROLLED}},
                {"elv", {FunctionalityType::ELEVATOR, CONTROLLED}},
                {"brg", {FunctionalityType::BRIDGE, CONTROLLED}},
                {"gte", {FunctionalityType::GATE, CONTROLLED}}};

            auto it = funcMap.find(str);
            if (it != funcMap.end())
                return it->second;

            return {FunctionalityType::_UNDEFINED_, UNDEFINED};
        }
    }
}