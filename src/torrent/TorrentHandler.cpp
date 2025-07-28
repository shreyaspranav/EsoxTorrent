#include "TorrentHandler.h"

#include "Base.h"
#include "Handle.h"
#include "TorrentDataDecoder.h"
#include "asio/executor_work_guard.hpp"
#include "asio/io_context.hpp"
#include "torrent/PeerCommunicationHandler.h"
#include "torrent/TrackerCommunicationHandler.h"

#include <filesystem>
#include <memory>
#include <thread>

#include <bencode.hpp>
#include <asio.hpp>
#include <vector>

namespace Esox
{
    struct Torrent
    {
        Ref<TorrentData> torrentData;
        std::filesystem::path filePath, destinationPath;
        Size uploaded, downloaded;
    };

    struct TorrentHandlerData
    {
        Ref<asio::io_context> ctx;
        Ref<asio::executor_work_guard<asio::io_context::executor_type>> workGuard;

        Vector<std::thread> ctxThreads;

        UnorderedMap<Handle, Torrent> processingTorrents;
        String peerID;
    }torrentHandlerState;

    // Implementation: -------------------------------------------------------------------------------------------------
    void TorrentHandler::Init(const Ref<asio::io_context>& ctx)
    {
        torrentHandlerState.ctx = ctx;
        torrentHandlerState.workGuard =
            std::make_shared<asio::executor_work_guard<asio::io_context::executor_type>>(ctx->get_executor());

        // Get the number of concurrent threads to create a threadpool.
        uint32_t concurrentThreadCount = std::thread::hardware_concurrency();
        ESOX_LOG_WARN("Hardware Thread Count: %u", concurrentThreadCount);

        if(concurrentThreadCount == 0)
            concurrentThreadCount = 4; // default it to 4 cannot be determined.

        for (int i = 0; i < concurrentThreadCount; i++)
            torrentHandlerState.ctxThreads.push_back(
                std::thread([=](){
                    ctx->run();
                })
            );

        // Generate peerID.
        torrentHandlerState.peerID = PeerCommunicationHandler::GetSelfPeerID();

        TrackerCommunicationHandler::SetIOContext(ctx);
        PeerCommunicationHandler::SetIOContext(ctx);

        AddTorrentFile("resources/torrent-files/sintel.torrent", ""); // TEMP
    }

    void TorrentHandler::Shutdown()
    {
        torrentHandlerState.workGuard->reset();
        // TODO: Define a clear approach on the downloading torrents.
        torrentHandlerState.ctx->stop();
        for (auto&& thread : torrentHandlerState.ctxThreads)
            if(thread.joinable())
                thread.join();

    }

    void TorrentHandler::AddTorrentFile(const std::filesystem::path& torrentFilePath, const std::filesystem::path& destinationPath)
    {
        // TODO: Check if the same torrent is not being in the processing torrents. Only then add the torrent.
        Ref<TorrentData> torrentData = TorrentDataDecoder::GetTorrentDataFromTorrentFile(torrentFilePath);
        if (!torrentData)
        {
            ESOX_LOG_ERROR("Failed to retrieve torrent info!");
            return;
        }

        Handle handle; // Generate a new handle

        Torrent torrent;
        torrent.destinationPath = destinationPath;
        torrent.filePath = torrentFilePath;
        torrent.torrentData = torrentData;
        torrent.uploaded = 0;
        torrent.downloaded = 0;

        torrentData->Log();

        torrentHandlerState.processingTorrents.insert({ handle, torrent });
        TorrentHandler::TorrentInstanceThreadEP(handle);
    }

    void TorrentHandler::Update(float timestep)
    {

    }

    // Private Functions: -------------------------------------------------------------------------------------
    void TorrentHandler::TorrentInstanceThreadEP(Handle torrentHandle)
    {
        Torrent& currentTorrent = torrentHandlerState.processingTorrents[torrentHandle];
        Ref<TorrentData> currentTorrentData = currentTorrent.torrentData;

        TrackerCommunicationHandler::RequestTracker(
            currentTorrentData->announceList.empty() ?
                std::vector<String>{ currentTorrentData->announce } : currentTorrentData->announceList,
            currentTorrentData->infoHash,
            currentTorrent.downloaded,
            currentTorrent.uploaded,
            currentTorrentData->info->totalSize,
            [](UnorderedMap<String, TrackerResponse> response)
            {
                for(auto&& pair : response)
                    ESOX_LOG_INFO("URL: %s, Tracker Count: %lu", pair.first.c_str(), pair.second.peers.size());
            }
        );
    }
}
