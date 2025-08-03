#pragma once
#include "Base.h"

#include <filesystem>

namespace Esox
{
    class URL
    {
    public:
        enum class Type
        {
            NONE = 0,

            HTTP, UDP, WSS
        };
    public:
        URL() = delete;

        static URL FromString(const String& url);

        Type                   GetType()     const { return m_Type; }
        String                 GetHost()     const { return m_Host; }
        uint16_t               GetPort()     const { return m_Port; }
        std::filesystem::path  GetLocation() const { return m_Location; }
    private:
        URL(const String& url);

    private:
        Type m_Type;
        String m_Host;
        uint16_t m_Port;
        std::filesystem::path m_Location;
    };
}
