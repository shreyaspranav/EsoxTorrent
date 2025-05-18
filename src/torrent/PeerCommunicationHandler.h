#pragma once
#include "Base.h"
#include "asio/io_context.hpp"

namespace Esox
{
    class PeerCommunicationHandler
    {
    public:
        static void SetIOContext(const Ref<asio::io_context>& ctx);
        static String GetSelfPeerID();

    private:
        static String GenerateSelfPeerID();
    };
}

