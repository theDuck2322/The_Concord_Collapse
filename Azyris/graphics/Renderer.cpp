#include "Renderer.h"

namespace Az
{
    void Renderer::SetShader(Az::Shader *shader)
    {
        m_Shader = shader;
    }
    void Renderer::SetFrustumPtr(Az::Frustum *frustumPtr)
    {
        m_Frustum = frustumPtr;
    }
    void Renderer::AddMesh(Az::Mesh *mesh, glm::mat4 *overrideMatrix)
    {
        m_Meshes.push_back(std::make_pair(mesh, overrideMatrix));
    }
    void Renderer::AddModel(Az::Model *model, glm::mat4 *overrideMatrix)
    {
        m_Models.push_back(std::make_pair(model, overrideMatrix));
    }

    void Renderer::Draw(const glm::vec3 &cameraPosition)
    {
        // if (m_Frustum == nullptr || m_Shader == nullptr)
        //{
        //     std::cout << "Frustum pointer or shader pointer are nullptr" << std::endl;
        //}

        m_FirstPass();
        m_TransparentPass(cameraPosition);
        m_TransparentMeshes.clear();
        m_Models.clear();
        m_Meshes.clear();
    }

    void Renderer::m_FirstPass()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        for (auto &pair : m_Models)
        {
            auto model = pair.first;
            auto matrix = pair.second;

            for (auto &mesh : model->meshes)
            {
                if (mesh.hasTransparency)
                {
                    m_TransparentMeshes.push_back(std::make_pair(&mesh, matrix));
                }
                else
                {
                    // if (TestOBBFrustum(mesh.GetOBB_Mat4(matrix), *m_Frustum))
                    {
                        mesh.Draw(*m_Shader, matrix);
                    }
                }
            }
        }

        for (auto &pair : m_Meshes)
        {
            auto mesh = pair.first;
            auto matrix = pair.second;
            if (mesh->hasTransparency)
            {
                m_TransparentMeshes.push_back(std::make_pair(mesh, matrix));
            }
            else
            {
                mesh->Draw(*m_Shader, matrix);
            }
        }
    }
    void Renderer::m_TransparentPass(const glm::vec3 &cameraPosition)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        std::vector<std::pair<float, std::pair<Az::Mesh *, glm::mat4 *>>> transparentMeshWdistance;

        for (auto &pair : m_TransparentMeshes)
        {
            auto mesh = pair.first;
            glm::vec3 meshCenter = mesh->GetCenter(pair.second);
            float distance = glm::length(cameraPosition - meshCenter);
            transparentMeshWdistance.push_back(std::make_pair(distance, std::make_pair(mesh, pair.second)));
        }

        std::sort(transparentMeshWdistance.begin(), transparentMeshWdistance.end(),
                  [](const auto &a, const auto &b)
                  {
                      return a.first > b.first;
                  });

        for (auto &item : transparentMeshWdistance)
        {
            auto mesh = item.second.first;
            auto matrix = item.second.second;
            // if (TestOBBFrustum(mesh->GetOBB_Mat4(matrix), *m_Frustum))
            {
                mesh->Draw(*m_Shader, matrix);
            }
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}
