#pragma once
#include "Base.h"
#include <filesystem>

#include "Handle.h"

#include <asio/io_context.hpp>

namespace Esox
{
    class TorrentHandler
    {
    public:
        static void Init(const Ref<asio::io_context>& ctx);
        static void Shutdown();

        static void AddTorrentFile(const std::filesystem::path& torrentFilePath, const std::filesystem::path& destinationPath);

        static void Update(float timestep);
    private:
        static void TorrentInstanceThreadEP(Handle torrentHandle);
    };
}

