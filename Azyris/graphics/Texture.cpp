#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include <iostream>

namespace Az
{
    bool Texture::loadFromFile(const std::string &filePath)
    {
        if (m_TextureIsLoaded)
        {
            this->Delete();
        }

        stbi_set_flip_vertically_on_load(false); // Flip image vertically if needed

        int width, height, channels;
        unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
        if (!data)
        {
            std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
            return EXIT_FAILURE;
        }

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

        m_Width = width;
        m_Height = height;

        stbi_image_free(data);
        m_TextureIsLoaded = true;

        std::cout << "Texture: " << filePath << " is loaded" << std::endl;

        return EXIT_SUCCESS;
    }

    void Texture::Bind(uint32_t slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }

    void Texture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Delete()
    {
        if (m_TextureIsLoaded)
        {
            m_Width = 0;
            m_Height = 0;
            m_TextureIsLoaded = false;
            glDeleteTextures(1, &m_TextureID);
        }
    }
}