#include "TorrentHandler.h"

#include "Handle.h"
#include "TorrentInfoDecoder.h"

#include <filesystem>

#include <bencode.hpp>

namespace Esox
{
	struct Torrent
	{
		Ref<TorrentInfo> torrentData;
		std::filesystem::path filePath, destinationPath;
	};

	struct TorrentHandlerData
	{
		UnorderedMap<Handle, Torrent> processingTorrents;
	}torrentHandlerState;

	// Implementation: -------------------------------------------------------------------------------------------------
	void TorrentHandler::Init()
	{
		AddTorrentFile("resources/torrent-files/Fedora-Astronomy_KDE-Live-x86_64-40.torrent", ""); // TEMP
	}

	void TorrentHandler::AddTorrentFile(const std::filesystem::path& torrentFilePath, const std::filesystem::path& destinationPath)
	{
		Ref<TorrentInfo> torrentInfo = TorrentInfoDecoder::GetTorrentInfoFromTorrentFile(torrentFilePath);
		if (!torrentInfo)
		{
			ESOX_LOG_ERROR("Failed to retrieve torrent info!");
			return;
		}

		Handle handle; // Generate a new handle

		Torrent torrent;
		torrent.destinationPath = destinationPath;
		torrent.filePath = torrentFilePath;
		torrent.torrentData = torrentInfo;

		torrentHandlerState.processingTorrents.insert({ handle, torrent });
	}

	void TorrentHandler::Update(float timestep)
	{

	}
}