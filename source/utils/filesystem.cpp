#include "filesystem.hpp"

fs::path utils::FileSystem::getTempDir()
{
	return fs::temp_directory_path();
}

fs::path utils::FileSystem::getLocalAppDataDir()
{
	const char* localAppData = std::getenv( "LOCALAPPDATA" );
	return fs::path( localAppData );
}

fs::path utils::FileSystem::getRoamingAppDataDir()
{
	const char* localAppData = std::getenv( "APPDATA" );
	return fs::path( localAppData );
}