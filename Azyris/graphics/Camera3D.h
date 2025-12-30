#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Utils/Shapes.h>

namespace Az
{
    class Camera3D
    {
    public:
        Camera3D() = default;
        Camera3D(float fov, float aspectRatio, float nearClip, float farClip);

        void SetProjection(float fov, float aspectRatio, float nearClip, float farClip);

        // Movement
        void MoveForward(float delta);
        void MoveRight(float delta);
        void MoveUp(float delta);

        void Rotate(float yawOffset, float pitchOffset);
        void SetRotation(float yaw, float pitch);

        float GetYaw() const { return m_Yaw; }
        float GetPitch() const { return m_Pitch; }

        // Access
        glm::vec3 &GetPosition() { return m_Position; }
        void SetPosition(const glm::vec3 &position)
        {
            m_Position = position;
            RecalculateViewMatrix();
        }

        inline Frustum *GetFrustum() { return &m_Frustum; }

        const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4 &GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        glm::mat4 *GetViewProjectionPtr() { return &m_ViewProjectionMatrix; }

        glm::vec3 GetForward() const { return m_Front; }
        glm::vec3 GetRight() const { return m_Right; }
        glm::vec3 GetUp() const { return m_Up; }

        glm::vec3 GetRayForward(float length = 100.0f) const
        {
            // Flip Z if needed
            return m_Position + glm::vec3(m_Front.x, m_Front.y, -m_Front.z) * length;
        }

        glm::quat GetQuaternion();

    private:
        void
        RecalculateViewMatrix();
        void UpdateVectors();

        void GenerateFrustum();
        void CalculateFrustumCorners();

    private:
        glm::mat4 m_ViewMatrix{1.0f};
        glm::mat4 m_ProjectionMatrix{1.0f};
        glm::mat4 m_ViewProjectionMatrix{1.0f};

        glm::vec3 m_Position{0.0f};
        glm::vec3 m_Front{0.0f, 0.0f, -1.0f};
        glm::vec3 m_Up{0.0f, 1.0f, 0.0f};
        glm::vec3 m_Right{1.0f, 0.0f, 0.0f};

        float m_Yaw = -90.0f;
        float m_Pitch = 0.0f;

        Frustum m_Frustum;
    };

}
