#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace core
{
	class SystemCleaner
	{
		struct BrowserInfo
		{
			std::string name;
			std::filesystem::path cachePath;
		};

	public:
		SystemCleaner();
		~SystemCleaner(){}

		void cleanTemp();
		void cleanBrowserCache( const std::string browseName );

		[[nodiscard]] std::vector< std::string > getInstalledBrowsers();
	private:
		void detectInstalledBrowsers();

		void clearDir( const std::filesystem::path& pathDir );

		std::vector< BrowserInfo > m_browsers;
	};
}