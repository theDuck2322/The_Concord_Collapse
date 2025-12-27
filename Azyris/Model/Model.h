#pragma once

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <Model/Mesh.h>

namespace Az
{
    unsigned int TextureFromFile(const char *path, bool &usesTransparency);
    unsigned int TextureFromEmbedded(const aiTexture *tex, bool &usesTransparency);

    class Model
    {
    public:
        Model() = default;
        Model(const std::string &path);
        void load(const std::string &path);

        // because of how gltf has a model matrix for each mesh (local position or stuff like that)
        // static objects needs to have use their own model matrix (already loaded) and
        // dynamic models like players, objects etc, needs to override their model
        // thats because in the Mesh::Draw(Shader &shader, const glm::mat4 *overrideModel = nullptr)
        // the model matrix is already overwritten via: "use your own or the override model?"
        // SO do not try to use Shader::setuniform(model) because its useles if u plan to use this
        // Draw function :))
        void Draw(Shader &shader, const glm::mat4 *overrideModel = nullptr) const;

        std::vector<Mesh> meshes;

    private:
        Assimp::Importer importer;
        const aiScene *scene = nullptr;

        std::vector<sTexture> loadedTextures;

        void processNode(aiNode *node, const glm::mat4 &parent);
        Mesh processMesh(aiMesh *mesh, aiNode *node, const glm::mat4 &transform);

        std::vector<sTexture> loadTextures(aiMaterial *mat,
                                           aiTextureType type,
                                           const std::string &name);
    };
}
