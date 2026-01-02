#include <Input.h>

namespace Az
{
    std::unordered_map<AZ_Scancode, bool> Input::m_CurrentKeys;
    std::unordered_map<AZ_Scancode, bool> Input::m_PreviousKeys;
    uint32_t Input::m_CurrentMouseButtons = 0;
    uint32_t Input::m_PreviousMouseButtons = 0;
    glm::vec2 Input::m_MousePosition = {0.0f, 0.0f};
    glm::vec2 Input::m_RelativeMousePosition = {0.0f, 0.0f};

    void Input::Init()
    {
        m_CurrentKeys.clear();
        m_PreviousKeys.clear();
        m_CurrentMouseButtons = 0;
        m_PreviousMouseButtons = 0;
        m_MousePosition = {0.0f, 0.0f};
        m_RelativeMousePosition = {0.0f, 0.0f};
    }

    void Input::Update()
    {
        // Copy previous state
        m_PreviousKeys = m_CurrentKeys;
        m_PreviousMouseButtons = m_CurrentMouseButtons;

        // Mouse
        float mx, my;
        m_CurrentMouseButtons = SDL_GetMouseState(&mx, &my);
        m_MousePosition = glm::vec2((float)mx, (float)my);

        float x, y;
        SDL_GetRelativeMouseState(&x, &y);
        m_RelativeMousePosition = glm::vec2(x, y);

        // Keyboard
        const bool *keystates = SDL_GetKeyboardState(nullptr);
        for (auto &key : m_CurrentKeys)
        {
            m_CurrentKeys[key.first] = keystates[key.first];
        }
    }

    // Keyboard
    bool Input::GetKey(AZ_Scancode key)
    {
        return m_CurrentKeys[key];
    }

    bool Input::GetKeyDown(AZ_Scancode key)
    {
        return m_CurrentKeys[key] && !m_PreviousKeys[key];
    }

    bool Input::GetKeyUp(AZ_Scancode key)
    {
        return !m_CurrentKeys[key] && m_PreviousKeys[key];
    }

    // Mouse
    glm::vec2 Input::GetMousePos()
    {
        return m_MousePosition;
    }

    float Input::GetMouseX()
    {
        return m_MousePosition.x;
    }

    float Input::GetMouseY()
    {
        return m_MousePosition.y;
    }

    glm::vec2 Input::GetMouseDelta()
    {
        return m_RelativeMousePosition;
    }

    float Input::GetMouseDeltaX()
    {

        return m_RelativeMousePosition.x;
    }

    float Input::GetMouseDeltaY()
    {

        return m_RelativeMousePosition.y;
    }

    bool Input::GetMouseButton(uint8_t button)
    {
        return m_CurrentMouseButtons & SDL_BUTTON_MASK(button);
    }

    bool Input::GetMouseButtonDown(uint8_t button)
    {
        return (m_CurrentMouseButtons & SDL_BUTTON_MASK(button)) &&
               !(m_PreviousMouseButtons & SDL_BUTTON_MASK(button));
    }

    bool Input::GetMouseButtonUp(uint8_t button)
    {
        return !(m_CurrentMouseButtons & SDL_BUTTON_MASK(button)) &&
               (m_PreviousMouseButtons & SDL_BUTTON_MASK(button));
    }
}
