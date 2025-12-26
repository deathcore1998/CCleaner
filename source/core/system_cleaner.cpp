#include "system_cleaner.hpp"

#include <windows.h>

#include "common/cleaner_info.hpp"
#include "common/constants.hpp"
#include "core/task_manager.hpp"
#include "utils/filesystem.hpp"


namespace
{
	constexpr float EPS = 0.001f;
}

core::SystemCleaner::SystemCleaner()
{
	initializeBrowserData();

	TaskManager::instance().addTask( [ this ] ()
	{
		initializeSystemTempData();
	} );
}

std::vector< std::string > core::SystemCleaner::getInstalledBrowsers()
{
	std::vector< std::string > browserNames;
	for ( const auto& [ name, cleanItem ] : m_browsersCleanMap )
	{
		browserNames.push_back( name );
	}

	return browserNames;
}

common::Summary core::SystemCleaner::getSummary()
{
	std::scoped_lock lock( m_summaryMutex );
	m_currentState = common::CleanerState::IDLE;
	return m_summary;
}

void core::SystemCleaner::clear( const common::CleanTargets& cleanTargets )
{
	using clock = std::chrono::steady_clock;
	const auto startTime = clock::now();

	analysisTargets( cleanTargets );

	m_cleanedFiles = 0;

	TaskManager::instance().addTask( [ this, startTime, cleanTargets ] ()
	{
		// Awaiting analysis
		while ( TaskManager::instance().countActiveTasks() > 1 )
		{
			m_progress = ( float ) TaskManager::instance().countActiveTasks() / ( float ) countAnalysTasks;
		}
		
		// Start cleaning
		const uint64_t totalFiles = m_summary.totalFiles;
		if ( totalFiles != 0 )
		{
			clearTargets( cleanTargets );
			while ( TaskManager::instance().countActiveTasks() > 1 )
			{
				m_progress = m_cleanedFiles / totalFiles;
			}

			m_progress = 1.f;
		}

		const auto endTime = clock::now();
		const std::chrono::duration< float > elapsed = endTime - startTime;

		m_summary.type = common::SummaryType::CLEANING;
		m_summary.totalTime = elapsed.count();

		m_currentState = common::CleanerState::CLEANING_DONE;
	} );
}

void core::SystemCleaner::analysis( const common::CleanTargets& cleanTargets )
{
	using clock = std::chrono::steady_clock;

	const auto startTime = clock::now();
	analysisTargets( cleanTargets );

	TaskManager::instance().addTask( [ this, startTime ] ()
	{
		while ( TaskManager::instance().countActiveTasks() > 1 )
		{
			m_progress = ( float ) TaskManager::instance().countActiveTasks() / ( float ) countAnalysTasks;
		}

		const auto endTime = clock::now();
		const std::chrono::duration< float > elapsed = endTime - startTime;
		const float duration = elapsed.count();

		m_progress = 1.f;

		m_summary.type = common::SummaryType::ANALYSIS;
		m_summary.totalTime = duration < EPS ? 0.0f : duration;

		m_currentState = common::CleanerState::ANALYSIS_DONE;
	} );
}

common::CleanerState core::SystemCleaner::getCurrentState()
{
	return m_currentState;
}

float core::SystemCleaner::getCurrentProgress()
{
	return m_progress;
}

void core::SystemCleaner::initializeBrowserData()
{
	const fs::path local = utils::FileSystem::instance().getLocalAppDataDir();
	const fs::path roaming = utils::FileSystem::instance().getRoamingAppDataDir();

	auto isBrowserInstalled = [ & ] ( std::string_view folderName )
	{
		return fs::exists( local / folderName ) || fs::exists( roaming / folderName );
	};

	auto addBrowserInfo = [ this ] ( std::string_view browserName, 
									 const fs::path& basePath,
									 std::string cachePath = "Cache", 
									 std::string cockiesPath = "Network\\Cookies", 
									 std::string historyPath = "History" )
	{
		auto addGroup = [ this ] ( std::string_view browserName, Category category, const fs::path& path )
		{
			if ( fs::exists( path ) )
			{
				m_browsersCleanMap[ browserName.data() ].insert( { category, path } );
			}
		};

		addGroup( browserName, Category::CACHE, basePath / cachePath );
		addGroup( browserName, Category::COOKIES, basePath / cockiesPath );
		addGroup( browserName, Category::HISTORY, basePath / historyPath );
	};

	if ( isBrowserInstalled( common::GOOGLE_CHROME_PATH ) )
	{
		const fs::path base = local / common::GOOGLE_CHROME_PATH / common::USER_DATA_DEFAULT;
		addBrowserInfo( common::GOOGLE_CHROME, base );
	}

	if ( isBrowserInstalled( common::MOZILLA_FIREFOX_PATH ) )
	{
		const fs::path profilesRoot = roaming / common::MOZILLA_FIREFOX_PATH / "Profiles";
		if ( fs::exists( profilesRoot ) )
		{
			for ( auto& entry : fs::directory_iterator( profilesRoot ) )
			{
				if ( !entry.is_directory() )
				{
					continue;
				}

				const fs::path profilePath = entry.path();
				addBrowserInfo( common::MOZILLA_FIREFOX, profilePath, "cache2\\entries", "cookies.sqlite", "places.sqlite" );
			}
		}
	}

	if ( isBrowserInstalled( common::YANDEX_BROWSER_PATH ) )
	{
		const fs::path base = local / common::YANDEX_BROWSER_PATH / common::USER_DATA_DEFAULT;
		addBrowserInfo( common::YANDEX_BROWSER, base );
	}

	if ( isBrowserInstalled( common::MICROSOFT_EDGE_PATH ) )
	{
		const fs::path base = local / common::MICROSOFT_EDGE_PATH / common::USER_DATA_DEFAULT;
		addBrowserInfo( common::MICROSOFT_EDGE, base );
	}

	if ( isBrowserInstalled( common::OPERA_PATH ) )
	{
		const fs::path base = roaming / common::OPERA_PATH;
		addBrowserInfo( common::OPERA, base );
	}
}

