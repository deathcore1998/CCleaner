#include "filesystem.hpp"

fs::path utils::FileSystem::getTempDir() const
{
	return fs::temp_directory_path();
}

fs::path utils::FileSystem::getLocalAppDataDir() const
{
	const char* localAppData = std::getenv( "LOCALAPPDATA" );
	return fs::path( localAppData );
}

fs::path utils::FileSystem::getRoamingAppDataDir() const
{
	const char* localAppData = std::getenv( "APPDATA" );
	return fs::path( localAppData );
}

fs::path utils::FileSystem::getWindowsDir() const
{
	const char* winDir = std::getenv( "WINDIR" );
	return fs::path( winDir ? winDir : "C:\\Windows" );
}

fs::path utils::FileSystem::getUpdateCacheDir() const
{
	return getWindowsDir() / "SoftwareDistribution" / "Download";
}

fs::path utils::FileSystem::getLogsDir() const
{
	return getWindowsDir() / "Logs";
}

fs::path utils::FileSystem::getPrefetchDir() const
{
	return getWindowsDir() / "Prefetch";
}