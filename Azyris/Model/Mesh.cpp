#include "Mesh.h"
#include <glad/glad.h>
#include <Utils/Helpers.h>
#include <Utils/DrawCallCounter.h>
namespace Az
{
    Mesh::Mesh(std::vector<Vertex> v,
               std::vector<unsigned int> i,
               std::vector<sTexture> t,
               bool transparent,
               const glm::vec4 &diffuse,
               bool hasDiffuse)
        : vertices(std::move(v)),
          indices(std::move(i)),
          textures(std::move(t)),
          hasTransparency(transparent),
          diffuseColor(diffuse),
          hasDiffuseColor(hasDiffuse)
    {
        setup();
    }

    glm::vec3 Mesh::GetLocalAABBMin() const
    {
        if (vertices.empty())
            return glm::vec3(0.0f);

        glm::vec3 min = vertices[0].Position;
        for (const auto &v : vertices)
            min = glm::min(min, v.Position);

        return min;
    }

    glm::vec3 Mesh::GetLocalAABBMax() const
    {
        if (vertices.empty())
            return glm::vec3(0.0f);

        glm::vec3 max = vertices[0].Position;
        for (const auto &v : vertices)
            max = glm::max(max, v.Position);

        return max;
    }

    glm::vec3 Mesh::GetLocalHalfExtents() const
    {
        glm::vec3 min = GetLocalAABBMin();
        glm::vec3 max = GetLocalAABBMax();
        return (max - min) * 0.5f;
    }

    void Mesh::setup()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(Vertex),
                     vertices.data(),
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(unsigned int),
                     indices.data(),
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void *)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

        for (const auto &v : vertices)
            localCenter += v.Position;

        if (!vertices.empty())
            localCenter /= static_cast<float>(vertices.size());

        center = glm::vec3(modelMatrix * glm::vec4(localCenter, 1.0f));

        glBindVertexArray(0);
    }

    OBB Mesh::GetOBB_Mat4(glm::mat4 *overrideModel) const
    {
        glm::vec3 localMin = GetLocalAABBMin();
        glm::vec3 localMax = GetLocalAABBMax();

        glm::vec3 localCenter = (localMin + localMax) * 0.5f;
        glm::vec3 halfExtents = (localMax - localMin) * 0.5f;

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), localCenter);

        glm::mat4 obbTransform = glm::mat4(1.0f);
        if (overrideModel)
        {
            obbTransform = *overrideModel * translation;
        }
        else
        {
            obbTransform = modelMatrix * translation;
        }

        return {obbTransform, halfExtents};
    }

    void Mesh::Draw(Shader &shader, const glm::mat4 *overrideModel) const
    {
        shader.use();

        unsigned int unit = 0;
        for (const auto &tex : textures)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, tex.id);
            shader.setUniform(tex.type, static_cast<int>(unit));
            ++unit;
        }

        shader.setUniform("useDiffuseTexture", !textures.empty());
        shader.setUniform("hasDiffuseColor", hasDiffuseColor);
        shader.setUniform("materialDiffuseColor", diffuseColor);

        if (overrideModel)
            shader.setUniform("model", *overrideModel);
        else
            shader.setUniform("model", modelMatrix);

        glBindVertexArray(VAO);

        Az::DrawElements(GL_TRIANGLES,
                         static_cast<GLsizei>(indices.size()),
                         GL_UNSIGNED_INT,
                         nullptr);

        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

}
