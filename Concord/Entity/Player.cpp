#include <Player.h>
#include <Utils/Timer.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <core/Input.h>

namespace Crd
{
    void Player::Init(Az::Camera3D *camera, Az::physx::PhysicsManager *manager)
    {
        m_CameraPtr = camera;
        m_PhysicsManager = manager;
        m_Gamepad.FindGamepad();

        // Create player capsule (better for character than box)
        // Or use box if you prefer
        size_t id = manager->CreateCapsule(1.0f, glm::vec3(0, 10, 0), 0.5, 3);
        m_Body = manager->GetRigidbodyById(id);

        // Configure physics body for player
        m_Body->setFriction(0.5f);
        m_Body->setRestitution(0.0f);              // No bounce
        m_Body->setSleepingThresholds(0.0f, 0.0f); // Never sleep
        m_Body->setAngularFactor(0.0f);            // No rotation
        m_Body->setRollingFriction(0.0f);
        m_PhysicsManager = manager;
        // Set initial camera position
        m_UpdateCameraFromBody();
    }

    void Player::Update()
    {
        if (m_Gamepad.IsConnected())
            m_ProcessInputGamepad();

        m_ProcessInputKeyboard();

        m_GroundCheck();
        m_ApplyMovement();
        m_UpdateCameraFromBody();
        m_HandlePropMovement();
    }

    void Player::m_ProcessInputGamepad()
    {
        if (m_Gamepad.IsConnected())
        {
            m_Gamepad.UpdateButtonStates();
        }
        if (m_Gamepad.GetButtonDown(AZ_GPAD_BUTTON_DPAD_DOWN))
        {
            m_EnableExtraSpeed = !m_EnableExtraSpeed;
        }
        // cheats
        if (m_EnableExtraSpeed)
        {
            m_RunSpeed = 16 * m_WalkSpeed;
        }
        else
            m_RunSpeed = 2 * m_WalkSpeed;

        // Reset movement input
        m_MovementInput = glm::vec3(0.0f);

        // --- Movement (relative to camera direction) ---
        if (abs(m_Gamepad.GetAxis(AZ_GPAD_AXIS_LEFTY)) > 0.3f)
        {
            // Forward/backward relative to camera
            glm::vec3 forward = m_CameraPtr->GetForward();
            forward.y = 0; // Keep movement horizontal
            forward = glm::normalize(forward);
            m_MovementInput += forward * (-m_Gamepad.GetAxis(AZ_GPAD_AXIS_LEFTY));
        }
        if (abs(m_Gamepad.GetAxis(AZ_GPAD_AXIS_LEFTX)) > 0.3f)
        {
            // Right/left relative to camera
            glm::vec3 right = m_CameraPtr->GetRight();
            right.y = 0; // Keep movement horizontal
            right = glm::normalize(right);
            m_MovementInput += right * m_Gamepad.GetAxis(AZ_GPAD_AXIS_LEFTX);
        }

        // Normalize diagonal movement
        if (glm::length(m_MovementInput) > 1.0f)
        {
            m_MovementInput = glm::normalize(m_MovementInput);
        }

        // --- Camera rotation ---
        if (abs(m_Gamepad.GetAxis(AZ_GPAD_AXIS_RIGHTX)) > 0.3f ||
            abs(m_Gamepad.GetAxis(AZ_GPAD_AXIS_RIGHTY)) > 0.3f)
        {
            float deltaTime = Az::Timer::fDeltaTime;

            // Get current camera rotation
            float currentYaw = m_CameraPtr->GetYaw();
            float currentPitch = m_CameraPtr->GetPitch();

            // Apply offsets
            currentYaw += m_Gamepad.GetAxis(AZ_GPAD_AXIS_RIGHTX) * deltaTime * m_GpadSensitivity;
            currentPitch -= m_Gamepad.GetAxis(AZ_GPAD_AXIS_RIGHTY) * deltaTime * m_GpadSensitivity;

            // Clamp
            currentPitch = glm::clamp(currentPitch, -89.0f, 89.0f);

            // Set new rotation
            m_CameraPtr->SetRotation(currentYaw, currentPitch);
        }

        // --- Jump ---
        if (m_Gamepad.GetButton(AZ_GPAD_BUTTON_SOUTH) &&
            (m_IsGrounded || m_GroundedTime <= m_CoyoteTime))
        {
            m_GroundedTime = m_CoyoteTime + 1.0f; // prevent double jump

            // --- Reset vertical velocity ---
            btVector3 vel = m_Body->getLinearVelocity();
            vel.setY(0); // cancel any upward/downward motion
            m_Body->setLinearVelocity(vel);

            // --- Apply jump impulse ---
            btVector3 jumpImpulse;
            if (m_IsGrounded)
            {
                // Jump along ground normal (for slopes)
                jumpImpulse = m_GroundNormal * m_JumpForce;
            }
            else
            {
                // Coyote jump → vertical only
                jumpImpulse = btVector3(0, m_JumpForce, 0);
            }

            m_Body->applyCentralImpulse(jumpImpulse);
        }

        m_Speed = m_WalkSpeed;
        if (m_Gamepad.GetButton(AZ_GPAD_BUTTON_LEFT_STICK))
        {
            m_Speed = m_RunSpeed;
        }

        if (m_IsGrounded && glm::length(m_MovementInput) < 0.1f)
        {
            btVector3 v = m_Body->getLinearVelocity();
            m_Body->setLinearVelocity(btVector3(0, v.y(), 0));
        }
    }

