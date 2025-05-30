#include "TorrentDataDecoder.h"
#include "Base.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <memory>

#include <bencode.hpp>
#include <openssl/sha.h>

namespace Esox
{
    Ref<TorrentData> TorrentDataDecoder::GetTorrentDataFromTorrentFile(const std::filesystem::path& torrentFilePath)
    {
        Ref<TorrentData> torrentData;
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

        // Decode the .torrent file + Calculate the "info" hash
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

                torrentData = std::make_shared<TorrentData>();
                torrentInfo = std::make_shared<TorrentInfo>(name);
                torrentData->info = torrentInfo;

                torrentInfo->pieceLength = static_cast<uint32_t>(std::get<bencode::integer>(infoDict["piece length"]));

                String pieceHashes = std::get<String>(infoDict["pieces"]);
                torrentInfo->pieceHashes = new char[pieceHashes.size()];
                memcpy(torrentInfo->pieceHashes, pieceHashes.c_str(), pieceHashes.size()); // Might be unsafe?
                
                torrentInfo->pieceCount = ceil(pieceHashes.size() / 20.0f);      // Because each piece hash is 20 byte SHA-1 hash
                torrentInfo->pieceHashesLength = pieceHashes.size();

                torrentInfo->totalSize = torrentInfo->pieceLength * torrentInfo->pieceCount;

                torrentData->announce = std::get<String>(rootDict["announce"]);
            }

            // Optional ones: -------------------------------------------------
            // TODO: Add more optional fields.
            {
                if (rootDict.find("announce-list") != rootDict.end())
                {
                    auto&& announceList = std::get<bencode::list>(rootDict["announce-list"]);
                    for (auto&& announceListEntry : announceList)
                    {
                        // announce-list is actually a list of a list
                        // The outer list contains list of trackers sorted w.r.t priority,
                        // The inner list contains the trackers with equal priority.
                        // For now, I will add the trackers in a single list.
                        // TODO: Add priority information along with the tracker.
                        auto&& trackerList = std::get<bencode::list>(announceListEntry);
                        for(auto&& trackerEntry : trackerList)
                            torrentData->announceList.push_back(std::get<String>(trackerEntry));
                    }
                }

                if (rootDict.find("url-list") != rootDict.end())
                {
                    auto&& urlList = std::get<bencode::list>(rootDict["url-list"]);
                    for (auto&& urlListEntry : urlList)
                        torrentData->urlList.push_back(std::get<String>(urlListEntry));
                }

                if (rootDict.find("created by") != rootDict.end())
                {
                    String createdBy = std::get<String>(rootDict["created by"]);
                    torrentData->createdBy = createdBy;
                }

                if (rootDict.find("comment") != rootDict.end())
                {
                    String comment = std::get<String>(rootDict["comment"]);
                    torrentData->comment = comment;
                }

                if (rootDict.find("creation date") != rootDict.end())
                {
                    uint32_t creationDate = std::get<bencode::integer>(rootDict["creation date"]);
                    torrentData->creationDate = creationDate;
                }

                // if there is the field called length, it is a single file torrent.
                if (infoDict.find("length") != infoDict.end())
                {
                    uint32_t totalSize = std::get<bencode::integer>(infoDict["length"]);
                    torrentInfo->totalSize = static_cast<size_t>(totalSize);
                }

                if (infoDict.find("files") != infoDict.end())
                {
                    auto&& fileList = std::get<bencode::list>(infoDict["files"]);
                    torrentInfo->totalSize = 0;

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
                        torrentInfo->totalSize += size;
                    }
                }
            }

            // Calculate the "info" hash
            String infoBencoded = bencode::encode(infoDict);
            SHA1(reinterpret_cast<const uint8_t*>(infoBencoded.c_str()), infoBencoded.length(), torrentData->infoHash);
        }

        delete[] fileBuffer;
        return torrentData;
    }
    Ref<TorrentData> TorrentDataDecoder::GetTorrentDataFromMagnetLink(const String& link)
    {
        // TODO: To be implemented.
        return nullptr;
    }
}
