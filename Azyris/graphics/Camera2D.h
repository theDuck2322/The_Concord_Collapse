#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Az
{
    class Camera2D
    {
    public:
        Camera2D() = default;
        Camera2D(float left, float right, float bottom, float top);

        void SetSize(float width, float height);

        glm::vec3 GetSize() { return m_Size; }

        glm::vec3 &GetPosition() { return m_Position; }
        void SetPosition(const glm::vec3 &position)
        {
            m_Position = position;
            RecalculateViewMatrix();
        }

        double GetRotation() const { return m_Rotation; }
        void SetRotation(double rotation)
        {
            m_Rotation = rotation;
            RecalculateViewMatrix();
        }

        const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4 &GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        inline float GetWidth() { return m_Size.x; }
        inline float GetHeight() { return m_Size.y; }

    private:
        void RecalculateViewMatrix();

    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_Position;

        glm::vec3 m_Size;

        double m_Rotation;
    };
}