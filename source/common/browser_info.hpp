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
}