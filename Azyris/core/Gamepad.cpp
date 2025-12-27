#include "Gamepad.h"
#include <iostream>
#include <Helpers.h>

namespace Az
{
    bool Gamepad::FindGamepad()
    {
        if (m_IsConnected == true)
        {
            std::cout << "Controller already connected!" << std::endl;
            return false;
        }

        int count = 0;
        m_JoystickPtr = SDL_GetGamepads(&count);

        if (!m_JoystickPtr)
        {
            std::cout << "SDL_GetGamepads failed: " << SDL_GetError() << "\n";
            SDL_Quit();
            return 1;
        }
        std::cout << "Found " << count << " gamepad(s)\n";
        m_JoystickID = m_JoystickPtr[0];
        m_Gamepad = SDL_OpenGamepad(m_JoystickID);
        if (count > 0)
        {

            if (m_Gamepad)
            {
                const char *name = SDL_GetGamepadNameForID(m_JoystickID);
                std::cout << "Opened gamepad 0: " << (name ? name : "Unknown") << "\n";
                m_IsConnected = true;
                return false;
            }
            else
            {
                std::cout << "SDL_OpenGamepad failed: " << SDL_GetError() << "\n";
                m_IsConnected = false;
                return true;
            }
        }
        m_IsConnected = false;
        return false;
    }

    float Gamepad::GetAxis(AZ_GamepadAxis axis)
    {
        int val;
        float value = 0;

        switch (axis)
        {
        case AZ_GPAD_AXIS_LEFTX:
            val = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_LEFTX);
            break;
        case AZ_GPAD_AXIS_LEFTY:
            val = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_LEFTY);
            break;
        case AZ_GPAD_AXIS_RIGHTX:
            val = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
            break;
        case AZ_GPAD_AXIS_RIGHTY:
            val = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_RIGHTY);
            break;
        case AZ_GPAD_AXIS_LEFT_TRIGGER:
            val = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
            break;
        case AZ_GPAD_AXIS_RIGHT_TRIGGER:
            val = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
            break;

        default:
            val = 0;
            break;
        }
        value = Az::normalizeAxis(val);
        return value;
    }
    bool Gamepad::GetButton(AZ_GamepadButton button)
    {
        bool value = false;

        switch (button)
        {
        case AZ_GPAD_BUTTON_SOUTH:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
            break;
        case AZ_GPAD_BUTTON_EAST:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_EAST);
            break;
        case AZ_GPAD_BUTTON_WEST:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_WEST);
            break;
        case AZ_GPAD_BUTTON_NORTH:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_NORTH);
            break;
        case AZ_GPAD_BUTTON_BACK:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_BACK);
            break;
        case AZ_GPAD_BUTTON_GUIDE:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_GUIDE);
            break;
        case AZ_GPAD_BUTTON_START:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_START);
            break;
        case AZ_GPAD_BUTTON_LEFT_STICK:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_LEFT_STICK);
            break;
        case AZ_GPAD_BUTTON_RIGHT_STICK:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_RIGHT_STICK);
            break;
        case AZ_GPAD_BUTTON_LEFT_SHOULDER:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
            break;
        case AZ_GPAD_BUTTON_RIGHT_SHOULDER:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
            break;
        case AZ_GPAD_BUTTON_DPAD_UP:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
            break;
        case AZ_GPAD_BUTTON_DPAD_DOWN:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
            break;
        case AZ_GPAD_BUTTON_DPAD_LEFT:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
            break;
        case AZ_GPAD_BUTTON_DPAD_RIGHT:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
            break;
        case AZ_GPAD_BUTTON_MISC1:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_MISC1);
            break;
        case AZ_GPAD_BUTTON_LEFT_PADDLE1:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_LEFT_PADDLE1);
            break;
        case AZ_GPAD_BUTTON_RIGHT_PADDLE1:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1);
            break;
        case AZ_GPAD_BUTTON_LEFT_PADDLE2:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_LEFT_PADDLE2);
            break;
        case AZ_GPAD_BUTTON_RIGHT_PADDLE2:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2);
            break;
        case AZ_GPAD_BUTTON_TOUCHPAD:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_TOUCHPAD);
            break;
        case AZ_GPAD_BUTTON_MISC2:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_MISC2);
            break;
        case AZ_GPAD_BUTTON_MISC3:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_MISC3);
            break;
        case AZ_GPAD_BUTTON_MISC4:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_MISC4);
            break;
        case AZ_GPAD_BUTTON_MISC5:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_MISC5);
            break;
        case AZ_GPAD_BUTTON_MISC6:
            value = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_MISC6);
            break;

        default:
            value = false;
            break;
        }
        return value;
    }
    void Gamepad::UpdateButtonStates()
    {
        if (!m_Gamepad)
            return;

        // Copy current state to previous
        for (int i = 0; i < NUM_BUTTONS; ++i)
            m_PreviousButtons[i] = m_CurrentButtons[i];

        // Read current state from SDL
        for (int i = 0; i < NUM_BUTTONS; ++i)
            m_CurrentButtons[i] = SDL_GetGamepadButton(m_Gamepad, static_cast<SDL_GamepadButton>(i));
    }
    bool Gamepad::GetButtonDown(AZ_GamepadButton button)
    {
        int idx = static_cast<int>(button);
        if (idx < 0 || idx >= NUM_BUTTONS)
            return false;

        return m_CurrentButtons[idx] && !m_PreviousButtons[idx];
    }
    bool Gamepad::GetButtonUp(AZ_GamepadButton button)
    {
        int idx = static_cast<int>(button);
        if (idx < 0 || idx >= NUM_BUTTONS)
            return false;

        return !m_CurrentButtons[idx] && m_PreviousButtons[idx];
    }
}
