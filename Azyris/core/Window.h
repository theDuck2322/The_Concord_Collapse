#pragma once
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace Az
{
    class Screen
    {
    public:
        Screen() = default;
        bool AZCreateWindow(const char *title, uint32_t width, uint32_t height);
        void Close();

        inline SDL_Window *GetWindow() { return m_WindowPtr; }
        inline SDL_GLContext *GetGLContext() { return &m_Context; }

        inline void Clear(float r = 0, float g = 0, float b = 0, float a = 1)
        {
            Clear(glm::vec4(r, g, b, a));
        }

        inline void Clear(const glm::vec4 &color)
        {
            glClearColor(color.r,
                         color.g,
                         color.b,
                         color.a);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void SwapBuffers();

        inline void SetShouldClose(bool val) { m_ShouldClose = val; }
        inline bool ShouldClose() { return m_ShouldClose; }

        inline SDL_Event *GetEvent() { return &m_Event; }
        inline bool PollEvent()
        {
            return SDL_PollEvent(&m_Event);
        }

        inline glm::vec2 GetSize()
        {
            int *w;
            int *h;
            SDL_GetWindowSize(m_WindowPtr, w, h);
            return glm::vec2((float)*w, (float)*h);
        }
        inline void SetSize(uint32_t width, uint32_t height)
        {
            m_Width = width;
            m_Height = height;
        }

    private:
        bool SetWorkingDirectory();

    private:
        SDL_Window *m_WindowPtr;
        SDL_GLContext m_Context;

        SDL_Event m_Event;

        uint32_t m_Width;
        uint32_t m_Height;

        bool m_ShouldClose = true;
    };

} // namespace Az
