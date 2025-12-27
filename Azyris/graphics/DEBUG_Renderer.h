#pragma once

#include <Shader.h>
#include <buffer.h>
#include <Utils/DrawCallCounter.h>
#include <Utils/Shapes.h>

namespace Az
{

    struct vtx
    {
        glm::vec3 pos;
    };

    class DEBUG_Renderer // line drawer ?
    {
    public:
        DEBUG_Renderer() = default;
        void INIT_RENDERER(Az::Shader *shader, glm::mat4 *viewProj);
        void SetShader(Az::Shader *shader);

        void Begin();
        void AddVertex(const glm::vec3 &a);
        void AddLine(const glm::vec3 &a, const glm::vec3 &b);
        void AddAABB(const glm::vec3 &min, const glm::vec3 &max);
        void AddOBB(const Az::OBB &obb);
        void AddFrustum(const Az::Frustum &frustum);
        void AddLines(glm::vec3 *a, uint32_t size);

        void Draw();

    private:
        Az::Shader *m_Shader = nullptr;
        bool m_CanDraw = false; // this bool checks if shader is nullptr
        Az::VAO m_Vao;
        Az::VBO m_Vbo;
        // Az::EBO m_Ebo;

        std::vector<vtx> m_Vertices;

    private:
        const uint32_t MAX_VERTICES = 32768;
        glm::mat4 *m_ViewProj;
    };

    inline glm::vec3 tA, tB;
}