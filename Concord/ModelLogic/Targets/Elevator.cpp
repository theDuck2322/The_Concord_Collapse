#include <Elevator.h>

namespace Crd
{
    namespace MdLogic
    {
        void Elevator::SetMesh(Az::Mesh *mesh)
        {
            if (!mesh)
            {
                std::cout << "Mesh is nullptr" << std::endl;
                return;
            }
            m_Mesh = mesh;
            m_OriginalModel = mesh->localMatrix;

            ParseMeshName();

            Update(false);
        }

        void Elevator::Update(bool firstCheck)
        {
        }

        bool Elevator::ParseMeshName()
        {
            const std::string &name = m_Mesh->nodeName;

            size_t open = name.find('(');
            size_t close = name.find(')', open);

            if (open == std::string::npos || close == std::string::npos || close <= open + 1)
                return false;

            std::string inside = name.substr(open + 1, close - open - 1);
            std::stringstream ss(inside);

            float x, y, z;
            char c1, c2;

            if (!(ss >> x >> c1 >> z >> c2 >> y))
                return false;

            if (c1 != ',' || c2 != ',')
                return false;

            ss >> std::ws;
            if (!ss.eof())
                return false;

            m_EndPoint = {x, y, z};
            return true;
        }

        glm::vec3 Elevator::ExtractScale(const glm::mat4 &matrix) const
        {
            return glm::vec3();
        }
        glm::mat4 Elevator::RemoveScale(const glm::mat4 &matrix, const glm::vec3 &scale) const
        {
            return glm::mat4();
        }
        glm::mat4 Elevator::ComputeTransform(float angle) const
        {
            return glm::mat4();
        }
        void Elevator::ApplyTransformToPhysics(const glm::mat4 &transform)
        {
        }
        void Elevator::Animate(float angle)
        {
        }
    }
}