#include <graphics/ImGUI_Layer.h>
#include "ImGUI_Layer.h"

namespace ImGuiLayer
{
    void ImGuiLayer::Init(SDL_Window *window, SDL_GLContext *GLcontext)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // optional
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // optional
        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForOpenGL(window, GLcontext);
        ImGui_ImplOpenGL3_Init("#version 450");
    }

    void Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
    void BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }
    void EndFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

}
