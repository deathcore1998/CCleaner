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

fs::path utils::FileSystem::getUpdateCacheDir() const
{
	return fs::path();
}

fs::path utils::FileSystem::getLogsDir() const
{
	return fs::path();
}

fs::path utils::FileSystem::getPrefetchDir() const
{
	return fs::path();
}