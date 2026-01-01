#include <Application.h>

namespace Crd
{
    void Application::ProccessEvents()
    {
        while (m_Window.PollEvent())
        {
            SDL_Event *event = m_Window.GetEvent();

            ImGui_ImplSDL3_ProcessEvent(event);

            if (event->type == SDL_EVENT_QUIT)
            {
                m_Window.SetShouldClose(true);
            }
            if (event->type == SDL_EVENT_WINDOW_RESIZED)
            {
                int w = 0;
                int h = 0;
                SDL_GetWindowSize(m_Window.GetWindow(), &w, &h);
                m_Window.SetSize(uint32_t(w), uint32_t(h));
                glViewport(0, 0, w, h);
                m_AspectRatio = (float)w / (float)h;
                m_Camera3D.SetProjection(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
            }
            if (event->type == SDL_EVENT_KEY_DOWN &&
                event->key.scancode == SDL_SCANCODE_F11 &&
                !event->key.repeat)
            {
                SDL_SetWindowFullscreen(m_Window.GetWindow(),
                                        SDL_GetWindowFlags(m_Window.GetWindow()) & SDL_WINDOW_FULLSCREEN
                                            ? 0
                                            : SDL_WINDOW_FULLSCREEN);
                int w = 0;
                int h = 0;
                SDL_GetWindowSize(m_Window.GetWindow(), &w, &h);
                m_Window.SetSize(uint32_t(w), uint32_t(h));
                glViewport(0, 0, w, h);
                m_AspectRatio = (float)w / (float)h;
                m_Camera3D.SetProjection(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
            }

            if (event->type == SDL_EVENT_GAMEPAD_ADDED)
            {
                m_Player.GetGamepad()->FindGamepad();
            }
            if (event->type == SDL_EVENT_GAMEPAD_REMOVED)
            {
                m_Player.GetGamepad()->DisableGamepad();
                std::cout << "Gamepad disconected" << std::endl;
            }

            SDL_UpdateGamepads();
        }

        if (Az::Input::GetKeyDown(AZ_F))
        {
            m_WireframeMode = !m_WireframeMode;
            if (m_WireframeMode)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        if (Az::Input::GetKeyDown(AZ_P))
        {
            m_DebugDraw = !m_DebugDraw;
        }

        if (Az::Input::GetKeyDown(AZ_F1))
        {
            bool toggle = SDL_GetWindowRelativeMouseMode(m_Window.GetWindow());
            SDL_SetWindowRelativeMouseMode(m_Window.GetWindow(), !toggle);
        }
    }

    bool Application::Run(const std::string &title, uint32_t width, uint32_t height)
    {
        if (m_InitWindow(title, width, height))
        {
            std::cout << "Failed to create the window" << std::endl;
            return true;
        }
#ifdef AZ_DEBUG
#pragma message("DEBUG BUILD")
        std::cout << "Debug Mode" << std::endl;
#else
#pragma message("RELEASE BUILD")
        std::cout << "Release Mode" << std::endl;
#endif

        m_InitSystems();
        m_LoadAssets();
        m_InitPhysics();
        m_InitLogic();
        m_MainLoop();
        m_ShutDown();
        return false;
    }

    ///---------------------------------------- Window Creation --------------------------------------------
    bool Application::m_InitWindow(const std::string &title, uint32_t width, uint32_t height)
    {
        if (m_Window.AZCreateWindow(title.c_str(), width, height) == EXIT_FAILURE)
        {
            return true;
        }
        return false;
    }

    ///--------------------------------------- Systems initialization ----------------------------------------
    void Application::m_InitSystems()
    {
        ImGuiLayer::Init(m_Window.GetWindow(), m_Window.GetGLContext());

        Az::Input::Init();

        m_AspectRatio = (float)m_Window.GetSize().x / (float)m_Window.GetSize().y;

        m_Camera3D = Az::Camera3D(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);

        m_DBR.INIT_RENDERER(nullptr, m_Camera3D.GetViewProjectionPtr());
    }

    ///---------------------------------- Assets loading --------------------------------------------------
    void Application::m_LoadAssets()
    {
        // shader loading
        m_Shader.load("Azyris/shaders/default.vert", "Azyris/shaders/default.frag");
        m_Renderer.SetShader(&m_Shader);

        m_DEBUG_SHADER.load("Azyris/shaders/DebugShader.vert", "Azyris/shaders/DebugShader.frag");
        m_DBR.SetShader(&m_DEBUG_SHADER);

        // models loading
        m_Scene.load("Azyris/Assets/hm.glb");
        m_LogicModel.load("Azyris/Assets/func.glb");
        m_Sphere.load("Azyris/Assets/ball.glb");
        m_Cube.load("Azyris/Assets/bx.glb");
        m_Props.load("Azyris/Assets/pickable.glb");
    }

    size_t SceneID;
    size_t SphereID;
    size_t CubeID;
    std::vector<glm::vec3> SceneColliderLines;

    ///-------------------------------------- Physics Initialization ------------------------------
    void Application::m_InitPhysics()
    {
        Crd::Object::Prop::SetPhysicsManagerPtr(&m_PhysicsManager);
        m_Player.Init(&m_Camera3D, &m_PhysicsManager); // player shall save its collider id

        SceneID = m_PhysicsManager.CreateMeshCollider(m_Scene);
        auto sceneRB = m_PhysicsManager.GetRigidbodyById(SceneID);
        sceneRB->setFriction(1.0f);

        SphereID = m_PhysicsManager.CreateSphere(1, glm::vec3(0, 10, 0), 1);
        auto ball = m_PhysicsManager.GetRigidbodyById(SphereID);
        ball->setDamping(0.5f, 0.5f);
        ball->setFriction(0.5f);
        ball->setRollingFriction(0.2f);

        CubeID = m_PhysicsManager.CreateBox(1.0f, glm::vec3(3, 2, 10), glm::vec3(1));
        auto boxrb = m_PhysicsManager.GetRigidbodyById(CubeID);
        boxrb->setFriction(1.0f);
        boxrb->setDamping(0.5f, 0.5f);

        Az::physx::DebugLineCollector collector(SceneColliderLines);

        // --- Extract debug lines ---
        btCollisionShape *shape = sceneRB->getCollisionShape();

        if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
        {
            auto *meshShape = static_cast<btBvhTriangleMeshShape *>(shape);

            btVector3 aabbMin(-10000, -10000, -10000);
            btVector3 aabbMax(10000, 10000, 10000);

            meshShape->processAllTriangles(&collector, aabbMin, aabbMax);
        }
    }

    ///------------------------------------- Logic initialization --------------------------------
    void Application::m_InitLogic()
    {
        m_ModelInspector.CheckMeta(m_LogicModel);
        m_ModelInspector.CheckMeta(m_Props);

        auto data = m_ModelInspector.GetData();
        auto pickable = m_ModelInspector.GetPickableObjects();
        m_LogicProcessor.SetData(data);
        m_LogicProcessor.SetPickable(pickable);
        m_LogicProcessor.Init(&m_PhysicsManager);
    }

    ///------------------------------------- Main Loop -------------------------------------------
    void Application::m_MainLoop()
    {
        while (!m_Window.ShouldClose())
        {

            m_UpdateLogic();

            m_Render();
            if (m_DebugDraw)
                m_RenderDebug();

            m_RenderUI();

            m_Window.SwapBuffers();
        }
    }

    ///---------------------------------- Update Logic ---------------------------------------------
    void Application::m_UpdateLogic()
    {
        Az::Timer::UpdateTime();
        Az::Input::Update();

        /////// EVENT POLLING ///////////
        ProccessEvents();

        /////////////////////////////////
        // Step physics
        m_PhysicsManager.Update(1.0f / 60.0f, 6, 1.0f / 60.0f);

        m_LogicProcessor.Update();

        btVector3 from = Az::ConvertGLMVec3(m_Player.GetHeadPosition());

        btVector3 to = Az::ConvertGLMVec3(m_Player.GetForwardRay(8.0f));

        m_LogicProcessor.RaycastLogic(from, to, m_Player.GetGamepad()->GetButtonDown(AZ_GPAD_BUTTON_WEST) || Az::Input::GetMouseButtonDown(1));

        if (Az::Input::GetKeyDown(AZ_E))
        {
            if (m_Player.IsHoldingProp())
            {
                m_Player.DropHeldProp();
            }
            else
            {
                if (auto *prop = m_LogicProcessor.RaycastProp(from, to, true))
                {
                    m_Player.TryPickupProp(prop);
                }
            }
        }

        m_Player.Update();
    }

    ///----------------------------------- Render function -------------------------------------------
    void Application::m_Render()
    {
        m_Window.Clear(0.109f, 0.137f, 0.180f, 1);

        m_Shader.use();
        m_Shader.setUniform("viewPos", m_Camera3D.GetPosition());
        m_Shader.setUniform("projection", m_Camera3D.GetProjectionMatrix());
        m_Shader.setUniform("view", m_Camera3D.GetViewMatrix());
        m_Shader.setUniform("enableLight", true);
        m_Shader.setUniform("lightDirection", glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f)));
        m_Shader.setUniform("lightColor", glm::vec3(1.0f));
        m_Shader.setUniform("lightIntensity", 0.5f);

        m_Renderer.AddModel(&m_Scene);
        m_Renderer.AddModel(&m_LogicModel);

        btTransform trans;
        m_PhysicsManager.GetRigidbodyById(SphereID)->getMotionState()->getWorldTransform(trans);

        btVector3 pos = trans.getOrigin();
        btQuaternion rot = trans.getRotation();

        glm::quat glmRot(Az::ConvertBTQuat(rot));

        // Build model matrix: TRANSLATE * ROTATE
        glm::mat4 ballModel =
            glm::translate(glm::mat4(1.0f),
                           glm::vec3(pos.getX(), pos.getY(), pos.getZ())) *
            glm::mat4_cast(glmRot);
        m_Renderer.AddModel(&m_Sphere, &ballModel);

        for (auto &prop : *m_LogicProcessor.GetProps())
        {
            m_Renderer.AddMesh(prop.get()->GetMesh(), prop.get()->GetModelMatrix());
        }

        Az::Profiler::StartProfiling();
        m_Renderer.Draw(m_Camera3D.GetPosition());
        Az::Profiler::EndProfiling();
    }

    ///-------------------------------------- Render Debug ---------------------------------------------
    void Application::m_RenderDebug()
    {
        m_DBR.Begin();

        for (auto &rb : *m_PhysicsManager.GetRigidbodies())
        {
            if (!rb || m_Player.GetRigidbody() == rb)
                continue;
            btVector3 min, max;
            rb->getCollisionShape()->getAabb(rb->getWorldTransform(), min, max);

            btVector3 halfExtent = (max - min) * 0.5f;

            btTransform transform;
            rb->getMotionState()->getWorldTransform(transform);
            btQuaternion qt = transform.getRotation();
            btVector3 translation = transform.getOrigin();
            glm::vec3 position = Az::ConvertBTVec3(translation);
            glm::mat4 orientation = glm::mat4_cast(Az::ConvertBTQuat(qt));

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * orientation;
            m_DBR.AddOBB({model, Az::ConvertBTVec3(halfExtent)});
            // m_DBR.AddAABB(glm::vec3(min.getX(), min.getY(), min.getZ()),
            //               glm::vec3(max.getX(), max.getY(), max.getZ()));
        }

        for (size_t i = 0; i < SceneColliderLines.size(); i += 2)
        {
            m_DBR.AddLine(SceneColliderLines[i], SceneColliderLines[i + 1]);
        }

        m_DBR.Draw();
    }

    ///---------------------------------------- Render UI -----------------------------------------------
    void Application::m_RenderUI()
    {
        ImGuiLayer::BeginFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowBgAlpha(0.0f); // fully transparent

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_NoFocusOnAppearing |
                                        ImGuiWindowFlags_NoNav;

        auto trs = m_Player.GetRigidbody()->getWorldTransform();
        auto ps = trs.getOrigin();
        ImGui::Begin("HUD", nullptr, window_flags);

        // Draw text
        ImGui::SetCursorPos(ImVec2(10, 10)); // top-left corner
        ImGui::Text("FPS: %.1f", 1.0f / Az::Timer::fDeltaTime);
        ImGui::Text("Ammo: %d / %d", 3, 67);
        ImGui::Text("Position: %f, %f, %f", ps.getX(), ps.getY(), ps.getZ());
        ImGui::Text("Draw calls: %d", (int)Az::GetDrawCallsCounter());
        ImGui::Text("Time for rendering: %.17g", Az::Profiler::GetLastDuration());

        ImGui::End();

        // Get screen size
        ImGuiIO &io = ImGui::GetIO();
        float cx = io.DisplaySize.x * 0.5f;
        float cy = io.DisplaySize.y * 0.5f;

        // Get the foreground draw list (draws above everything)
        ImDrawList *draw_list = ImGui::GetForegroundDrawList();

        // Crosshair size and color
        float size = 10.0f;                     // half-length of each line
        ImU32 color = IM_COL32(255, 0, 0, 255); // red

        // Draw vertical line
        draw_list->AddLine(ImVec2(cx, cy - size), ImVec2(cx, cy + size), color, 2.0f);

        // Draw horizontal line
        draw_list->AddLine(ImVec2(cx - size, cy), ImVec2(cx + size, cy), color, 2.0f);

        ImGuiLayer::EndFrame();
    }

    ///----------------------------------------- SHUTDOWN -------------------------------------------
    void Application::m_ShutDown()
    {
        ImGuiLayer::Shutdown();
        m_PhysicsManager.Cleanup();
    }
}