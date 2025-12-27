#pragma once

#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl3.h>
#include <ImGUI/imgui_impl_opengl3.h>

#include <SDL3/SDL.h>
#include <glad/glad.h>

namespace ImGuiLayer
{
    void Init(SDL_Window *window, SDL_GLContext *GLcontext);

    void Shutdown();

    void BeginFrame();

    void EndFrame();
}
