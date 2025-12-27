#include <DEBUG_Renderer.h>
#include <DrawCallCounter.h>

namespace Az
{
    void DEBUG_Renderer::INIT_RENDERER(Az::Shader *shader, glm::mat4 *viewProj)
    {
        SetShader(shader);
        m_ViewProj = viewProj;
        m_Vao.Init();
        m_Vao.Bind();
        m_Vbo = Az::VBO(nullptr, sizeof(vtx) * MAX_VERTICES, GL_DYNAMIC_DRAW);
        m_Vao.LinkVBO(m_Vbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(vtx), (void *)0);
        m_Vao.Unbind();
    }

    void DEBUG_Renderer::SetShader(Az::Shader *shader)
    {
        m_Shader = shader;
        m_CanDraw = (shader != nullptr);
    }
    void DEBUG_Renderer::Begin()
    {
        glLineWidth(4);
        m_Vertices.clear();
    }
    void DEBUG_Renderer::AddVertex(const glm::vec3 &a)
    {
        if (m_Vertices.size() + 1 >= MAX_VERTICES)
        {
            Draw();
            Begin();
        }

        m_Vertices.push_back({a});
    }
    void DEBUG_Renderer::AddLine(const glm::vec3 &a, const glm::vec3 &b)
    {
        if (m_Vertices.size() + 2 >= MAX_VERTICES)
        {
            Draw();
            Begin();
        }
        m_Vertices.push_back({a});
        m_Vertices.push_back({b});
    }
    void DEBUG_Renderer::AddAABB(const glm::vec3 &min, const glm::vec3 &max)
    {
        if (m_Vertices.size() + 24 >= MAX_VERTICES)
        {
            Draw();
            Begin();
        }

        // 8 corners of the box
        glm::vec3 v[8] = {
            {min.x, min.y, min.z}, // 0
            {max.x, min.y, min.z}, // 1
            {max.x, max.y, min.z}, // 2
            {min.x, max.y, min.z}, // 3
            {min.x, min.y, max.z}, // 4
            {max.x, min.y, max.z}, // 5
            {max.x, max.y, max.z}, // 6
            {min.x, max.y, max.z}  // 7
        };

        // bottom face
        AddLine(v[0], v[1]);
        AddLine(v[1], v[2]);
        AddLine(v[2], v[3]);
        AddLine(v[3], v[0]);

        // top face
        AddLine(v[4], v[5]);
        AddLine(v[5], v[6]);
        AddLine(v[6], v[7]);
        AddLine(v[7], v[4]);

        // vertical edges
        AddLine(v[0], v[4]);
        AddLine(v[1], v[5]);
        AddLine(v[2], v[6]);
        AddLine(v[3], v[7]);
    }

    void DEBUG_Renderer::AddOBB(const Az::OBB &obb)
    {
        // Local-space corners of a unit OBB centered at origin
        const glm::vec3 he = obb.halfExtents;

        glm::vec3 localCorners[8] =
            {
                {-he.x, -he.y, -he.z},
                {he.x, -he.y, -he.z},
                {he.x, he.y, -he.z},
                {-he.x, he.y, -he.z},
                {-he.x, -he.y, he.z},
                {he.x, -he.y, he.z},
                {he.x, he.y, he.z},
                {-he.x, he.y, he.z}};

        // Transform corners to world space
        glm::vec3 worldCorners[8];
        for (int i = 0; i < 8; ++i)
        {
            worldCorners[i] = glm::vec3(obb.transform * glm::vec4(localCorners[i], 1.0f));
        }

        // Bottom face
        AddLine(worldCorners[0], worldCorners[1]);
        AddLine(worldCorners[1], worldCorners[2]);
        AddLine(worldCorners[2], worldCorners[3]);
        AddLine(worldCorners[3], worldCorners[0]);

        // Top face
        AddLine(worldCorners[4], worldCorners[5]);
        AddLine(worldCorners[5], worldCorners[6]);
        AddLine(worldCorners[6], worldCorners[7]);
        AddLine(worldCorners[7], worldCorners[4]);

        // Vertical edges
        AddLine(worldCorners[0], worldCorners[4]);
        AddLine(worldCorners[1], worldCorners[5]);
        AddLine(worldCorners[2], worldCorners[6]);
        AddLine(worldCorners[3], worldCorners[7]);
    }

    void DEBUG_Renderer::AddFrustum(const Az::Frustum &frustum)
    {
        const auto &corners = frustum.corners;

        // Near plane rectangle
        AddLine(corners[0], corners[1]); // bottom near
        AddLine(corners[1], corners[2]); // right near
        AddLine(corners[2], corners[3]); // top near
        AddLine(corners[3], corners[0]); // left near

        // Far plane rectangle
        AddLine(corners[4], corners[5]); // bottom far
        AddLine(corners[5], corners[6]); // right far
        AddLine(corners[6], corners[7]); // top far
        AddLine(corners[7], corners[4]); // left far

        // Connect near and far planes
        AddLine(corners[0], corners[4]); // bottom-left
        AddLine(corners[1], corners[5]); // bottom-right
        AddLine(corners[2], corners[6]); // top-right
        AddLine(corners[3], corners[7]); // top-left
    }

    void DEBUG_Renderer::AddLines(glm::vec3 *a, uint32_t size)
    {
        for (uint32_t i = 0; i < size; i++)
        {
            AddVertex(a[i]);
        }
    }

    void DEBUG_Renderer::Draw()
    {
        if (!m_CanDraw || m_Vertices.empty())
            return;

        m_Shader->use();
        m_Shader->setUniform("viewProj", *m_ViewProj);

        m_Vao.Bind();
        m_Vbo.Bind();

        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        m_Vertices.size() * sizeof(vtx),
                        m_Vertices.data());
        Az::DrawArrays(GL_LINES, 0, m_Vertices.size());
        m_Vbo.Unbind();
        m_Vao.Unbind();
    }
}