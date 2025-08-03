#pragma once
#include "Base.h"

#include <asio.hpp>

namespace Esox
{
    class TCPHandler
    {
        using ConnectCallback  = std::function<void(void)>;

        // Parameters: 
        //      success: bool     -> false when the packet recieving failed.
        //      data:    uint8_t* -> pointer for the recieved data.
        //      size:    size_t   -> size of the packet recieved.
        using ResponseCallback = std::function<void(bool, uint8_t*, size_t)>;

    public:
        TCPHandler() = delete;
        TCPHandler(const Ref<asio::io_context>& ctx, const asio::ip::tcp::endpoint& ep);

        ~TCPHandler();

        void Start(ConnectCallback callback);
        void Stop();

        void SendPacket(
            uint8_t* data,
            size_t size,
            const ResponseCallback& callback,
            uint32_t timeoutMS = 2000,
            uint8_t maxRetries = 3
        );

    private:
        void StartRecieve();
        void HandleTimeout();
    
    private:
        Ref<asio::io_context> m_Ctx;
        asio::ip::tcp::endpoint m_Endpoint;

        uint8_t* m_SendBuffer;
        size_t m_SendBufferSize;

        uint8_t* m_RecieveBuffer;

        std::chrono::time_point<std::chrono::steady_clock> m_SendTime;
        asio::steady_timer m_TimeoutTimer;

        ResponseCallback m_ResponseCallback;

        asio::ip::tcp::socket m_Socket;
        bool m_Running, m_Connected;

        uint32_t m_TimeoutMS, m_CurrentTimeout;
        uint8_t m_MaxRetries, m_CurrentAttempt;
    };
}
