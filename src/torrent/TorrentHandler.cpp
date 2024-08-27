#include "TorrentHandler.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <bencode.hpp>

namespace Esox
{
	void TorrentHandler::Init()
	{
		std::ifstream inputFileStream("resources/torrent-files/ubuntu-iso.torrent");
		// auto data = bencode::decode(inputFileStream);
	}
}