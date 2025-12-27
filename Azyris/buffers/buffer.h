#pragma once
#include <glad/glad.h>
#include <cstdint>

namespace Az
{

    class VBO
    {
    public:
        VBO() = default;
        VBO(const float *vertices, int64_t size, GLenum usage = GL_STATIC_DRAW);

        void Bind() const;
        void SetData(int64_t size, const void *data);
        void Clear() const;
        void Unbind() const;
        void Delete();

    private:
        uint32_t m_ID; // VBO ID
    };

    class EBO
    {
    public:
        EBO() = default;
        EBO(uint32_t *indices, int64_t size, GLenum usage = GL_STATIC_DRAW);

        void Bind();
        void SetData(int64_t size, const void *data);
        void Clear();
        void Unbind();
        void Delete();

    private:
        uint32_t m_ID;
    };

    class VAO
    {
    public:
        VAO() = default;
        void Init();
        // Links a VBO to the VAO with configurable layout
        void LinkVBO(const VBO &vbo, uint32_t layout, int32_t size, GLenum type, GLboolean normalized, int32_t stride, void *offset);

        void Bind() const;
        void Unbind() const;
        void Delete();

    private:
        uint32_t m_ID; // VAO ID
    };

} // namespace Az
