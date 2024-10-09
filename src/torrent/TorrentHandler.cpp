#include "TorrentHandler.h"

#include <fstream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <memory>

#include "Handle.h"

#include <bencode.hpp>

namespace Esox
{
	// All the data required from a torrent file.
	struct Torrent
	{
		// Information of a torrent (data in the "info" field) -----
		String name;
		uint32_t totalLength, pieceCount, pieceLength;
		
		Pair<String, uint32_t> filePathsAndSizes;
		
		char* pieceHashes;
		// ---------------------------------------------------------

		String announceURL;
		Vector<String> announceList;

		// Optional data: ------------------------------------------
		
		// URLs for web seeding(HTTP and FTP servers):
		Vector<String> urlList;
		
		// ---------------------------------------------------------
	};

	struct TorrentHandlerData
	{
		UnorderedMap<Handle, Torrent*> processingTorrents;
	}torrentHandlerState;

	// Private methods: -------------------------------------------------------------------------------------------------
	static Torrent* DecodeTorrentFile(const std::filesystem::path& torrentFilePath)
	{
		return nullptr;
	}

	// Implementation: -------------------------------------------------------------------------------------------------
	void TorrentHandler::Init()
	{
		AddTorrentFile("resources/torrent-files/ubuntu-iso.torrent", "");
	}

	void TorrentHandler::AddTorrentFile(const std::filesystem::path& torrentFilePath, const std::filesystem::path& destinationPath)
	{
		DecodeTorrentFile(torrentFilePath);
	}

	void TorrentHandler::Update(float timestep)
	{

	}
}