#pragma once

#include <string>

namespace common
{
	struct BrowserInfo
	{
		std::string name;

		bool clearCache = true;
		bool clearCookies = true;
		bool clearHistory = true;
		bool clearDownloadHistory = true;
	};

	struct TempInfo
	{
		bool cleanTempFiles = true;
		bool cleanUpdateCache = true;
		bool cleanLogs = true;
	};

	struct SystemInfo
	{
		bool cleanPrefetch = true;
		bool cleanRecycleBin = true;
	};
}