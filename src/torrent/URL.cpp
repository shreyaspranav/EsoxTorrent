#include "Base.h"
#include "URL.h"

namespace Esox
{
    URL URL::FromString(const String &url)
    {
        return URL(url);
    }

    URL::URL(const String& url)
    {
        // Get the protocol first
        if (url.starts_with("http"))
            m_Type = Type::HTTP;
        else if (url.starts_with("udp"))
            m_Type = Type::UDP;
        else if (url.starts_with("wss"))
            m_Type = Type::WSS;
        else
        {
            ESOX_LOG_ERROR("Non Supported URL: '%s' Must be a HTTP, UDP or a WSS URL.", url.c_str());
            ESOX_ASSERT(false);
        }
        bool portSet = false;

        // Get the hostname
        size_t hostNameStart = url.find("://") + 3;
        size_t hostNameEnd = url.find(':', hostNameStart);
        if (hostNameEnd == String::npos)
        {
            // There is no port number in the tracker URL, use defaults in case of HTTP and WSS.
            // Throw an error for UDP.
            portSet = true;
            switch (m_Type)
            {
            case Type::HTTP:
                m_Port = 80;
                ESOX_LOG_WARN("Port no. not specified, using default HTTP port: 80");
                break;
            case Type::WSS:
                m_Port = 443;
                ESOX_LOG_WARN("Port no. not specified, using default WSS port: 433");
                break;
            default:
                ESOX_LOG_ERROR("Port no. not specified in the URL: %s", url.c_str());
                ESOX_ASSERT(false);
            }

            hostNameEnd = url.find('/', hostNameStart);
            if (hostNameEnd == String::npos)
                hostNameEnd = url.size();
        }

        m_Host = url.substr(hostNameStart, hostNameEnd - hostNameStart);

        size_t pathStart = url.find('/', hostNameEnd);
        if (!portSet)
        {
            String portNoStr = url.substr(hostNameEnd + 1, pathStart - hostNameEnd + 1);
            m_Port = std::atoi(portNoStr.c_str());
        }
        // Assuming there is query parameters in the URL:
        m_Location = url.substr(pathStart + 1, url.size() - pathStart + 1);
    }
}