void core::SystemCleaner::initializeSystemTempData()
{
	auto& fs = utils::FileSystem::instance();

	m_tempSystemCleanMap[ common::TEMP ].insert( { Category::TEMP_FILES, fs.getTempDir() } );
	m_tempSystemCleanMap[ common::TEMP ].insert( { Category::UPDATE_CACHE, fs.getUpdateCacheDir() });
	m_tempSystemCleanMap[ common::TEMP ].insert( { Category::LOGS, fs.getLogsDir() } );

	m_tempSystemCleanMap[ common::SYSTEM ].insert( { Category::PREFETCH, fs.getPrefetchDir() } );
}

void core::SystemCleaner::analysisTargets( const common::CleanTargets& cleanTargets )
{
	resetData();
	m_currentState = common::CleanerState::ANALYZING;

	analysisBrowsersInfo( cleanTargets.browsers );
	analysisTemporaryData( cleanTargets.temp );
	analysisSystemComponents( cleanTargets.system );
}

core::DirInfo core::SystemCleaner::processPath( const fs::path& pathDir, bool deleteFiles )
{
	core::DirInfo info {};

	auto processFile = [ &info, deleteFiles ] ( const fs::path& filePath, uint64_t fileSize )
	{
		try
		{
			bool shouldCount = !deleteFiles || fs::remove( filePath );
			if ( shouldCount )
			{
				++info.countFile;
				info.dirSize += fileSize;
			}
		}
		catch ( const fs::filesystem_error& ) {}
	};

	try
	{
		if ( fs::is_directory( pathDir ) )
		{
			for ( const auto& entry : fs::recursive_directory_iterator( pathDir, fs::directory_options::skip_permission_denied ) )
			{
				if ( entry.is_regular_file() )
				{
					processFile( entry.path(), entry.file_size() );
				}
			}
		}
		else if ( fs::is_regular_file( pathDir ) )
		{
			processFile( pathDir, fs::file_size( pathDir ) );
		}
	}
	catch ( const fs::filesystem_error& ) {}

	return info;
}

void core::SystemCleaner::analysisBrowsersInfo( const std::vector< common::BrowserInfo >& browsers )
{
	for ( const common::BrowserInfo& browserInfo : browsers )
	{
		if ( !browserInfo.isNeedClean() )
		{
			continue;
		}
		++countAnalysTasks;

		TaskManager::instance().addTask( [ this, browserInfo ] ()
		{
			const auto it = m_browsersCleanMap.find( browserInfo.name );
			const CleanGroup& cleanGroup = it->second;
			analysisOptions( browserInfo.cleanOptions, cleanGroup, browserInfo.name );
		} );
	}
}

void core::SystemCleaner::analysisTemporaryData( const common::TempInfo& tempInfo )
{
	if ( !tempInfo.isNeedClean() )
	{
		return;
	}

	++countAnalysTasks;

	TaskManager::instance().addTask( [ this, tempInfo ] ()
	{
		const auto it = m_tempSystemCleanMap.find( tempInfo.name );
		const CleanGroup& cleanGroup = it->second;
		analysisOptions( tempInfo.cleanOptions, cleanGroup, tempInfo.name );
	} );
}

void core::SystemCleaner::analysisSystemComponents( const common::SystemInfo& systemInfo )
{
	if ( !systemInfo.isNeedClean() )
	{
		return;
	}
	++countAnalysTasks;

	TaskManager::instance().addTask( [ this, systemInfo ] ()
	{
		const auto it = m_tempSystemCleanMap.find( systemInfo.name );
		const CleanGroup& cleanGroup = it->second;
		analysisOptions( systemInfo.cleanOptions, cleanGroup, systemInfo.name );
	} );
}

