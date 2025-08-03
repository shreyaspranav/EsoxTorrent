#pragma once

#include "Base.h"
#include "UDPHandler.h"
#include <asio/ip/udp.hpp>
#include <asio/io_context.hpp>
#include <cstdint>
#include <functional>

namespace Esox
{
    struct Endpoint
    {
        String host;
        uint16_t port;
    };

    struct TrackerResponse
    {
        bool responded;

        uint32_t txnID;
        uint32_t interval;
        uint32_t seeders, leechers;

        Vector<Endpoint> peers;
    };

    using TrackerResponseCallbackFn          = std::function<void(UnorderedMap<String, TrackerResponse>)>;
    using TrackerResponseCallbackInternalFn  = std::function<void(TrackerResponse)>;

    class TrackerCommunicationHandler
    {
    public:
        static void SetIOContext(const Ref<asio::io_context>& ctx);

        static void RequestTracker(
            const Vector<String>& announceList,
            uint8_t* infohash,
            size_t downloaded,
            size_t uploaded,
            size_t total,
            const TrackerResponseCallbackFn& callback
        );

    private:
        static void RequestTrackerUDP(
            const URL& announce,
            uint8_t* infohash,
            size_t downloaded,
            size_t uploaded,
            size_t total,
            const TrackerResponseCallbackInternalFn& callback
        );
        // TODO: FIXME
        static void RequestTrackerHTTP(
            const URL& announce,
            uint8_t* infohash,
            size_t downloaded,
            size_t uploaded,
            size_t total,
            const TrackerResponseCallbackInternalFn& callback
        );
    };
}
