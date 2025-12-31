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
                int w;
                int h;
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
                int w;
                int h;
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

    struct DebugLineCollector : public btTriangleCallback
    {
        std::vector<glm::vec3> &lines;

        DebugLineCollector(std::vector<glm::vec3> &outLines) : lines(outLines) {}

        void processTriangle(btVector3 *triangle, int /*partId*/, int /*triangleIndex*/) override
        {
            glm::vec3 v0(triangle[0].x(), triangle[0].y(), triangle[0].z());
            glm::vec3 v1(triangle[1].x(), triangle[1].y(), triangle[1].z());
            glm::vec3 v2(triangle[2].x(), triangle[2].y(), triangle[2].z());

            lines.push_back(v0);
            lines.push_back(v1);
            lines.push_back(v1);
            lines.push_back(v2);
            lines.push_back(v2);
            lines.push_back(v0);
        }
    };

    bool Application::Run(const std::string &title, uint32_t width, uint32_t height)
    {
        if (m_Window.AZCreateWindow(title.c_str(), width, height) == EXIT_FAILURE)
        {
            return true;
        }

        ImGuiLayer::Init(m_Window.GetWindow(), m_Window.GetGLContext());

        Az::Input::Init();
        m_AspectRatio = (float)width / (float)height;
        m_Camera3D = Az::Camera3D(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);

        m_Shader.load("Azyris/shaders/test.vert", "Azyris/shaders/test.frag");

        Az::Model sphere, logicModel;
        m_Scene.load("Azyris/Assets/hm.glb");
        logicModel.load("Azyris/Assets/func.glb");
        sphere.load("Azyris/Assets/ball.glb");

        auto ball = m_PhysicsManager.CreateSphere(1, glm::vec3(0, 10, 0), 1);
        ball->setDamping(0.5f, 0.5f);
        ball->setFriction(0.5f);
        ball->setRollingFriction(0.2f);

        auto sceneRb = m_PhysicsManager.CreateMeshCollider(m_Scene);
        sceneRb->setFriction(1.0f);

        Az::Shader dbShader;
        dbShader.load("Azyris/shaders/lineDrawer.vert", "Azyris/shaders/lineDrawer.frag");
        Az::DEBUG_Renderer dbr;
        dbr.INIT_RENDERER(&dbShader, m_Camera3D.GetViewProjectionPtr());

        std::vector<glm::vec3> sceneColliderLines;
        DebugLineCollector collector(sceneColliderLines);

        // --- Extract debug lines ---
        btCollisionShape *shape = sceneRb->getCollisionShape();

        if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
        {
            auto *meshShape = static_cast<btBvhTriangleMeshShape *>(shape);

            btVector3 aabbMin(-10000, -10000, -10000);
            btVector3 aabbMax(10000, 10000, 10000);

            meshShape->processAllTriangles(&collector, aabbMin, aabbMax);
        }

        Az::Model cube("Azyris/Assets/bx.glb");
        Az::Model Props("Azyris/Assets/pickable.glb");

        auto boxrb = m_PhysicsManager.CreateBox(1.0f, glm::vec3(3, 2, 10), glm::vec3(1));
        boxrb->setFriction(1.0f);
        boxrb->setDamping(0.5f, 0.5f);

        Crd::Object::Prop::SetPhysicsManagerPtr(&m_PhysicsManager);
        Crd::Object::Prop prop;

        prop.SetMesh(&cube.meshes[0]);
        prop.SetRigidBody(boxrb);
        prop.Init();

        std::cout << "Data processed: " << std::endl;
        Crd::MdIsp::ModelInspector Inspector;
        Inspector.CheckMeta(logicModel);
        Inspector.CheckMeta(Props);
        auto data = Inspector.GetData();
        auto pickable = Inspector.GetPickableObjects();
        Crd::MdLogic::LogicProcessor LogicProcess;
        LogicProcess.SetData(data);
        LogicProcess.SetPickable(pickable);
        LogicProcess.Init(&m_PhysicsManager);

        m_Player.Init(&m_Camera3D, &m_PhysicsManager);

        m_Renderer.SetShader(&m_Shader);

        //  -------------------- Main Loop --------------------
        while (!m_Window.ShouldClose())
        {
            Az::Timer::UpdateTime();
            Az::Input::Update();

            /////// EVENT POLLING ///////////
            ProccessEvents();
            /////////////////////////////////
            // Step physics
            m_PhysicsManager.Update(1.0f / 60.0f, 6, 1.0f / 60.0f);

            // -------------------- RAYCAST TO BUTTON --------------------
            btVector3 from(m_Camera3D.GetPosition().x, m_Camera3D.GetPosition().y, m_Camera3D.GetPosition().z);
            btVector3 forward(m_Camera3D.GetForward().x, m_Camera3D.GetForward().y, m_Camera3D.GetForward().z);
            btVector3 to = from + forward * 8.0f; // ray length
            LogicProcess.RaycastLogic(from, to, m_Player.GetGamepad()->GetButtonDown(AZ_GPAD_BUTTON_WEST) || Az::Input::GetMouseButtonDown(1));
            if (Az::Input::GetKeyDown(AZ_E))
            {
                if (m_Player.IsHoldingProp())
                {
                    m_Player.DropHeldProp();
                }
                else
                {
                    if (auto *prop = LogicProcess.RaycastProp(from, to, true))
                    {
                        m_Player.TryPickupProp(prop);
                    }
                }
            }

            // ------------------------------------------------------------
            LogicProcess.Update();
            m_Player.Update();

            // -------------------- Rendering --------------------
            m_Window.Clear(0.109f, 0.137f, 0.180f, 1);

            m_Shader.use();
            m_Shader.setUniform("viewPos", m_Camera3D.GetPosition());
            m_Shader.setUniform("projection", m_Camera3D.GetProjectionMatrix());
            m_Shader.setUniform("view", m_Camera3D.GetViewMatrix());

            // Draw scene
            m_Renderer.AddModel(&m_Scene);
            m_Renderer.AddModel(&logicModel);

            // Draw ball
            btTransform trans;
            ball->getMotionState()->getWorldTransform(trans);

            btVector3 pos = trans.getOrigin();
            btQuaternion rot = trans.getRotation();

            glm::quat glmRot(Az::ConvertBTQuat(rot));

            // Build model matrix: TRANSLATE * ROTATE
            glm::mat4 ballModel =
                glm::translate(glm::mat4(1.0f),
                               glm::vec3(pos.getX(), pos.getY(), pos.getZ())) *
                glm::mat4_cast(glmRot);

            /// sphere drawing with lighting = TRUE

            m_Shader.setUniform("enableLight", true);
            m_Shader.setUniform("lightDirection", glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f)));
            m_Shader.setUniform("lightColor", glm::vec3(1.0f));
            m_Shader.setUniform("lightIntensity", 0.5f);
            // sphere.Draw(m_Shader, &ballModel);
            m_Renderer.AddModel(&sphere, &ballModel);

            m_Renderer.AddModel(&cube, prop.GetModelMatrix());

            for (auto &prop : *LogicProcess.GetProps())
            {
                m_Renderer.AddMesh(prop.get()->GetMesh(), prop.get()->GetModelMatrix());
            }

            // m_Shader.setUniform("enableLight", false);
            m_Renderer.SetFrustumPtr(m_Camera3D.GetFrustum());
            Az::Profiler::StartProfiling();
            m_Renderer.Draw(m_Camera3D.GetPosition());
            Az::Profiler::EndProfiling();

            if (m_DebugDraw)
            {
                dbr.Begin();

                dbr.AddOBB({*prop.GetModelMatrix(), cube.meshes[0].GetLocalHalfExtents()});

                dbr.AddOBB(sphere.meshes[0].GetOBB_Mat4(&ballModel));
                dbr.AddFrustum(*m_Camera3D.GetFrustum()); // Draw the full frustum shape
                // m_Player.GetRigidbody()->getAabb(m_Player.GetRigidbody()->getWorldTransform(), min, max);
                for (auto &ctrlPair : LogicProcess.GetControllersMap())
                {
                    for (auto &ctrl : ctrlPair.second)
                    {
                        btRigidBody *rb = ctrl->GetRigidBody();
                        if (rb) // Check controller’s rigid body
                        {
                            btVector3 min, max;
                            rb->getCollisionShape()->getAabb(rb->getWorldTransform(), min, max);
                            dbr.AddAABB(glm::vec3(min.getX(), min.getY(), min.getZ()),
                                        glm::vec3(max.getX(), max.getY(), max.getZ()));
                        }

                        auto obj = ctrl->GetControlled();
                        if (obj)
                        {
                            btRigidBody *objRb = obj->GetRigidBody();
                            if (objRb) // Check controlled object’s rigid body
                            {
                                btVector3 min, max;
                                objRb->getCollisionShape()->getAabb(objRb->getWorldTransform(), min, max);
                                dbr.AddAABB(glm::vec3(min.getX(), min.getY(), min.getZ()),
                                            glm::vec3(max.getX(), max.getY(), max.getZ()));
                            }
                        }
                    }
                }

                for (auto &prop : *LogicProcess.GetProps())
                {
                    btRigidBody *rb = prop.get()->GetRigidbody();
                    if (rb) // Check controller’s rigid body
                    {
                        btVector3 min, max;
                        rb->getCollisionShape()->getAabb(rb->getWorldTransform(), min, max);
                        dbr.AddAABB(glm::vec3(min.getX(), min.getY(), min.getZ()),
                                    glm::vec3(max.getX(), max.getY(), max.getZ()));
                    }
                }

                // draw scene collider
                for (size_t i = 0; i < sceneColliderLines.size(); i += 2)
                {
                    dbr.AddLine(sceneColliderLines[i], sceneColliderLines[i + 1]);
                }

                dbr.Draw();
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

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

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            m_Window.SwapBuffers();
        }

        ImGuiLayer::Shutdown();

        return false;
    }
}