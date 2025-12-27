#pragma once

#include <chrono>

namespace Az
{
    class Profiler
    {
    public:
        static void StartProfiling();

        static double EndProfiling();

        inline static double GetLastDuration() { return m_Duration; }

    private:
        static std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
        static std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime;

        static double m_Duration;

        static bool m_IsProfiling;
    };
}