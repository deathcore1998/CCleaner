#include "filesystem.hpp"

fs::path utils::FileSystem::getTempDir()
{
	fs::path path = fs::temp_directory_path();
	return path;
}

fs::path utils::FileSystem::getAppDataDir()
{
	const char* localAppData = std::getenv( "LOCALAPPDATA" );
	if ( localAppData )
	{
		return fs::path( localAppData );
	}
	return fs::path();
}
