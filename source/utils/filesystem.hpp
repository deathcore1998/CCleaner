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

		fs::path getTempDir() const;
		fs::path getLocalAppDataDir() const;
		fs::path getRoamingAppDataDir() const;

		fs::path getUpdateCacheDir() const;
		fs::path getLogsDir() const;
		fs::path getPrefetchDir() const;

	private:
		FileSystem(){}

		FileSystem& operator=( const FileSystem& ) = delete;
		FileSystem( const FileSystem& ) = delete;

	};
}