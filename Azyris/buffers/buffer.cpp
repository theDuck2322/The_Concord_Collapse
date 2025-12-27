#include <buffer.h>

namespace Az
{
    ////////////    VBO     //////////////////////

    VBO::VBO(const float *vertices, int64_t size, GLenum usage)
    {
        glGenBuffers(1, &m_ID);
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
    }

    void VBO::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    }

    void VBO::SetData(int64_t size, const void *data)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    void VBO::Clear() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 0, nullptr); // Clears without reallocating
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VBO::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VBO::Delete()
    {
        glDeleteBuffers(1, &m_ID);
    }

    ////////////    EBO     //////////////////////

    EBO::EBO(uint32_t *indices, int64_t size, GLenum usage)
    {
        glGenBuffers(1, &m_ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, usage);
    }
    void EBO::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    }

    void EBO::SetData(int64_t size, const void *data)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
    }

    void EBO::Clear()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 0, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    void EBO::Unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    void EBO::Delete()
    {
        glDeleteBuffers(1, &m_ID);
    }

    ////////////    VAO     //////////////////////

    void VAO::Init()
    {
        glGenVertexArrays(1, &m_ID);
    }

    void VAO::LinkVBO(const VBO &vbo, uint32_t layout, int32_t size, GLenum type, GLboolean normalized, int32_t stride, void *offset)
    {
        this->Bind();
        vbo.Bind();

        // Configure vertex attribute
        glVertexAttribPointer(layout, size, type, normalized, stride, offset);
        glEnableVertexAttribArray(layout);

        vbo.Unbind();
        this->Unbind();
    }

    void VAO::Bind() const
    {
        glBindVertexArray(m_ID);
    }

    void VAO::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VAO::Delete()
    {
        glDeleteVertexArrays(1, &m_ID);
    }
}