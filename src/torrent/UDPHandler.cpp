#include "Base.h"
#include "asio/buffer.hpp"
#include "asio/detail/chrono.hpp"
#include "asio/ip/udp.hpp"
#include "asio/post.hpp"
#include "asio/registered_buffer.hpp"
#include "asio/steady_timer.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <endian.h>
#include <string>
#include "UDPHandler.h"

namespace Esox
{
    // Constants: ------------------------------------------------------------------------------------
    constexpr size_t   recieveBufferSize       = 16384;

    // Public methods: -------------------------------------------------------------------------------
    UDPHandler::UDPHandler(const Ref<asio::io_context>& ctx, const asio::ip::udp::endpoint& endpoint)
        :m_Ctx(ctx),
        m_Endpoint(endpoint),
        m_Socket(*ctx, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
        m_TimeoutTimer(*ctx),
        m_Running(false)
    {
        // non-blocking sockets operate asynchronously.
        // m_Socket.open(asio::ip::udp::v4());
        m_Socket.non_blocking(true);

        // Just setting some arbiterary values:
        m_MaxRetries = 1;
        m_TimeoutMS = 1000;

        m_CurrentAttempt = 1;
        m_CurrentTimeout = m_TimeoutMS;

        m_RecieveBuffer = new uint8_t[recieveBufferSize];
    }

    UDPHandler::~UDPHandler()
    {
        Stop();
        delete[] m_RecieveBuffer;
    }

    void UDPHandler::SendPacket(
        uint32_t txnID,
        uint8_t* data,
        size_t size,
        const ResponseCallback& callback,
        uint32_t timeoutMS,
        uint8_t maxRetries
    )
    {
        // If reciever is not running, just fail the operation.
        if(!m_Running)
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
        m_TxnID = txnID;
        m_Callback = callback;
        m_TimeoutMS = timeoutMS;
        m_MaxRetries = maxRetries;

        // Copy the data to make sure it is available during retransmission.
        m_SendBuffer = new uint8_t[size];
        std::memcpy(m_SendBuffer, data, size);
        m_SendBufferSize = size;

        m_CurrentAttempt = 1;
        m_CurrentTimeout = m_TimeoutMS;

        if(!m_Socket.is_open())
            m_Socket.open(asio::ip::udp::v4());

        m_Socket.async_send_to(
            asio::buffer(data, size),
            m_Endpoint,
            [this, txnID](auto&& ec, size_t size)
            {
                if(!ec)
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
                if(!ec)
                    HandleTimeout();
            }
        );
    }

    void UDPHandler::Start()
    {
        if(m_Running) return;
        m_Running = true;

        StartRecieve();
    }

    void UDPHandler::Stop()
    {
        m_Running = false;
        m_Socket.cancel();
        m_TimeoutTimer.cancel();
    }
    // Private methods: ------------------------------------------------------------------------------
    void UDPHandler::StartRecieve()
    {
        if(!m_Running) return;

        using namespace asio::ip;

        udp::endpoint ep;

        m_Socket.async_receive_from(
            asio::buffer(m_RecieveBuffer, recieveBufferSize),
            ep,
            [this, &ep](auto&& ec, size_t size)
            {
                if(!ec)
                {
                    ESOX_LOG_INFO("Successfully Recieved %lu bytes from %s", size, ep.address().to_string().c_str());

                    // Make sure that the transaction ID matches.
                    // Both the connect response and the announce response have their transaction ID 4 bytes offset from beginning.
                    uint32_t rxTxnID;
                    std::memcpy(&rxTxnID, m_RecieveBuffer + 4, sizeof(uint32_t));
                    rxTxnID = be32toh(rxTxnID);

                    if(rxTxnID == m_TxnID)
                    {
                        asio::post(
                            *m_Ctx,
                            [this, size]()
                            {
                                m_Callback(true, m_RecieveBuffer, size);
                            }
                        );
                        m_TimeoutTimer.cancel();
                    }
                }
                else
                {
                    ESOX_LOG_INFO("Failed to recieve from %s (%s)", ep.address().to_string().c_str(), ec.message().c_str());
                }
                // Keep on recieving until Stop() is called.
                StartRecieve();
            }
        );
    }

    void UDPHandler::HandleTimeout()
    {
        if(m_CurrentAttempt <= m_MaxRetries)
        {
            // decrement the retries, double the timeout
            m_CurrentAttempt++;
            m_CurrentTimeout += m_TimeoutMS;

            ESOX_LOG_WARN(
                "Resending Packet to %s, Attempt: %u, Timeout: %u",
                m_Endpoint.address().to_string().c_str(),
                m_CurrentAttempt,
                m_CurrentTimeout
            );
            m_Socket.async_send_to(
                asio::buffer(m_SendBuffer, m_SendBufferSize),
                m_Endpoint,
                [this](auto&& ec, size_t size)
                {
                    if(!ec)
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
            });
        }
        else
        {
            Stop();
            m_Callback(false, nullptr, 0);  // Call the callback such that the recieve is failed.
            delete m_SendBuffer;
        }
    }
}
