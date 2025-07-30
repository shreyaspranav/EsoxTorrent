#pragma once
#include <Base.h>

#include "URL.h"
#include "asio/ip/tcp.hpp"
#include "asio/ip/udp.hpp"
#include "asio/steady_timer.hpp"

#include <asio.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>

namespace Esox
{
    using ResponseCallback = std::function<void(bool, uint8_t*, size_t)>;

    class UDPHandler
    {
    public:
        UDPHandler() = delete;
        UDPHandler(const Ref<asio::io_context>& ctx, const asio::ip::udp::endpoint& endpoint);

        ~UDPHandler();

        void SendPacket(
            uint32_t txnID,
            uint8_t* data,
            size_t size,
            const ResponseCallback& callback,
            uint32_t timeoutMS = 2000,
            uint8_t maxRetries = 3
        );

        void Start();
        void Stop();
    private:
        void StartRecieve();
        void HandleTimeout();

    private:
        Ref<asio::io_context> m_Ctx;
        asio::ip::udp::endpoint m_Endpoint;

        uint32_t m_TxnID;
        ResponseCallback m_Callback;
        uint32_t m_TimeoutMS;
        uint8_t m_MaxRetries;

        std::chrono::time_point<std::chrono::steady_clock> m_SendTime;
        asio::steady_timer m_TimeoutTimer;

        uint8_t* m_SendBuffer;
        size_t m_SendBufferSize;
        uint8_t* m_RecieveBuffer;

        asio::ip::udp::socket m_Socket;
        bool m_Running;

        uint32_t m_CurrentTimeout;
        uint8_t m_CurrentAttempt;
    };
}
