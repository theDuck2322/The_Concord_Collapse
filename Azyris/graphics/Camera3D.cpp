#include "Camera3D.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace Az
{
    Camera3D::Camera3D(float fov, float aspectRatio, float nearClip, float farClip)
    {
        SetProjection(fov, aspectRatio, nearClip, farClip);
        RecalculateViewMatrix();
    }

    void Camera3D::SetProjection(float fov, float aspectRatio, float nearClip, float farClip)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        GenerateFrustum();
        // RecalculateViewMatrix();
    }

    void Camera3D::MoveForward(float delta)
    {
        m_Position += m_Front * delta;
        RecalculateViewMatrix();
    }

    void Camera3D::MoveRight(float delta)
    {
        m_Position += m_Right * delta;
        RecalculateViewMatrix();
    }

    void Camera3D::MoveUp(float delta)
    {
        m_Position += m_Up * delta;
        RecalculateViewMatrix();
    }

    void Camera3D::Rotate(float yawOffset, float pitchOffset)
    {
        m_Yaw += yawOffset;
        m_Pitch += pitchOffset;

        // Constrain pitch (avoid gimbal flip)
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;

        UpdateVectors();
        RecalculateViewMatrix();
    }

    void Camera3D::SetRotation(float yaw, float pitch)
    {
        m_Yaw = yaw;
        m_Pitch = pitch;
        m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
        UpdateVectors();
        RecalculateViewMatrix();
    }

    void Camera3D::UpdateVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);

        m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    void Camera3D::GenerateFrustum()
    {
        // Extract planes from view-projection matrix
        glm::mat4 matrix = glm::transpose(m_ViewProjectionMatrix);

        // Left plane
        m_Frustum.leftFace.normal.x = matrix[0][3] + matrix[0][0];
        m_Frustum.leftFace.normal.y = matrix[1][3] + matrix[1][0];
        m_Frustum.leftFace.normal.z = matrix[2][3] + matrix[2][0];
        m_Frustum.leftFace.distance = matrix[3][3] + matrix[3][0];

        // Right plane
        m_Frustum.rightFace.normal.x = matrix[0][3] - matrix[0][0];
        m_Frustum.rightFace.normal.y = matrix[1][3] - matrix[1][0];
        m_Frustum.rightFace.normal.z = matrix[2][3] - matrix[2][0];
        m_Frustum.rightFace.distance = matrix[3][3] - matrix[3][0];

        // Bottom plane
        m_Frustum.bottomFace.normal.x = matrix[0][3] + matrix[0][1];
        m_Frustum.bottomFace.normal.y = matrix[1][3] + matrix[1][1];
        m_Frustum.bottomFace.normal.z = matrix[2][3] + matrix[2][1];
        m_Frustum.bottomFace.distance = matrix[3][3] + matrix[3][1];

        // Top plane
        m_Frustum.topFace.normal.x = matrix[0][3] - matrix[0][1];
        m_Frustum.topFace.normal.y = matrix[1][3] - matrix[1][1];
        m_Frustum.topFace.normal.z = matrix[2][3] - matrix[2][1];
        m_Frustum.topFace.distance = matrix[3][3] - matrix[3][1];

        // Near plane
        m_Frustum.nearFace.normal.x = matrix[0][3] + matrix[0][2];
        m_Frustum.nearFace.normal.y = matrix[1][3] + matrix[1][2];
        m_Frustum.nearFace.normal.z = matrix[2][3] + matrix[2][2];
        m_Frustum.nearFace.distance = matrix[3][3] + matrix[3][2];

        // Far plane
        m_Frustum.farFace.normal.x = matrix[0][3] - matrix[0][2];
        m_Frustum.farFace.normal.y = matrix[1][3] - matrix[1][2];
        m_Frustum.farFace.normal.z = matrix[2][3] - matrix[2][2];
        m_Frustum.farFace.distance = matrix[3][3] - matrix[3][2];

        // Normalize all planes
        auto normalizePlane = [](Plane &plane)
        {
            float length = glm::length(plane.normal);
            plane.normal /= length;
            plane.distance /= length;
        };

        normalizePlane(m_Frustum.leftFace);
        normalizePlane(m_Frustum.rightFace);
        normalizePlane(m_Frustum.bottomFace);
        normalizePlane(m_Frustum.topFace);
        normalizePlane(m_Frustum.nearFace);
        normalizePlane(m_Frustum.farFace);

        CalculateFrustumCorners();
    }

    void Camera3D::CalculateFrustumCorners()
    {
        // Inverse of view-projection matrix to go from NDC to world space
        glm::mat4 invViewProj = glm::inverse(m_ViewProjectionMatrix);

        // NDC cube corners (x, y, z in [-1, 1])
        std::array<glm::vec4, 8> ndcCorners = {
            // Near plane
            glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // bottom-left-near
            glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),  // bottom-right-near
            glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),   // top-right-near
            glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),  // top-left-near
            // Far plane
            glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), // bottom-left-far
            glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),  // bottom-right-far
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),   // top-right-far
            glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f)   // top-left-far
        };

        // Transform to world space
        for (int i = 0; i < 8; ++i)
        {
            glm::vec4 worldPos = invViewProj * ndcCorners[i];
            m_Frustum.corners[i] = glm::vec3(worldPos) / worldPos.w; // Perspective divide
        }
    }

    glm::quat Camera3D::GetQuaternion()
    {
        glm::mat4 rotMat(1.0f);

        // Column-major order for GLM
        rotMat[0] = glm::vec4(m_Right, 0.0f);  // X-axis
        rotMat[1] = glm::vec4(m_Up, 0.0f);     // Y-axis
        rotMat[2] = glm::vec4(-m_Front, 0.0f); // Z-axis, negative for OpenGL look direction
        rotMat[3] = glm::vec4(0, 0, 0, 1);
        return glm::quat_cast(rotMat);
    }

    void Camera3D::RecalculateViewMatrix()
    {
        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        GenerateFrustum();
    }

}