    void Player::m_ProcessInputKeyboard()
    {
        // cheats
        if (Az::Input::GetKeyDown(AZ_L))
        {
            m_EnableExtraSpeed = !m_EnableExtraSpeed;
        }

        if (m_EnableExtraSpeed)
        {
            m_RunSpeed = 16 * m_WalkSpeed;
        }
        else
            m_RunSpeed = 2 * m_WalkSpeed;

        m_MovementInput = glm::vec3(0.0f);

        if (Az::Input::GetKey(AZ_W) && !Az::Input::GetKey(AZ_S))
        {
            // Forward relative to camera
            glm::vec3 forward = m_CameraPtr->GetForward();
            forward.y = 0; // Keep movement horizontal
            forward = glm::normalize(forward);
            m_MovementInput += forward * (float)Az::Input::GetKey(AZ_W);
        }
        if (Az::Input::GetKey(AZ_S) && !Az::Input::GetKey(AZ_W))
        {
            // Backward relative to camera
            glm::vec3 forward = m_CameraPtr->GetForward();
            forward.y = 0; // Keep movement horizontal
            forward = glm::normalize(forward);
            m_MovementInput += forward * -(float)Az::Input::GetKey(AZ_S);
        }
        if (Az::Input::GetKey(AZ_A) && !Az::Input::GetKey(AZ_D))
        {
            // Right/left relative to camera
            glm::vec3 right = m_CameraPtr->GetRight();
            right.y = 0; // Keep movement horizontal
            right = glm::normalize(right);
            m_MovementInput += right * -(float)Az::Input::GetKey(AZ_A);
        }
        if (Az::Input::GetKey(AZ_D) && !Az::Input::GetKey(AZ_A))
        {
            // Right/left relative to camera
            glm::vec3 right = m_CameraPtr->GetRight();
            right.y = 0; // Keep movement horizontal
            right = glm::normalize(right);
            m_MovementInput += right * (float)Az::Input::GetKey(AZ_D);
        }

        // Normalize diagonal movement
        if (glm::length(m_MovementInput) > 1.0f)
        {
            m_MovementInput = glm::normalize(m_MovementInput);
        }

        if (m_CanLook)
        {
            // Mouse rotation
            float deltaTime = Az::Timer::fDeltaTime;
            // Get current camera rotation
            float currentYaw = m_CameraPtr->GetYaw();
            float currentPitch = m_CameraPtr->GetPitch();

            // Apply offsets
            currentYaw += Az::Input::GetMouseDeltaX() * deltaTime * m_MouseSensitivity;
            currentPitch -= Az::Input::GetMouseDeltaY() * deltaTime * m_MouseSensitivity;
            // Clamp
            currentPitch = glm::clamp(currentPitch, -89.0f, 89.0f);
            // Set new rotation
            m_CameraPtr->SetRotation(currentYaw, currentPitch);
        }

        if (Az::Input::GetKey(AZ_SPACE) &&
            (m_IsGrounded || m_GroundedTime <= m_CoyoteTime))
        {
            m_GroundedTime = m_CoyoteTime + 1.0f; // prevent double jump

            // --- Reset vertical velocity ---
            btVector3 vel = m_Body->getLinearVelocity();
            vel.setY(0); // cancel any upward/downward motion
            m_Body->setLinearVelocity(vel);

            // --- Apply jump impulse ---
            btVector3 jumpImpulse;
            if (m_IsGrounded)
            {
                // Jump along ground normal (for slopes)
                jumpImpulse = m_GroundNormal * m_JumpForce;
            }
            else
            {
                // Coyote jump → vertical only
                jumpImpulse = btVector3(0, m_JumpForce, 0);
            }

            m_Body->applyCentralImpulse(jumpImpulse);
        }

        m_Speed = m_WalkSpeed;
        if (Az::Input::GetKey(AZ_LSHIFT))
        {
            m_Speed = m_RunSpeed;
        }

        if (m_IsGrounded && glm::length(m_MovementInput) < 0.1f)
        {
            btVector3 v = m_Body->getLinearVelocity();
            m_Body->setLinearVelocity(btVector3(0, v.y(), 0));
        }
    }

