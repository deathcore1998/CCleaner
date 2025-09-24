#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace core
{
	class SystemCleaner
	{
	public:
		SystemCleaner();
		~SystemCleaner(){}

		void cleanTemp();
		void cleanBrowserCache( const std::string browseName );

		[[nodiscard]] std::vector< std::string > getInstalledBrowsers();
	private:
		void clearDir( const std::filesystem::path& pathDir );
	};
}