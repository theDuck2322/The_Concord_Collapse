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
        bool m_InitWindow(const std::string &title, uint32_t width, uint32_t height);
        void m_InitSystems();
        void m_LoadAssets();
        void m_InitPhysics();
        void m_InitLogic();
        void m_MainLoop();

        void m_UpdateLogic();
        void m_Render();
        void m_RenderDebug();
        void m_RenderUI();

        void m_ShutDown();

    private: // Loop functions
    private:
        Az::Screen m_Window;
        Az::Renderer m_Renderer;
        Az::Camera3D m_Camera3D;
        Crd::Player m_Player;

        Az::physx::PhysicsManager m_PhysicsManager;
        Crd::MdIsp::ModelInspector m_ModelInspector;
        Crd::MdLogic::LogicProcessor m_LogicProcessor;

        Az::Model m_Scene;
        Az::Model m_Sphere;
        Az::Model m_LogicModel;
        Az::Model m_Cube;
        Az::Model m_Props;

        Az::DEBUG_Renderer m_DBR;

        bool m_WireframeMode = false;
        bool m_DebugDraw = false;

        Az::Shader m_Shader;
        Az::Shader m_DEBUG_SHADER;

    private: // camera settings
        float m_FOV = 70.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        float m_NearClip = 0.01f;
        float m_FarClip = 1000.0f;
        float m_Speed = 10.0f;
        float m_Sensitivity = 140.0f;
    };
}