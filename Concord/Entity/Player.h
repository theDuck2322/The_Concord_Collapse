#pragma once
#include <graphics/Camera3D.h>
#include <core/Gamepad.h>
#include <PhysicsManager/PhysicsManager.h>
#include <glm/glm.hpp>

namespace Crd
{
    class Player
    {
    public:
        void Init(Az::Camera3D *camera, Az::physx::PhysicsManager *manager);
        void Update();

        // Get player position for camera
        glm::vec3 GetHeadPosition() const;

        inline Az::Gamepad *GetGamepad() { return &m_Gamepad; }
        inline btRigidBody *GetRigidbody() { return m_Body; }

    private:
        void m_ProcessInputGamepad();
        void m_ProcessInputKeyboard();
        void m_UpdateCameraFromBody();
        void m_GroundCheck();
        void m_ApplyMovement();

    private:
        bool m_EnableExtraSpeed = false;

        float m_WalkSpeed = 5.0f;
        float m_RunSpeed = 2 * m_WalkSpeed;

        float m_Speed = 5.0f;

        float m_GpadSensitivity = 140.0f;
        float m_MouseSensitivity = 20.0f;
        float m_JumpForce = 10.0f;
        float m_CameraHeight = 1.8f; // Camera at eye level (1.8m)

        Az::Gamepad m_Gamepad;
        Az::physx::PhysicsManager *m_PhysicsManager;
        btRigidBody *m_Body;
        Az::Camera3D *m_CameraPtr;

        // Input accumulation for physics
        glm::vec3 m_MovementInput = glm::vec3(0.0f);
        float m_Yaw = 0.0f;
        float m_Pitch = 0.0f;

        // Ground check
        btVector3 m_GroundNormal = btVector3(0, 1, 0);
        bool m_IsGrounded = false;
        float m_GroundCheckDistance = 2.1f;
        float m_GroundedTime = 0.0f;
        float m_MinGroundedTime = 0.05f;
        float m_CoyoteTime = 0.15f;
    };
}