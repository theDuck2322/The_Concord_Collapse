#pragma once
#include <glm/glm.hpp>
#include <Model/Model.h>
#include <graphics/Shader.h>
#include <Utils/Shapes.h>

#include <vector>
#include <utility>

namespace Az
{
    class Renderer
    {
    public:
        void SetShader(Az::Shader *shader);
        void SetFrustumPtr(Az::Frustum *frustumPtr);

        // use this function every frame
        void AddModel(Az::Model *model, glm::mat4 *overrideMatrix = nullptr);
        // this function removes all the pointers after draw
        void Draw(const glm::vec3 &cameraPosition);

    private:
        // m_FirstPass function renders all the opaque meshes and selects the transparents one
        // and adds them to 'm_TransparentMeshes'
        void m_FirstPass();
        // m_TransparentPass functions sorts all the transparent meshes acording to distance
        // between it and camera position, then it draws
        void m_TransparentPass(const glm::vec3 &cameraPosition);

    private:
        std::vector<std::pair<Az::Model *, glm::mat4 *>> m_Models;
        std::vector<std::pair<Az::Mesh *, glm::mat4 *>> m_TransparentMeshes;

        Az::Frustum *m_Frustum;
        Az::Shader *m_Shader;
    };
}
