#pragma once
#include "Base.h"
#include <filesystem>

namespace Esox
{
	// This struct defines the information of a torrent instance.
	struct TorrentInfo
	{
		// Compulsory data: [These data should not be empty]----------------------------------------------
		String name;
		uint32_t totalLength, pieceCount, pieceLength;

		String announceURL;

		char* pieceHashes;
		Size pieceHashesLength;

		// Optional data: [These data could be empty] ----------------------------------------------------
		Vector<Pair<String, Size>> filePathsAndSizes;

		Vector<String> announceList;

		// URLs for web seeding(HTTP and FTP servers):
		Vector<String> urlList;
		// --------------------------------------------------------------

		TorrentInfo(const String& name)
			:name(name)
		{}
		~TorrentInfo() { delete[] pieceHashes; }
	};

	// Class to decode from torrent files and magnet links:
	class TorrentInfoDecoder
	{
	public:
		// Decodes from a torrent file.
		static Ref<TorrentInfo> GetTorrentInfoFromTorrentFile(const std::filesystem::path& torrentFilePath);
		
		// Decodes from a magnet link.
		static Ref<TorrentInfo> GetTorrentInfoFromMagnetLink(const String& link);
	};
}