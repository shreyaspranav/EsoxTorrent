#pragma once
#include "Base.h"
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <locale>
#include <sstream>

namespace Esox
{
    // This struct defines the information in the "info" dictionary.
    struct TorrentInfo
	{
		String name;
		uint32_t pieceCount, pieceLength;
        size_t totalSize;

		char* pieceHashes;
		Size pieceHashesLength;

		Vector<Pair<String, Size>> filePathsAndSizes;


		TorrentInfo(const String& name)
			:name(name)
		{}
		~TorrentInfo() { delete[] pieceHashes; }
	};

    // This struct defines the information of a torrent instance.
    struct TorrentData
    {
        // List of compulsory fields: [These data should not be empty]
        String announce;
        Ref<TorrentInfo> info;

        // Optional data: [These data could be empty]
        Vector<String> announceList, urlList;
        String createdBy, comment;
        uint32_t creationDate;

        void Log()
        {
#ifndef PRODUCTION
            ESOX_LOG_INFO("Torrent Data Log ################################################################");
            ESOX_LOG_INFO("Name: %s", info->name.c_str());
            ESOX_LOG_INFO("Created By: %s", createdBy.c_str());
            ESOX_LOG_INFO("Comment: %s", comment.c_str());

            std::stringstream ss;
            std::time_t creationDateTime = static_cast<std::time_t>(creationDate);
            std::tm* time_ptr = std::localtime(&creationDateTime);

            ss << std::put_time(time_ptr, "%Y-%m-%d %H:%M:%S");
            std::string creationDateString = ss.str();

            ESOX_LOG_INFO("Creation Date: %s", creationDateString.c_str());
            ESOX_LOG_INFO("Announce URL: %s", announce.c_str());

            ESOX_LOG_INFO("Announce URL List: %zu URL(s)", announceList.size());
            for (String url : announceList)
                ESOX_LOG_INFO("    %s", url.c_str());

            ESOX_LOG_INFO("Other URL List: %zu URL(s)", urlList.size());
            for (String url : urlList)
                ESOX_LOG_INFO("    %s", url.c_str());

            ESOX_LOG_INFO("Piece Length: %uB", info->pieceLength);
            ESOX_LOG_INFO("Piece Count: %u",  info->pieceCount);
            ESOX_LOG_INFO("Total Size: %zuB", info->totalSize);

            ESOX_LOG_INFO("File List: ", info->filePathsAndSizes.size());
            for (auto&& filePair: info->filePathsAndSizes)
                ESOX_LOG_INFO("    %s: %zuB", filePair.first.c_str(), filePair.second);

            ESOX_LOG_INFO("#################################################################################");
#endif
        }
    };

	// Class to decode from torrent files and magnet links:
	class TorrentDataDecoder
	{
	public:
		// Decodes from a torrent file.
		static Ref<TorrentData> GetTorrentDataFromTorrentFile(const std::filesystem::path& torrentFilePath);
		
		// Decodes from a magnet link.
		static Ref<TorrentData> GetTorrentDataFromMagnetLink(const String& link);
	};
}
