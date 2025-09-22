#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace utils
{
	class FileSystem
	{
	public:
		[[nodiscard]] static FileSystem& instance()
		{
			static FileSystem fileSystem;
			return fileSystem;
		}

		fs::path getTempDir();
		fs::path getLocalAppDataDir();
		fs::path getRoamingAppDataDir();

	private:
		FileSystem(){}

		FileSystem& operator=( const FileSystem& ) = delete;
		FileSystem( const FileSystem& ) = delete;

	};
}