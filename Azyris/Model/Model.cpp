#include "Model.h"
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <iostream>
#include <Helpers.h>
#include <assimp/postprocess.h>

static bool EndsWith(const std::string &value, const std::string &ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

namespace Az
{

    unsigned int TextureFromFile(const char *path, bool &usesTransparency)
    {
        usesTransparency = false;

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

        if (!data)
        {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return 0;
        }

        GLenum format = GL_RGB;
        if (channels == 4)
        {
            format = GL_RGBA;
            for (int i = 0; i < width * height; ++i)
            {
                if (data[i * 4 + 3] < 255)
                {
                    usesTransparency = true;
                    break;
                }
            }
        }

        unsigned int id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexImage2D(GL_TEXTURE_2D, 0, format,
                     width, height, 0,
                     format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return id;
    }

    unsigned int TextureFromEmbedded(const aiTexture *tex, bool &usesTransparency)
    {
        usesTransparency = false;
        stbi_set_flip_vertically_on_load(true); // Flip image vertically if needed

        int width, height, channels;
        unsigned char *data = stbi_load_from_memory(
            reinterpret_cast<const unsigned char *>(tex->pcData),
            tex->mWidth,
            &width, &height, &channels, 0);

        if (!data)
        {
            std::cerr << "Failed to load embedded texture" << std::endl;
            return 0;
        }

        GLenum format = GL_RGB;
        if (channels == 4)
        {
            format = GL_RGBA;
            for (int i = 0; i < width * height; ++i)
            {
                if (data[i * 4 + 3] < 255)
                {
                    usesTransparency = true;
                    break;
                }
            }
        }

        unsigned int id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexImage2D(GL_TEXTURE_2D, 0, format,
                     width, height, 0,
                     format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return id;
    }

    Model::Model(const std::string &path)
    {
        load(path);
    }

    void Model::Draw(Shader &shader, const glm::mat4 *overrideModel) const
    {
        for (const auto &mesh : meshes)
            mesh.Draw(shader, overrideModel);
    }

    void Model::load(const std::string &path)
    {
        if (!EndsWith(path, ".gltf") && !EndsWith(path, ".glb"))
        {
            throw std::runtime_error("Only glTF / GLB models supported");
        }

        scene = importer.ReadFile(
            path,
            aiProcess_Triangulate |
                aiProcess_CalcTangentSpace |
                aiProcess_JoinIdenticalVertices |
                aiProcess_ValidateDataStructure);

        if (!scene || !scene->mRootNode)
        {
            std::cout << "Path may be incorrect" << std::endl;
            throw std::runtime_error(importer.GetErrorString());
        }

        processNode(scene->mRootNode, glm::mat4(1.0f));
    }

    void Model::processNode(aiNode *node, const glm::mat4 &parent)
    {
        glm::mat4 local = Convert(node->mTransformation);
        glm::mat4 global = parent * local;

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, node, global));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], global);
    }

    Mesh Model::processMesh(aiMesh *mesh, aiNode *node, const glm::mat4 &transform)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<sTexture> textures;

        // --- vertices ---
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex v{};
            v.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

            if (mesh->HasNormals())
                v.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

            if (mesh->mTextureCoords[0])
                v.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

            vertices.push_back(v);
        }

        // --- indices ---
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
                indices.push_back(mesh->mFaces[i].mIndices[j]);

        // --- material / textures ---

        bool hasTransparency = false;
        glm::vec4 diffuseColor(1.0f, 1.0f, 1.0f, 1.0f); // Default to white with full alpha
        bool hasDiffuseColor = false;
        bool hasTexture = false;

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

            // Try loading diffuse textures
            auto diffuseTex = loadTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
            if (!diffuseTex.empty())
            {
                textures.insert(textures.end(), diffuseTex.begin(), diffuseTex.end());
                hasTexture = true;

                // Check for transparency
                for (auto &t : diffuseTex)
                    if (t.hasAlpha)
                        hasTransparency = true;
            }

            aiColor4D color4(1.0f, 1.0f, 1.0f, 1.0f);
            if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color4) == AI_SUCCESS)
            {
                diffuseColor = glm::vec4(color4.r, color4.g, color4.b, color4.a);
                hasDiffuseColor = true;

                // Check if color has transparency
                if (color4.a < 1.0f)
                {
                    hasTransparency = true;
                }
            }
            // Fallback to aiColor3D for compatibility
            else
            {
                aiColor3D color3(1.0f, 1.0f, 1.0f);
                if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color3) == AI_SUCCESS)
                {
                    diffuseColor = glm::vec4(color3.r, color3.g, color3.b, 1.0f);
                    hasDiffuseColor = true;
                }
            }

            // Check for base color factor in glTF (PBR materials)
            // glTF uses _BaseColorFactor which is stored in AI_MATKEY_BASE_COLOR
            aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
            if (mat->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS)
            {
                diffuseColor = glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
                hasDiffuseColor = true;

                if (baseColor.a < 1.0f)
                {
                    hasTransparency = true;
                }
            }
        }

        // --- create mesh ---
        Mesh result(vertices, indices, textures, hasTransparency, diffuseColor, hasDiffuseColor);
        result.modelMatrix = transform;
        result.SetPivot(glm::vec3(transform[3])); // translation column
        result.nodeName = node->mName.C_Str();
        // --- read metadata from node if present ---

        if (result.nodeName.compare(0, 6, "transp") == 0)
        {
            result.hasTransparency = true;
        }

        std::cout << result.nodeName << std::endl;

        return result;
    }

    std::vector<sTexture> Model::loadTextures(
        aiMaterial *mat,
        aiTextureType type,
        const std::string &name)
    {
        std::vector<sTexture> textures;

        for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string path = str.C_Str();

            // 1. Check cache
            bool found = false;
            for (const auto &loaded : loadedTextures)
            {
                if (loaded.path == path)
                {
                    textures.push_back(loaded);
                    found = true;
                    break;
                }
            }

            if (found)
                continue;

            // 2. Load new texture
            sTexture tex{};
            bool alpha = false;

            if (!path.empty() && path[0] == '*')
            {
                const aiTexture *embedded = scene->GetEmbeddedTexture(path.c_str());
                if (!embedded)
                    continue;

                tex.id = TextureFromEmbedded(embedded, alpha);
            }
            else
            {
                tex.id = TextureFromFile(path.c_str(), alpha);
            }

            tex.type = name;
            tex.path = path;
            tex.hasAlpha = alpha;

            textures.push_back(tex);
            loadedTextures.push_back(tex);
        }

        return textures;
    }
}
