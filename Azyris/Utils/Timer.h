#pragma once

#include <chrono>

namespace Az
{
    class Timer
    {
    public:
        // Updates the delta time and accumulates the total time passed
        static void UpdateTime();

        // Returns the total time passed in seconds
        static double GetTime();

        static double deltaTime;
        static float fDeltaTime;

    private:
        static std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
        static std::chrono::time_point<std::chrono::high_resolution_clock> m_LastTime;
        static double m_TimePassed; // Total time passed in seconds
    };
}