void core::SystemCleaner::analysisOptions( const common::CleanOptionsMap& cleanOptions, const CleanGroup& cleanGroup, const std::string& optionsName )
{
	for ( const auto& [ category, cleanOption ] : cleanOptions )
	{
		if ( !cleanOption.enabled )
		{
			continue;
		}

		if ( category == Category::RECYCLE_BIN )
		{
			core::DirInfo dirInfo {};

			SHQUERYRBINFO rbInfo {};
			rbInfo.cbSize = sizeof( SHQUERYRBINFO );

			const HRESULT hr = SHQueryRecycleBinA( nullptr, &rbInfo );
			if ( SUCCEEDED( hr ) )
			{
				dirInfo.countFile = static_cast< uint64_t >( rbInfo.i64NumItems );
				dirInfo.dirSize = static_cast< uint64_t >( rbInfo.i64Size );
			}

			accumulateResult( common::SYSTEM, cleanOption.displayName, dirInfo );
			continue;
		}

		auto group = cleanGroup.find( category );
		if ( group == cleanGroup.end() )
		{
			continue;
		}

		const core::DirInfo dirInfo = processPath( group->second );
		accumulateResult( optionsName, cleanOption.displayName, dirInfo );
	}
}

void core::SystemCleaner::accumulateResult( std::string itemName, std::string category, const core::DirInfo dirInfo )
{
	std::scoped_lock lock( m_summaryMutex );
	m_summary.totalFiles += dirInfo.countFile;
	m_summary.totalSize += dirInfo.dirSize;
	m_summary.results.push_back( { std::move( itemName ), std::move( category ), dirInfo.countFile, dirInfo.dirSize } );
}

void core::SystemCleaner::clearTargets( const common::CleanTargets& cleanTargets )
{
	resetData();
	m_currentState = common::CleanerState::CLEANING;

	cleanBrowsersInfo( cleanTargets.browsers );
	cleanTemporaryData( cleanTargets.temp );
	cleanSystemComponents( cleanTargets.system );
}

void core::SystemCleaner::cleanBrowsersInfo( const std::vector< common::BrowserInfo >& browsers )
{
	for ( const common::BrowserInfo& browserInfo : browsers )
	{
		if ( !browserInfo.isNeedClean() )
		{
			continue;
		}

		TaskManager::instance().addTask( [ this, browserInfo ] ()
		{
			const auto it = m_browsersCleanMap.find( browserInfo.name );
			const CleanGroup& cleanGroup = it->second;
			cleanOptions( browserInfo.cleanOptions, cleanGroup, browserInfo.name );
		} );
	}
}

void core::SystemCleaner::cleanTemporaryData( const common::TempInfo& tempInfo )
{
	if ( !tempInfo.isNeedClean() )
	{
		return;
	}

	TaskManager::instance().addTask( [ this, tempInfo ] ()
	{
		const auto it = m_tempSystemCleanMap.find( tempInfo.name );
		const CleanGroup& cleanGroup = it->second;
		cleanOptions( tempInfo.cleanOptions, cleanGroup, tempInfo.name );
	} );
}

void core::SystemCleaner::cleanSystemComponents( const common::SystemInfo& systemInfo )
{
	if ( !systemInfo.isNeedClean() )
	{
		return;
	}
	TaskManager::instance().addTask( [ this, systemInfo ] ()
	{
		const auto it = m_tempSystemCleanMap.find( systemInfo.name );
		const CleanGroup& cleanGroup = it->second;
		cleanOptions( systemInfo.cleanOptions, cleanGroup, systemInfo.name );
	} );
}

void core::SystemCleaner::cleanOptions( const common::CleanOptionsMap& cleanOptions, const CleanGroup& cleanGroup, const std::string& optionsName )
{
	for ( const auto& [ category, cleanOption ] : cleanOptions )
	{
		if ( !cleanOption.enabled )
		{
			continue;
		}

		if ( category == Category::RECYCLE_BIN )
		{
			core::DirInfo dirInfo;
			SHQUERYRBINFO rbInfo {};
			rbInfo.cbSize = sizeof( SHQUERYRBINFO );

			const HRESULT hr = SHQueryRecycleBinA( nullptr, &rbInfo );
			if ( SUCCEEDED( hr ) && rbInfo.i64NumItems > 0 )
			{
				dirInfo.countFile = static_cast< uint64_t >( rbInfo.i64NumItems );
				dirInfo.dirSize = static_cast< uint64_t >( rbInfo.i64Size );

				accumulateResult( common::SYSTEM, cleanOption.displayName, dirInfo );
				SHEmptyRecycleBinA( nullptr, nullptr, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND );
			}
			continue;
		}

		auto group = cleanGroup.find( category );
		if ( group == cleanGroup.end() )
		{
			continue;
		}

		const core::DirInfo dirInfo = processPath( group->second, true );
		accumulateResult( optionsName, cleanOption.displayName, dirInfo );
	}
}

void core::SystemCleaner::resetData()
{
	{
		std::lock_guard guard( m_summaryMutex );
		m_summary.results.clear();
		m_summary.totalFiles = 0;
		m_summary.totalSize = 0;
	}

	m_cleanedFiles = 0;

	countAnalysTasks = 0;
}