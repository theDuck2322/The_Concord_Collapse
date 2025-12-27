#include "Camera2D.h"

namespace Az
{
    Camera2D::Camera2D(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -100.0f, 100.0f)), m_ViewMatrix(1.0f)
    {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_Position = glm::vec3(0);
        m_Rotation = 0;

        m_Size = glm::vec3(right, top, 0);
    }

    void Camera2D::SetSize(float width, float height)
    {
        m_ProjectionMatrix = glm::ortho(0.0f, width, 0.0f, height, -100.0f, 100.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_Size = glm::vec3(width, height, 0);
    }

    void Camera2D::RecalculateViewMatrix()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                              glm::rotate(glm::mat4(1.0f), (float)m_Rotation, glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}