    void Player::m_GroundCheck()
    {
        if (!m_Body)
            return;

        m_IsGrounded = false;
        m_GroundNormal = btVector3(0, 1, 0);

        btVector3 from = m_Body->getWorldTransform().getOrigin();
        btVector3 to = from - btVector3(0, m_GroundCheckDistance, 0);

        btCollisionWorld::ClosestRayResultCallback groundRay(from, to);
        m_PhysicsManager->GetWorld()->rayTest(from, to, groundRay);

        if (groundRay.hasHit())
        {
            btVector3 n = groundRay.m_hitNormalWorld;
            float upDot = n.dot(btVector3(0, 1, 0));

            // CHANGED: Allow steeper slopes for jumping
            const float WALKABLE_SLOPE_ANGLE = 45.0f; // degrees
            const float WALKABLE_SLOPE_DOT = glm::cos(glm::radians(WALKABLE_SLOPE_ANGLE));

            if (upDot > WALKABLE_SLOPE_DOT) // Approximately 0.707 for 45 degrees
            {
                m_IsGrounded = true;
                m_GroundNormal = n;
            }
        }

        float dt = Az::Timer::fDeltaTime;

        if (m_IsGrounded)
        {
            m_GroundedTime = 0.0f;
            m_Body->setFriction(0.5f); // Ground friction for slope
        }
        else
        {
            m_GroundedTime += dt;
            m_Body->setFriction(0.0f); // Let the player slide in air
        }
    }

    void Player::m_ApplyMovement()
    {
        if (!m_Body)
            return;

        btVector3 velocity = m_Body->getLinearVelocity();

        glm::vec3 wishDir = m_MovementInput;
        float wishSpeed = m_Speed;

        if (glm::length(wishDir) > 1.0f)
            wishDir = glm::normalize(wishDir);

        glm::vec3 wishVel = wishDir * wishSpeed;
        btVector3 desiredVel(wishVel.x, velocity.y(), wishVel.z);

        if (m_IsGrounded)
        {
            btVector3 n = m_GroundNormal;
            btVector3 lateral =
                desiredVel - n * desiredVel.dot(n);

            desiredVel.setX(lateral.x());
            desiredVel.setZ(lateral.z());
        }

        m_Body->setLinearVelocity(desiredVel);
    }

    void Player::TryPickupProp(Crd::Object::Prop *prop)
    {
        if (!prop || m_HoldingProp)
            return;

        glm::vec3 grabPoint =
            GetHeadPosition() + m_CameraPtr->GetForward() * 2.0f;

        glm::vec3 propPos = prop->GetPosition();
        glm::vec3 cameraPos = GetHeadPosition();
        glm::vec3 direction = glm::normalize(cameraPos - propPos);

        glm::quat lookAtQuat =
            glm::quatLookAt(direction, glm::vec3(0, 1, 0));

        prop->PickUp(grabPoint, lookAtQuat);

        if (prop->IsPickedUp())
            m_HoldingProp = prop;
    }

    void Player::DropHeldProp()
    {
        if (!m_HoldingProp)
            return;
        m_CanLook = true;
        // Toggle pickup OFF
        m_HoldingProp->PickUp({}, {});
        m_HoldingProp = nullptr;
    }

    void Player::m_HandlePropMovement()
    {
        if (!m_HoldingProp)
            return;
        if (m_HoldingProp->IsPickedUp())
        {
            glm::vec3 propPos = m_HoldingProp->GetPosition(); // your prop’s world position
            glm::vec3 cameraPos = GetHeadPosition();
            glm::vec3 direction = glm::normalize(cameraPos - propPos);

            glm::quat lookAtQuat = glm::quatLookAt(direction, glm::vec3(0, 1, 0)); // up = Y
            glm::vec3 holdPos = GetHeadPosition() + m_CameraPtr->GetForward() * 4.0f;
            m_HoldingProp->Move(holdPos, m_CameraPtr->GetForward() * 2.0f, lookAtQuat);
            SetCanLook(!Az::Input::GetKey(AZ_Q));
            if (Az::Input::GetKey(AZ_Q))
            {
                m_HoldingProp->Rotate();
            }
        }
    }

    void Player::m_UpdateCameraFromBody()
    {
        if (!m_Body || !m_CameraPtr)
            return;

        // Get body position
        btTransform bodyTransform;
        m_Body->getMotionState()->getWorldTransform(bodyTransform);
        btVector3 bodyPos = bodyTransform.getOrigin();

        // Calculate head position (body pos + camera height)
        glm::vec3 headPos = glm::vec3(
            bodyPos.x(),
            bodyPos.y() + m_CameraHeight, // Camera at eye level
            bodyPos.z());

        // Update camera position (but keep its rotation)
        m_CameraPtr->SetPosition(headPos);
    }

    glm::vec3 Player::GetHeadPosition() const
    {
        if (!m_Body)
            return glm::vec3(0.0f);

        btTransform bodyTransform;
        m_Body->getMotionState()->getWorldTransform(bodyTransform);
        btVector3 bodyPos = bodyTransform.getOrigin();

        return glm::vec3(
            bodyPos.x(),
            bodyPos.y() + m_CameraHeight,
            bodyPos.z());
    }

    glm::vec3 Player::GetForwardRay(float length)
    {
        return GetHeadPosition() + m_CameraPtr->GetForward() * length;
    }
}