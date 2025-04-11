#pragma once

#include <filesystem>

namespace Esox
{
	class TorrentHandler
	{
	public:
		static void Init();
		static void AddTorrentFile(const std::filesystem::path& torrentFilePath, const std::filesystem::path& destinationPath);

		static void Update(float timestep);
	};
}

