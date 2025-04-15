#include "TorrentHandler.h"

#include "Base.h"
#include "Handle.h"
#include "TorrentDataDecoder.h"

#include <filesystem>

#include <bencode.hpp>

namespace Esox
{
	struct Torrent
	{
		Ref<TorrentData> torrentData;
		std::filesystem::path filePath, destinationPath;
	};

	struct TorrentHandlerData
	{
		UnorderedMap<Handle, Torrent> processingTorrents;
	}torrentHandlerState;

	// Implementation: -------------------------------------------------------------------------------------------------
	void TorrentHandler::Init()
	{
		AddTorrentFile("resources/torrent-files/big-buck-bunny.torrent", ""); // TEMP
	}

	void TorrentHandler::AddTorrentFile(const std::filesystem::path& torrentFilePath, const std::filesystem::path& destinationPath)
	{
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

        torrentData->Log();
		torrentHandlerState.processingTorrents.insert({ handle, torrent });
	}

	void TorrentHandler::Update(float timestep)
	{

	}
}
