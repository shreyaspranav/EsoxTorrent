#include "PeerCommunicationHandler.h"
#include "Base.h"
#include "Random.h"
#include "URL.h"

#include <cstring>
#include <sstream>

#include <asio.hpp>

namespace Esox
{
    struct PeerCommunicationHandlerState
    {
        String currentPeerID;

        Ref<asio::io_context> ctx;
        asio::error_code ec;
    };

    static PeerCommunicationHandlerState state;

    // Implementation: -------------------------------------------------------------------------------------

    void PeerCommunicationHandler::SetIOContext(const Ref<asio::io_context>& ctx)
    {
        state.ctx = ctx;
    }

    String PeerCommunicationHandler::GetSelfPeerID()
    {
        // TODO: This function should generate a peer ID for the first time, serialize it and then
        // use the id, if the serialized id exist, return that ID itself.

        if (state.currentPeerID.empty())
            state.currentPeerID = GenerateSelfPeerID();
        return state.currentPeerID;
    }

    // Private Functions: ----------------------------------------------------------------------------------
    String PeerCommunicationHandler::GenerateSelfPeerID()
    {
        std::stringstream peerID;
        peerID << '-' << ESOX_CLIENT_ID;

        // peerID is then followed by 4 digit version number
        // first char - major version
        // second char - minor version
        // third and fourth char - patch version

        peerID << ESOX_VERSION_MAJOR << ESOX_VERSION_MINOR;
        if(ESOX_VERSION_PATCH / 10 == 0)
            peerID << 0;
        peerID << ESOX_VERSION_PATCH;

        peerID << '-';
        for (int i = 0; i < 12; i++)
            peerID << Random::AlphaNumeric();

        return peerID.str();
    }
}
