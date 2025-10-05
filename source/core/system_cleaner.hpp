#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace common
{
	struct BrowserInfo;
	struct TempInfo;
	struct SystemInfo;
}

namespace core
{
	class SystemCleaner
	{
	public:
		SystemCleaner();
		~SystemCleaner(){}

		void cleanBrowserCache( const common::BrowserInfo& browseName );
		void cleanTemp( const common::TempInfo& tempInfo );
		void cleanSystem( const common::SystemInfo& tempSystem );

		[[nodiscard]] std::vector< std::string > getInstalledBrowsers();
	private:
		void clearDir( const std::filesystem::path& pathDir );
	};
}