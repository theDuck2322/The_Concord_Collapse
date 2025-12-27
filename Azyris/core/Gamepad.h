#pragma once

#include <SDL3/SDL.h>
#include <core/AzyrisKeys.h>

namespace Az
{
    // this gamepad class is trash, it is good only for one controller at a time
    // as the saying goes: "A jack of all trades is a master of none."
    class Gamepad
    {
    public:
        bool FindGamepad();
        static inline void UpdateGamepads()
        {
            SDL_UpdateGamepads();
        }
        //
        float GetAxis(AZ_GamepadAxis axis);

        bool GetButton(AZ_GamepadButton button);

        void UpdateButtonStates();

        bool GetButtonDown(AZ_GamepadButton button);

        bool GetButtonUp(AZ_GamepadButton button);

        inline bool IsConnected() { return m_IsConnected; }

    private:
        SDL_JoystickID *m_JoystickPtr;
        SDL_JoystickID m_JoystickID;
        SDL_Gamepad *m_Gamepad;
        bool m_IsConnected = false;

        static constexpr int NUM_BUTTONS = 32; // max SDL gamepad buttons
        bool m_CurrentButtons[NUM_BUTTONS] = {false};
        bool m_PreviousButtons[NUM_BUTTONS] = {false};
    };
} // namespace Az
