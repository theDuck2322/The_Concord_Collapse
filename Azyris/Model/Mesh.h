#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <graphics/Shader.h>
#include <Utils/Shapes.h>

#define MAX_BONE_INFLUENCE 4

namespace Az
{
    struct Vertex
    {
        glm::vec3 Position{};
        glm::vec3 Normal{};
        glm::vec2 TexCoords{};
        glm::vec3 Tangent{};
        glm::vec3 Bitangent{};

        int BoneIDs[MAX_BONE_INFLUENCE]{};
        float Weights[MAX_BONE_INFLUENCE]{};
    };

    /////////////////////
    // make a format name like type + _ + ID + _ + connector or
    // btn_1_d1
    // btn_1_dr

    ////////////

    struct sTexture
    {
        unsigned int id;  // OpenGL texture ID
        std::string type; // "texture_diffuse", "texture_normal", etc.
        std::string path; // for deduplication
        bool hasAlpha;
    };

    class Mesh
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<sTexture> textures;

        glm::mat4 modelMatrix{1.0f};

        bool hasTransparency = false;
        glm::vec4 diffuseColor{1.0f};
        bool hasDiffuseColor = false;

        std::string nodeName;

        Mesh(std::vector<Vertex> v,
             std::vector<unsigned int> i,
             std::vector<sTexture> t,
             bool transparent,
             const glm::vec4 &diffuse,
             bool hasDiffuse);

        glm::vec3 GetLocalAABBMin() const;
        glm::vec3 GetLocalAABBMax() const;
        glm::vec3 GetLocalHalfExtents() const;

        OBB GetOBB_Mat4(glm::mat4 *overrideModel = nullptr) const;

        void Draw(Shader &shader, const glm::mat4 *overrideModel = nullptr) const;
        glm::vec3 GetLocalCenter() const { return localCenter; }
        inline glm::vec3 GetCenter(glm::mat4 *overrideTransform = nullptr) const
        {
            if (overrideTransform)
            {
                return glm::vec3(*overrideTransform * glm::vec4(localCenter, 1.0f));
            }
            else
            {
                return glm::vec3(modelMatrix * glm::vec4(localCenter, 1.0f));
            }
        }
        glm::vec3 GetPivot() const { return pivot; }
        glm::vec3 GetPivotFromBlender() const { return glm::vec3(pivot.x, pivot.z, pivot.y); }
        inline void SetPivot(const glm::vec3 &pv) { pivot = pv; }

    private:
        unsigned int VAO = 0;
        unsigned int VBO = 0;
        unsigned int EBO = 0;

        glm::vec3 center{0.0f};
        glm::vec3 localCenter{0.0f};

        // WORLD SPACE PIVOT
        glm::vec3 pivot{0.0f};

        void setup();
    };
}
