#include "Base.h"
#include "TrackerCommunicationHandler.h"

#include "URL.h"
#include "Random.h"
#include "torrent/PeerCommunicationHandler.h"
#include "torrent/UDPHandler.h"

#include <asio.hpp>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>


namespace Esox
{
    // Constants: ----------------------------------------------------------------------------------
    static constexpr uint16_t udpPacketTimeoutMS = 5000;
    // ---------------------------------------------------------------------------------------------

    bool recieveFlag = false;
    struct TrackerCommunicationHandleState
    {
        Ref<asio::io_context> ctx;

        Vector<UDPHandler*> udpHandlers;
    };

    struct TrackerResponseAggregationContext
    {
        std::mutex mtx;
        size_t expectedCount;

        UnorderedMap<String, TrackerResponse> responses;
        TrackerResponseCallbackFn callback;

        TrackerResponseAggregationContext()
            :expectedCount(0) {}
    };

    static TrackerCommunicationHandleState state;

    // Private static functions: -------------------------------------------------------------------
    // UDP Packets: ---------------------
    static void MakeUDPConnectRequestPacket(uint8_t* packet, uint32_t transactionID)
    {
        uint64_t txProtocolID = htobe64(0x41727101980);           // Magic constant
        uint32_t txAction = htobe32(0);                           // 0: Connect
        uint32_t txTransactionID = htobe32(transactionID);

        std::memcpy(packet, &txProtocolID, sizeof(uint64_t));    packet += sizeof(uint64_t);
        std::memcpy(packet, &txAction, sizeof(uint32_t));        packet += sizeof(uint32_t);
        std::memcpy(packet, &txTransactionID, sizeof(uint32_t)); packet += sizeof(uint32_t);
    }

    static uint64_t GetConnectionIDFromUDPConnectResponse(uint8_t* responsePacket, size_t size)
    {
        uint64_t rxConnectionID;
        std::memcpy(&rxConnectionID, responsePacket + sizeof(uint32_t) * 2, sizeof(uint64_t));
        return be64toh(rxConnectionID);
    }

    static void MakeUDPAnnounceRequestPacket(
        uint8_t* packet,
        uint64_t connectionID,
        uint32_t transactionID,
        uint8_t* infoHash,
        size_t downloaded,
        size_t uploaded,
        size_t total,
        uint32_t event,
        uint32_t key,
        int32_t numWant,
        uint16_t listenPort
        )
    {
        uint64_t txConnectionID                                       = htobe64(connectionID);
        uint32_t txAction                                             = htobe32(1);                // 1: announce
        uint32_t txTransactionID                                      = htobe32(transactionID);
        const char* txPeerID                                          = PeerCommunicationHandler::GetSelfPeerID().c_str();
        uint64_t txDownloaded                                         = htobe64(downloaded);
        uint64_t txLeft                                               = htobe64(total - downloaded);
        uint64_t txUploaded                                           = htobe64(uploaded);
        uint32_t txEvent                                              = htobe32(event);
        uint32_t txIPAddress                                          = htobe32(0);                // Tracket Autodetects it.
        uint32_t txKey                                                = htobe32(key);
        int32_t txNumWant                                             = htobe32(numWant);
        uint16_t txListenPort                                         = htobe16(listenPort);

        std::memcpy(packet, &txConnectionID, sizeof(uint64_t));       packet += sizeof(uint64_t);
        std::memcpy(packet, &txAction, sizeof(uint32_t));             packet += sizeof(uint32_t);
        std::memcpy(packet, &txTransactionID, sizeof(uint32_t));      packet += sizeof(uint32_t);
        std::memcpy(packet, infoHash, 20);                            packet += 20;
        std::memcpy(packet, txPeerID, 20);                            packet += 20;
        std::memcpy(packet, &txDownloaded, sizeof(uint64_t));         packet += sizeof(uint64_t);
        std::memcpy(packet, &txLeft, sizeof(uint64_t));               packet += sizeof(uint64_t);
        std::memcpy(packet, &txUploaded, sizeof(uint64_t));           packet += sizeof(uint64_t);
        std::memcpy(packet, &txEvent, sizeof(uint32_t));              packet += sizeof(uint32_t);
        std::memcpy(packet, &txIPAddress, sizeof(uint32_t));          packet += sizeof(uint32_t);
        std::memcpy(packet, &txKey, sizeof(uint32_t));                packet += sizeof(uint32_t);
        std::memcpy(packet, &txNumWant, sizeof(uint32_t));            packet += sizeof(uint32_t);
        std::memcpy(packet, &txListenPort, sizeof(uint16_t));
    }
    static void GetTrackerResponseData(TrackerResponse* response, uint8_t* data, size_t size)
    {
        uint32_t rxInterval, rxLeechers, rxSeeders;

        std::memcpy(&rxInterval, data + sizeof(uint32_t) * 2, sizeof(uint32_t));
        std::memcpy(&rxLeechers, data + sizeof(uint32_t) * 3, sizeof(uint32_t));
        std::memcpy(&rxSeeders,  data + sizeof(uint32_t) * 4, sizeof(uint32_t));

        response->interval = be32toh(rxInterval);
        response->leechers = be32toh(rxLeechers);
        response->seeders  = be32toh(rxSeeders);

        size_t peerCount = (size - 20) / 6;               // The header is 20 bytes and each IP and port pair is 6 bytes
        for(size_t i = 0; i < peerCount; ++i)
        {
            uint32_t ipAddress;
            uint16_t port;

            std::memcpy(&ipAddress, data + 20 * sizeof(uint8_t) + i * 6, sizeof(uint32_t));
            std::memcpy(&port,      data + 24 * sizeof(uint8_t) + i * 6, sizeof(uint16_t));

            ipAddress = be32toh(ipAddress);
            port = be16toh(port);
            asio::ip::address_v4 addr(ipAddress);

            response->peers.push_back({ addr.to_string(), port });
            ESOX_LOG_INFO("ID: %s, Port: %u", addr.to_string().c_str(), port);
        }
    }

