#pragma once

#include <string>

namespace common
{
	constexpr std::string_view GOOGLE_CHROME = "Google Chrome";
	constexpr std::string_view MICROSOFT_EDGE = "Microsoft Edge";
	constexpr std::string_view MOZILLA_FIREFOX = "Mozilla Firefox";
	constexpr std::string_view OPERA = "Opera";
	constexpr std::string_view YANDEX_BROWSER = "YandexBrowser";

	constexpr std::string_view GOOGLE_CHROME_PATH = "Google\\Chrome";
	constexpr std::string_view MICROSOFT_EDGE_PATH = "Microsoft\\Edge";
	constexpr std::string_view MOZILLA_FIREFOX_PATH = "Mozilla\\Firefox";
	constexpr std::string_view OPERA_PATH = "Opera Software\\Opera Stable";	
	constexpr std::string_view YANDEX_BROWSER_PATH = "Yandex\\YandexBrowser";

	constexpr std::string_view USER_DATA_DEFAULT = "User Data\\Default";

	constexpr char TEMP[] = "Temp";
	constexpr char SYSTEM[] = "System";
}