#include <Window.h>
#include <iostream>
#include <DrawCallCounter.h>
#include <filesystem>

namespace Az
{
    bool Screen::AZCreateWindow(const char *title, uint32_t width, uint32_t height)
    {
        SetWorkingDirectory();

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

        SDL_SetGamepadEventsEnabled(true);

        // Set OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        m_WindowPtr = SDL_CreateWindow(title,
                                       width,
                                       height,
                                       SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        if (m_WindowPtr == NULL)
        {
#ifdef AZ_DEBUG
            std::cout << "The window was not created" << std::endl;

#endif
            return EXIT_FAILURE;
        }

        // Create an OpenGL context
        m_Context = SDL_GL_CreateContext(m_WindowPtr);
        if (!m_Context)
        {
#ifdef AZ_DEBUG
            std::cout << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
#endif
            SDL_DestroyWindow(m_WindowPtr);
            SDL_Quit();
            return EXIT_FAILURE;
        }

        // Load OpenGL functions using GLAD
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
#ifdef AZ_DEBUG
            std::cout << "Failed to initialize GLAD!" << std::endl;
#endif
            SDL_GL_DestroyContext(m_Context);
            SDL_DestroyWindow(m_WindowPtr);
            SDL_Quit();
            return EXIT_FAILURE;
        }

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

        // Enable VSync
        SDL_GL_SetSwapInterval(1);

        glViewport(0, 0, width, height);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glDepthMask(GL_TRUE);

        m_ShouldClose = false;

        m_Width = width;
        m_Height = height;

        return EXIT_SUCCESS;
    }

    void Screen::Close()
    {
        SDL_GL_DestroyContext(m_Context);
        SDL_DestroyWindow(m_WindowPtr);

        SDL_Quit();
    }

    void Screen::SwapBuffers()
    {
        SDL_GL_SwapWindow(m_WindowPtr);
        ResetDrawCallCounter();
    }

    bool Screen::SetWorkingDirectory()
    {
        std::filesystem::path currentDir = std::filesystem::current_path();
        std::filesystem::path parentDir = currentDir.parent_path();
        std::string folderName = currentDir.filename().string();

#ifdef AZ_DEBUG
        std::cout << "Current Folder: " << folderName << std::endl;
#endif

        if (folderName == "Debug")
        {
            currentDir = parentDir.parent_path();
            std::filesystem::current_path(currentDir);
#ifdef AZ_DEBUG
            std::cout << "Changed working directory to: " << std::filesystem::current_path() << std::endl;
#endif
        }

        else if (folderName == "Release")
        {
            currentDir = parentDir.parent_path();
            std::filesystem::current_path(currentDir);
#ifdef AZ_DEBUG
            std::cout << "Changed working directory to: " << std::filesystem::current_path() << std::endl;
#endif
        }
        else if (folderName == "build")
        {
            currentDir = parentDir;
            std::filesystem::current_path(currentDir);
#ifdef AZ_DEBUG
            std::cout << "Changed working directory to: " << std::filesystem::current_path() << std::endl;
#endif
        }
        return false;
    }

} // namespace Az
