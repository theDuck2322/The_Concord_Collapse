#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <stb_image.h>

namespace Az
{
    class Texture
    {
    public:
        Texture() = default;

        bool loadFromFile(const std::string &filePath);
        void Bind(uint32_t slot = 0);
        void Unbind();
        void Delete();

        inline glm::vec2 GetSize() { return glm::vec2(m_Width, m_Height); }
        inline bool textureIsLoaded() { return m_TextureIsLoaded; }

        inline uint32_t GetID() { return m_TextureID; }

    private:
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_TextureID;
        bool m_TextureIsLoaded = false;
    };
}