#include "system_cleaner.hpp"

#include <windows.h> // temporary
#include <fstream>

#include "common/cleaner_info.hpp"
#include "core/task_manager.hpp"
#include "utils/filesystem.hpp"


core::SystemCleaner::SystemCleaner()
{
}

void core::SystemCleaner::cleanBrowserCache( const common::BrowserInfo& browseName )
{


}

void core::SystemCleaner::cleanTemp( const common::TempInfo& tempInfo )
{
	if ( tempInfo.cleanTempFiles )
	{
		TaskManager::instance().addTask( [ this ] ()
		{
			auto tempDir = utils::FileSystem::instance().getTempDir();
			clearDir( tempDir );
		} );
	}
	if ( tempInfo.cleanUpdateCache )
	{
		TaskManager::instance().addTask( [ this ] ()
		{

		} );
	}
	if ( tempInfo.cleanLogs )
	{
		TaskManager::instance().addTask( [ this ] ()
		{

		} );
	}
}

void core::SystemCleaner::cleanSystem( const common::SystemInfo& tempSystem )
{
	if ( tempSystem.cleanPrefetch )
	{
		TaskManager::instance().addTask( [ this ] ()
		{

		} );
	}
	if ( tempSystem.cleanRecycleBin )
	{		
		TaskManager::instance().addTask( [ this ] ()
		{
			HRESULT hr = SHEmptyRecycleBinA( NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI );
		} );
	}
}

std::vector< std::string > core::SystemCleaner::getInstalledBrowsers()
{
	std::vector< std::string > browserNames;
	const fs::path local = utils::FileSystem::instance().getLocalAppDataDir();
	const fs::path roaming = utils::FileSystem::instance().getRoamingAppDataDir();

	auto checkBrowser = [ &browserNames, &local, &roaming ] ( std::string folder, std::string name, std::string displayName )
	{
		if ( fs::exists( local / folder / name ) || fs::exists( roaming / folder / name ) )
		{
			browserNames.push_back( std::move( displayName ) );
		}
	};

	checkBrowser( "Yandex", "YandexBrowser", "YandexBrowser" );
	checkBrowser( "Google", "Chrome", "Google Chrome" );
	checkBrowser( "Microsoft", "Edge", "Microsoft Edge" );
	checkBrowser( "Opera Software", "Opera Stable", "Opera" );
	checkBrowser( "Mozilla", "Firefox", "Mozilla Firefox" );

	return browserNames;
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
