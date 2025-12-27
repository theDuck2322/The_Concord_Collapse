#include "Profiler.h"
#include "Utils/Helpers.h"

namespace Az
{

    std::chrono::time_point<std::chrono::high_resolution_clock> Profiler::m_StartTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> Profiler::m_EndTime;

    double Profiler::m_Duration = 0;
    bool Profiler::m_IsProfiling = 0;

    void Profiler::StartProfiling()
    {
        AZ_Assert(m_IsProfiling != true, "Cant start a profiling because its already started!!");

        m_StartTime = std::chrono::high_resolution_clock::now();
        m_IsProfiling = true;
    }

    double Profiler::EndProfiling()
    {
        m_IsProfiling = false;
        m_EndTime = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> delta = m_EndTime - m_StartTime;

        m_Duration = delta.count();

        return m_Duration;
    }

}