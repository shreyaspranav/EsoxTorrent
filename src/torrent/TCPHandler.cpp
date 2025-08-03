#include "Base.h"
#include "asio/detail/chrono.hpp"
#include "asio/post.hpp"
#include "TCPHandler.h"

namespace Esox 
{
    // Constants: ------------------------------------------------------------------------------------
    constexpr size_t   recieveBufferSize = 16384;

    // Public methods: -------------------------------------------------------------------------------
    TCPHandler::TCPHandler(const Ref<asio::io_context>& ctx, const asio::ip::tcp::endpoint& ep)
        :m_Ctx(ctx), 
        m_Endpoint(ep),
        m_Socket(*ctx, ep.protocol()),
        m_Running(false),
        m_Connected(false),
        m_TimeoutTimer(*ctx)
    {
        m_Socket.non_blocking(true);

        // Arbitrary values:
        m_MaxRetries = 1;
        m_CurrentAttempt = 1;
        m_CurrentTimeout = 1000;
        m_SendBuffer = nullptr;
        m_SendBufferSize = 0;

        m_RecieveBuffer = new uint8_t[recieveBufferSize];
    }

    TCPHandler::~TCPHandler()
    {
        Stop();
        delete[] m_RecieveBuffer;
    }

    void TCPHandler::SendPacket(uint8_t* data, size_t size, const ResponseCallback& callback, uint32_t timeoutMS, uint8_t maxRetries)
    {
        if (!m_Running)
        {
            asio::post(
                *m_Ctx,
                [this, size, &callback]()
                {
                    callback(false, m_RecieveBuffer, size);
                }
            );
            return;
        }

        m_TimeoutMS = timeoutMS;
        m_MaxRetries = maxRetries;

        m_CurrentAttempt = 1;
        m_CurrentTimeout = timeoutMS;

        m_ResponseCallback = callback;

        // For retransmission.
        m_SendBuffer = new uint8_t[size];
        std::memcpy(m_SendBuffer, data, size);
        m_SendBufferSize = size;

        if (!m_Socket.is_open())
            m_Socket.open(m_Endpoint.protocol());

        m_Socket.async_write_some(
            asio::buffer(data, size),
            [this](auto&& ec, size_t size)
            {
                if (!ec)
                    ESOX_LOG_INFO(
                        "Successfully sent %lu bytes to %s",
                        size,
                        m_Endpoint.address().to_string().c_str()
                    );
                else
                    ESOX_LOG_ERROR(
                        "Failed to send packet to %s, Error: %s",
                        m_Endpoint.address().to_string().c_str(),
                        ec.message().c_str()
                    );
            }
        );

        m_TimeoutTimer.expires_after(asio::chrono::milliseconds(timeoutMS));
        m_TimeoutTimer.async_wait(
            [this](auto&& ec)
            {
                if (!ec)
                    HandleTimeout();
                else
                   ESOX_LOG_ERROR(
                        "Async timer failed (%d: %s, %s)",
                        ec.value(),
                        ec.message().c_str(),
                        ec.category().name()
                    );
            }
        );
    }

    void TCPHandler::Start(ConnectCallback callback)
    {
        m_Socket.async_connect(
            m_Endpoint,
            [this, callback](auto&& ec)
            {
                if (!ec)
                {
                    ESOX_LOG_INFO("Successfully established a TCP connection to %s",
                        m_Endpoint.address().to_string().c_str()
                    );

                    m_Connected = true;
                    m_Running = true;
                    StartRecieve();
                    callback();
                }
                else
                {
                    ESOX_LOG_ERROR(
                        "Failed to establish a TCP connection to %s [%d: %s (%s)]",
                        m_Endpoint.address().to_string().c_str(),
                        ec.value(),
                        ec.message().c_str(),
                        ec.category().name()
                    );

                }
            }
        );
    }

    void TCPHandler::Stop()
    {
        m_Running = false;
        m_Socket.cancel();
        m_Socket.close();
    }


    // Private methods: ------------------------------------------------------------------------------
    void TCPHandler::StartRecieve()
    {
        if (!m_Running) return;

        m_Socket.async_read_some(
            asio::buffer(m_RecieveBuffer, recieveBufferSize),
            [&](auto&& ec, size_t size)
            {
                if (!ec)
                {
                    ESOX_LOG_INFO("Successfully Recieved %lu bytes from %s",
                        size,
                        m_Endpoint.address().to_string().c_str()
                    );

                    asio::post(
                        *m_Ctx,
                        [this, size]()
                        {
                             m_ResponseCallback(true, m_RecieveBuffer, size);
                        }
                    );

                    m_TimeoutTimer.cancel();
                }
                else
                    ESOX_LOG_INFO("Failed to recieve from %s (%s)",
                        m_Endpoint.address().to_string().c_str(), ec.message().c_str()
                    );

                StartRecieve();
            }
        );
    }

    void TCPHandler::HandleTimeout()
    {
        if (m_CurrentAttempt <= m_MaxRetries)
        {
            m_CurrentAttempt++;
            m_CurrentTimeout += m_TimeoutMS;

            ESOX_LOG_WARN(
                "Resending Packet to %s, Attempt: %u, Timeout: %ums",
                m_Endpoint.address().to_string().c_str(),
                m_CurrentAttempt,
                m_CurrentTimeout
            );

            m_Socket.async_write_some(
                asio::buffer(m_SendBuffer, m_SendBufferSize),
                [this](auto&& ec, size_t size)
                {
                    if (!ec)
                        ESOX_LOG_INFO(
                            "Successfully sent %lu bytes to %s",
                            size,
                            m_Endpoint.address().to_string().c_str()
                        );
                    else
                        ESOX_LOG_ERROR(
                            "Failed to send packet to %s, Error: %s",
                            m_Endpoint.address().to_string().c_str(),
                            ec.message().c_str()
                        );
                }
            );

            m_TimeoutTimer.expires_after(asio::chrono::milliseconds(m_CurrentTimeout));
            m_TimeoutTimer.async_wait(
                [this](auto&& ec)
                {
                    if(!ec)
                        HandleTimeout();
                }
            );
        }
        else
        {
            Stop();
            m_ResponseCallback(false, nullptr, 0);
        }
    }
}
