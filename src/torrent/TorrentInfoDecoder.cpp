#include "TorrentInfoDecoder.h"

#include <fstream>
#include <cstring>

#include <bencode.hpp>

namespace Esox
{
	Ref<TorrentInfo> TorrentInfoDecoder::GetTorrentInfoFromTorrentFile(const std::filesystem::path& torrentFilePath)
	{
		Ref<TorrentInfo> torrentInfo;

		char* fileBuffer;
		Size fileBufferSize;

		// Read the .torrent file
		{
			// TODO: Do some better error handling,
			if (torrentFilePath.extension() != ".torrent")
			{
				ESOX_LOG_ERROR("%s is not a valid .torrent file!", torrentFilePath.string().c_str());
				return nullptr;
			}

			std::ifstream inputFileStream(torrentFilePath, std::ios::binary);
			if (!inputFileStream)
			{
				ESOX_LOG_ERROR("Cannot read %s", torrentFilePath.string().c_str());
				return nullptr;
			}

			inputFileStream.seekg(0, std::ios::end);
			fileBufferSize = inputFileStream.tellg();
			inputFileStream.seekg(0, std::ios::beg);

			fileBuffer = new char[fileBufferSize];
			inputFileStream.read(fileBuffer, fileBufferSize);
			inputFileStream.close();
		}

		// Decode the .torrent file
		{
			auto&& decodedData = bencode::decode(fileBuffer, fileBufferSize);
			auto&& rootDict = std::get<bencode::dict>(decodedData);

			// The compulsory fields are retrieved first, and then the optional ones

			// Retrive the data in the "info" fields(Because used in both compulsory and optional section)
			auto&& infoDict = std::get<bencode::dict>(rootDict["info"]);

			// Compulsory ones: ----------------------------------------------
			{
				// Retrieve the announce URL.
				String name = std::get<String>(infoDict["name"]);

				torrentInfo = std::make_shared<TorrentInfo>(name);
				torrentInfo->name = name;

				torrentInfo->announceURL = std::get<String>(rootDict["announce"]);
				torrentInfo->pieceLength = static_cast<uint32_t>(std::get<bencode::integer>(infoDict["piece length"]));

				std::string pieceHashes = std::get<String>(infoDict["pieces"]);
				torrentInfo->pieceHashes = new char[pieceHashes.size()];
				memcpy(torrentInfo->pieceHashes, pieceHashes.c_str(), pieceHashes.size()); // Might be unsafe?
			}

			// Optional ones: -------------------------------------------------
			// TODO: Add more optional fields.
			{
				if (rootDict.find("announce-list") != rootDict.end())
				{
					auto&& announceList = std::get<bencode::list>(infoDict["announce-list"]);
					for (auto&& announceListEntry : announceList)
						torrentInfo->announceList.push_back(std::get<String>(announceListEntry));
				}

				if (rootDict.find("url-list") != rootDict.end())
				{
					auto&& urlList = std::get<bencode::list>(infoDict["url-list"]);
					for (auto&& urlListEntry : urlList)
						torrentInfo->urlList.push_back(std::get<String>(urlListEntry));
				}

				if (infoDict.find("files") != infoDict.end())
				{
					auto&& fileList = std::get<bencode::list>(infoDict["files"]);
					for (auto&& fileEntry : fileList)
					{
						auto&& fileEntryDict = std::get<bencode::dict>(fileEntry);

						// The filePath is a list of folders, the last entry is the actual filename
						auto&& filePathList = std::get<bencode::list>(fileEntryDict["path"]);
						std::string filePath = "";

						for (int i = 0; i < filePathList.size(); i++)
							filePath =
							i != filePathList.size() - 1 ?
							filePath.append(std::get<String>(filePathList[i])) + "/" :
							filePath.append(std::get<String>(filePathList[i]));

						Size size = std::get<bencode::integer>(fileEntryDict["length"]);

						torrentInfo->filePathsAndSizes.push_back({ filePath, size });
					}
				}
			}
		}
		return torrentInfo;
	}
	Ref<TorrentInfo> TorrentInfoDecoder::GetTorrentInfoFromMagnetLink(const String& link)
	{
		return nullptr;
	}
}
