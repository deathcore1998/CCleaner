#include "system_cleaner.hpp"

#include <windows.h> // temporary
#include <fstream>

#include "core/task_manager.hpp"
#include "utils/filesystem.hpp"

void core::SystemCleaner::cleanTemp()
{
	TaskManager::instance().addTask( [ this ]()
	{
		auto tempDir = utils::FileSystem::instance().getTempDir();
        clearDir( tempDir );
	} );
}

void core::SystemCleaner::cleanBrowserCache( const std::string browseName )
{

}

std::vector< std::string > core::SystemCleaner::getInstalledBrowsers()
{
    return std::vector< std::string >();
}

void core::SystemCleaner::clearDir( const std::filesystem::path& pathDir )
{
    std::ofstream logFile( "temp_files_to_delete.txt" );
    if ( !logFile.is_open() )
        return;

    for ( const auto& entry : fs::recursive_directory_iterator( pathDir ) )
    {
        try
        {
            if ( fs::is_regular_file( entry.path() ) )
            {
                logFile << entry.path().string() << "\n";
            }
        }
        catch ( const std::exception& e )
        {
            // skip if access is denied
        }
    }
}
