#include <Timer.h>

namespace Az
{
    // Initialize static variables
    std::chrono::time_point<std::chrono::high_resolution_clock> Timer::m_StartTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> Timer::m_LastTime = Timer::m_StartTime;
    double Timer::m_TimePassed = 0.0;
    double Timer::deltaTime = 0.0;
    float Timer::fDeltaTime = 0.0;

    // Updates the delta time and accumulates the total time passed
    void Timer::UpdateTime()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> delta = currentTime - m_LastTime;

        deltaTime = delta.count(); // delta in seconds
        fDeltaTime = (float)deltaTime;
        m_LastTime = currentTime;
        m_TimePassed += deltaTime;
    }

    double Timer::GetTime()
    {
        return m_TimePassed;
    }
}