#pragma once
#include <Concord.h>

namespace Crd
{
    class Application
    {
    public:
        Application() = default;
        void ProccessEvents();

        bool Run(const std::string &title, uint32_t width, uint32_t height);

    private:
        Az::Screen m_Window;
        Az::Renderer m_Renderer;
        Az::Camera3D m_Camera3D;
        Crd::Player m_Player;
        Az::physx::PhysicsManager m_PhysicsManager;

        Az::Model m_Scene;

        bool m_WireframeMode = false;
        bool m_DebugDraw = false;

        Az::Shader m_Shader;

    private: // camera settings
        float m_FOV = 70.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        float m_NearClip = 0.01f;
        float m_FarClip = 1000.0f;
        float m_Speed = 10.0f;
        float m_Sensitivity = 140.0f;
    };
}