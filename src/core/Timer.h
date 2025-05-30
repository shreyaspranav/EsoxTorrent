#pragma once

#include "Base.h"
#include <chrono>

namespace Esox
{
    class Timer {
    public:
        Timer() { m_StartTime = std::chrono::high_resolution_clock::now(); }
        ~Timer() { Stop(); }

        void Reset() { m_StartTime = std::chrono::high_resolution_clock::now(); }

        void Stop()
        {
            auto endTime = std::chrono::high_resolution_clock::now();

            uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
            uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

            m_ElapsedTime = end - start;
        }

        uint64_t GetElapedTime() { return m_ElapsedTime; }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
        uint64_t m_ElapsedTime = 0;
    };
}