    // UDP communication: ---------------
    static void BeginUDPTrackerComminucation(
        asio::ip::udp::endpoint endpoint,
        uint8_t* infohash,
        size_t downloaded,
        size_t uploaded,
        size_t total,
        TrackerResponseCallbackInternalFn callback)
    {
        // Send Connect Request.
        uint8_t reqPacket[16];
        uint32_t txnID = Random::Integer();
        MakeUDPConnectRequestPacket(reqPacket, txnID);

        UDPHandler* handler = new UDPHandler(state.ctx, endpoint);
        state.udpHandlers.push_back(handler);
        handler->Start();
        handler->SendPacket(
            txnID,
            reqPacket,
            16 * sizeof(uint8_t),
            [handler, txnID, infohash, downloaded, uploaded, total, callback](bool success, uint8_t* data, size_t size)
            {
                if(!success)
                {
                    // Callback an empty response
                    TrackerResponse response;
                    response.responded = false;
                    response.interval = 0;
                    response.leechers = 0;
                    response.seeders = 0;
                    callback(response);
                    return;
                }

                uint64_t connectionID = GetConnectionIDFromUDPConnectResponse(data, size);

                uint8_t announcePacket[98];
                MakeUDPAnnounceRequestPacket(
                    announcePacket,
                    connectionID,
                    txnID,
                    infohash,
                    downloaded,
                    uploaded,
                    total,
                    2,                     // Event: 0: none, 1: completed, 2: started
                    Random::Integer(),     // Key:
                    -1,                    // -1 for all
                    17769                  // listenPort
                );
                handler->Start();
                handler->SendPacket(
                    txnID,
                    announcePacket,
                    98 * sizeof(uint8_t),
                    [txnID, callback](bool success, uint8_t* data, size_t size)
                    {
                        TrackerResponse response;
                        response.responded = true;
                        response.txnID = txnID;

                        GetTrackerResponseData(&response, data, size);
                        asio::post(*state.ctx,
                        [callback, response]()
                            {
                                callback(response);
                            });
                    }
                );
            }
        );
    }
    // Implementation: -----------------------------------------------------------------------------
    void TrackerCommunicationHandler::SetIOContext(const Ref<asio::io_context>& ctx)
    {
        state.ctx = ctx;
    }

    void TrackerCommunicationHandler::RequestTracker(
        const Vector<String>& announceList,
        uint8_t* infohash,
        size_t downloaded,
        size_t uploaded,
        size_t total,
        const TrackerResponseCallbackFn& callback)
    {
        using namespace asio::ip;

        Ref<TrackerResponseAggregationContext> trackerResponseCtx = std::make_shared<TrackerResponseAggregationContext>();
        trackerResponseCtx->callback = callback;

        for (const String& url : announceList)
        {
            URL announce = URL::FromString(url);
            if (announce.GetType() == URL::Type::UDP)     // TEMP
                trackerResponseCtx->expectedCount++;
        }
        for (const String& url : announceList)
        {
            URL announce = URL::FromString(url);
            switch (announce.GetType())
            {
            case URL::Type::HTTP:
                // TODO: Implement me.
                break;
            case URL::Type::UDP:
                TrackerCommunicationHandler::RequestTrackerUDP(
                    announce,
                    infohash,
                    downloaded,
                    uploaded,
                    total,
                    [url, trackerResponseCtx](TrackerResponse response)
                    {
                        {
                            std::lock_guard<std::mutex> lock(trackerResponseCtx->mtx);
                            trackerResponseCtx->responses.insert({ url, response });
                        }
                        if (trackerResponseCtx->responses.size() == trackerResponseCtx->expectedCount)
                            asio::post(
                                *state.ctx,
                                [trackerResponseCtx]()
                                {
                                    trackerResponseCtx->callback(trackerResponseCtx->responses);
                                }
                            );
                    }
                );
                break;
            case URL::Type::WSS:
                // TODO: Implement me.
                break;
            case URL::Type::NONE:
                // TODO: Implement me.
                break;
            }
        }
    }

    // Private Functions: ----------------------------------------------------------------------------------
    void TrackerCommunicationHandler::RequestTrackerHTTP(
        const URL& announce,
        uint8_t* infohash,
        size_t downloaded,
        size_t uploaded,
        size_t total,
        const TrackerResponseCallbackInternalFn& callback)
    {
        // TODO: Implement this.
    }

    void TrackerCommunicationHandler::RequestTrackerUDP(
        const URL& announce,
        uint8_t* infohash,
        size_t downloaded,
        size_t uploaded,
        size_t total,
        const TrackerResponseCallbackInternalFn& callback)
    {
        using namespace asio::ip;
        // The process goes like this:
        // Send a connect request -> recieve connection ID -> send announce request -> recieve peers.

        Ref<udp::resolver> resolver = std::make_shared<udp::resolver>(*state.ctx);
        resolver->async_resolve(
            announce.GetHost(),
            std::to_string(announce.GetPort()),
            [resolver, announce, infohash, downloaded, uploaded, total, callback](auto&& ec, auto results)
            {
                if(!ec)
                    BeginUDPTrackerComminucation(*results.begin(), infohash, downloaded, uploaded, total, callback);
                else
                    ESOX_LOG_ERROR(
                        "Failed to resolve %s, (%d: %s, %s)",
                        announce.GetHost().c_str(),
                        ec.value(),
                        ec.message().c_str(),
                        ec.category().name()
                    );
            }
        );
    }
}
