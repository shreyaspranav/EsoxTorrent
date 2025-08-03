#include "PeerCommunicationHandler.h"
#include "Base.h"
#include "Random.h"
#include "URL.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <asio.hpp>

namespace Esox
{
    // Constants: ------------------------------------------------------------------------------------------
    const std::string peerIdFileName = "peerid.bin";

    // -----------------------------------------------------------------------------------------------------
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

        if (!std::filesystem::exists(peerIdFileName)) {
            state.currentPeerID = GenerateSelfPeerID();

            // Serialize it. For now, just store it in a binary file on the root directory.
            std::ofstream ofs(peerIdFileName, std::ios::binary);
            ofs.write(state.currentPeerID.c_str(), state.currentPeerID.size() + 1); // Include the \0 character as well
            ofs.close();
        }
        else
        {
            // Read the first 21 bytes: 20 bytes peer id and \0 character
            std::ifstream ifs(peerIdFileName, std::ios::binary);

            char peerID[21];
            ifs.read(peerID, 21 * sizeof(char));
            state.currentPeerID = std::string(peerID);

            ESOX_LOG_INFO("Read Peer ID: %s", state.currentPeerID.c_str());
        }
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
