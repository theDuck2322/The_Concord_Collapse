#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <core/AzyrisKeys.h>

namespace Az
{
    class Input
    {
    public:
        static void Init();

        // Keyboard
        static bool GetKey(AZ_Scancode key);
        static bool GetKeyDown(AZ_Scancode key);
        static bool GetKeyUp(AZ_Scancode key);

        // Mouse
        static glm::vec2 GetMousePos();
        static float GetMouseX();
        static float GetMouseY();

        static glm::vec2 GetMouseDelta();
        static float GetMouseDeltaX();
        static float GetMouseDeltaY();

        static bool GetMouseButton(uint8_t button);
        static bool GetMouseButtonDown(uint8_t button);
        static bool GetMouseButtonUp(uint8_t button);

        // Must be called every frame
        static void Update();

    private:
        static std::unordered_map<AZ_Scancode, bool> m_CurrentKeys;
        static std::unordered_map<AZ_Scancode, bool> m_PreviousKeys;
        static uint32_t m_CurrentMouseButtons;
        static uint32_t m_PreviousMouseButtons;
        static glm::vec2 m_MousePosition;
        static glm::vec2 m_RelativeMousePosition;
    };
}
