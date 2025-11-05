#include "system_cleaner.hpp"

#include <windows.h>

#include "common/cleaner_info.hpp"
#include "core/task_manager.hpp"
#include "utils/filesystem.hpp"


namespace
{
	// take out in constants file 
	constexpr std::string_view GOOGLE_CHORM = "Google Chrome";
	constexpr std::string_view MOZILLA_FIREFOX = "Mozilla Firefox";
	constexpr std::string_view YANDEX_BROWSER = "YandexBrowser";
	constexpr std::string_view MICROSOFT_EDGE = "Microsoft Edge";
	constexpr std::string_view OPERA = "Opera";

	constexpr std::string_view GOOGLE_CHORM_PATH = "Google\\Chrome";
	constexpr std::string_view MOZILLA_FIREFOX_PATH = "Mozilla\\Firefox";
	constexpr std::string_view YANDEX_BROWSER_PATH = "Yandex\\YandexBrowser";
	constexpr std::string_view MICROSOFT_EDGE_PATH = "Microsoft\\Edge";
	constexpr std::string_view OPERA_PATH = "Opera Software\\Opera Stable";

	constexpr std::string_view USER_DATA_DEFAULT = "User Data\\Default";

	constexpr const char* TEMP = "Temp";
	constexpr const char* SYSTEM = "System";
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

		m_progress = 1.f;

		m_summary.type = common::SummaryType::ANALYSIS;
		m_summary.totalTime = elapsed.count();

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
	// TODO not find Cookies !
	const fs::path local = utils::FileSystem::instance().getLocalAppDataDir();
	const fs::path roaming = utils::FileSystem::instance().getRoamingAppDataDir();

	auto isBrowserInstalled = [ & ] ( std::string_view folderName )
	{
		return fs::exists( local / folderName ) || fs::exists( roaming / folderName );
	};

	auto addBrowserInfo = [ this ] ( std::string_view browserName, 
									 const fs::path& basePath,
									 std::string cachePath = "Cache", 
									 std::string cockiesPath = "Cookies", 
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

	if ( isBrowserInstalled( GOOGLE_CHORM_PATH ) )
	{
		const fs::path base = local / GOOGLE_CHORM_PATH / USER_DATA_DEFAULT;
		addBrowserInfo( GOOGLE_CHORM, base );
	}

	if ( isBrowserInstalled( MOZILLA_FIREFOX_PATH ) )
	{
		const fs::path profilesRoot = roaming / MOZILLA_FIREFOX_PATH / "Profiles";
		if ( fs::exists( profilesRoot ) )
		{
			for ( auto& entry : fs::directory_iterator( profilesRoot ) )
			{
				if ( !entry.is_directory() )
				{
					continue;
				}

				const fs::path profilePath = entry.path();
				addBrowserInfo( MOZILLA_FIREFOX, profilePath, "cache2\\entries", "cookies.sqlite", "places.sqlite" );
			}
		}
	}

	if ( isBrowserInstalled( YANDEX_BROWSER_PATH ) )
	{
		const fs::path base = local / YANDEX_BROWSER_PATH / USER_DATA_DEFAULT;
		addBrowserInfo( YANDEX_BROWSER, base );
	}

	if ( isBrowserInstalled( MICROSOFT_EDGE_PATH ) )
	{
		const fs::path base = local / MICROSOFT_EDGE_PATH / USER_DATA_DEFAULT;
		addBrowserInfo( MICROSOFT_EDGE, base );
	}

	if ( isBrowserInstalled( OPERA_PATH ) )
	{
		const fs::path base = roaming / OPERA_PATH;
		addBrowserInfo( OPERA, base );
	}
}

void core::SystemCleaner::initializeSystemTempData()
{
	auto& fs = utils::FileSystem::instance();

	m_tempSystemCleanMap[ TEMP ].insert( { Category::TEMP_FILES, fs.getTempDir() } );
	m_tempSystemCleanMap[ TEMP ].insert( { Category::UPDATE_CACHE, fs.getUpdateCacheDir() });
	m_tempSystemCleanMap[ TEMP ].insert( { Category::LOGS, fs.getLogsDir() } );

	m_tempSystemCleanMap[ SYSTEM ].insert( { Category::PREFETCH, fs.getPrefetchDir() } );
}

void core::SystemCleaner::analysisTargets( const common::CleanTargets& cleanTargets )
{
	resetData();
	m_currentState = common::CleanerState::ANALYZING;

	for ( const common::BrowserInfo& browser : cleanTargets.browsers )
	{
		if ( !browser.isNeedClean() )
		{
			continue;
		}
		++countAnalysTasks;

		TaskManager::instance().addTask( [ this, browser ] ()
		{
			analysisBrowserCache( browser );
		} );
	}

	++countAnalysTasks;
	const common::TempInfo& tempInfo = cleanTargets.temp;
	TaskManager::instance().addTask( [ this, tempInfo ] ()
	{
		analysisTemp( tempInfo );
	} );

	++countAnalysTasks;
	const common::SystemInfo& systemInfo = cleanTargets.system;
	TaskManager::instance().addTask( [ this, systemInfo ] ()
	{
		analysisSystem( systemInfo );
	} );
}

void core::SystemCleaner::clearTargets( const common::CleanTargets& cleanTargets )
{
	m_currentState = common::CleanerState::CLEANING;
	// TODO mb take data from m_summary ?
}

core::DirInfo core::SystemCleaner::analysisPath( const fs::path& pathDir )
{
	core::DirInfo info {};
	try
	{
		if ( fs::is_directory( pathDir ) )
		{
			for ( const auto& entry : fs::recursive_directory_iterator(
				pathDir, fs::directory_options::skip_permission_denied ) )
			{
				if ( entry.is_regular_file() )
				{
					++info.countFile;
					info.dirSize += entry.file_size();
				}
			}
		}
		else if ( fs::is_regular_file( pathDir ) )
		{
			info.countFile = 1;
			info.dirSize = fs::file_size( pathDir );
		}
	}
	catch ( const fs::filesystem_error& ) {}

	return info;
}

void core::SystemCleaner::analysisBrowserCache( const common::BrowserInfo& browseInfo )
{
	const auto it = m_browsersCleanMap.find( browseInfo.name );
	if ( it == m_browsersCleanMap.end() )
	{
		return;
	}

	const CleanGroup& cleanGroup = it->second;
	for ( const auto& [ category, enabled ] : browseInfo.cleanOptions )
	{
		if ( !enabled )
		{
			continue;
		}	

		auto group = cleanGroup.find( category );
		if ( group == cleanGroup.end() )
		{
			continue;
		}		

		const core::DirInfo dirInfo = analysisPath( group->second );
		accumulateResult( browseInfo.name, category, dirInfo );
	}
}

void core::SystemCleaner::analysisTemp( const common::TempInfo& tempInfo )
{
	const auto it = m_tempSystemCleanMap.find( tempInfo.name );
	if ( it == m_tempSystemCleanMap.end() )
	{
		return;
	}

	for ( const auto& [ category, enabled ] : tempInfo.cleanOptions )
	{
		if ( !enabled )
		{
			continue;
		}

		const CleanGroup& cleanGroup = it->second;
		auto group = cleanGroup.find( category );
		if ( group == cleanGroup.end() )
		{
			continue;
		}

		const core::DirInfo dirInfo = analysisPath( group->second );
		accumulateResult( tempInfo.name, category, dirInfo );
	}
}

void core::SystemCleaner::analysisSystem( const common::SystemInfo& systemInfo )
{
	const auto it = m_tempSystemCleanMap.find( systemInfo.name );
	if ( it == m_tempSystemCleanMap.end() )
	{
		return;
	}

	for ( const auto& [ category, enabled ] : systemInfo.cleanOptions )
	{
		if ( !enabled )
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

			accumulateResult( SYSTEM, category, dirInfo );
			continue;
		}

		const CleanGroup& cleanGroup = it->second;
		auto group = cleanGroup.find( category );
		if ( group == cleanGroup.end() )
		{
			continue;
		}

		const core::DirInfo dirInfo = analysisPath( group->second );
		accumulateResult( systemInfo.name, category, dirInfo );
	}
}

void core::SystemCleaner::accumulateResult( std::string itemName, Category category, const core::DirInfo dirInfo )
{
	std::scoped_lock lock( m_summaryMutex );
	m_summary.totalFiles += dirInfo.countFile;
	m_summary.totalSize += dirInfo.dirSize;
	m_summary.results.push_back( { itemName, convertCategory( category ), dirInfo.countFile, dirInfo.dirSize } );
}

void core::SystemCleaner::clearDir( const fs::path& pathDir )
{
	for ( const auto& entry : fs::recursive_directory_iterator( pathDir ) )
	{
		try
		{

		}
		catch ( const std::exception& e )
		{
			// skip if access is denied
		}
	}
}

void core::SystemCleaner::cleanBrowserCache( const common::BrowserInfo& browseInfo )
{
	TaskManager::instance().addTask( [ this, browseInfo ] ()
	{
	} );
}

void core::SystemCleaner::cleanTemp( const common::TempInfo& tempInfo )
{
	/*if ( tempInfo.cleanTempFiles )
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
	}*/
}

void core::SystemCleaner::cleanSystem( const common::SystemInfo& systemInfo )
{
	/*if ( tempSystem.cleanPrefetch )
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
	}*/
}

std::string core::SystemCleaner::convertCategory( Category cleanCategory )
{
	switch ( cleanCategory )
	{
		case Category::CACHE:
			return "Cache";
		case Category::COOKIES:
			return "Cookies";
		case Category::HISTORY:
			return "History";
		case Category::TEMP_FILES:
			return "Temp files";
		case Category::UPDATE_CACHE:
			return "Update cache";
		case Category::LOGS:
			return "Logs";
		case Category::PREFETCH:
			return "Prefetch";
		case Category::RECYCLE_BIN:
			return "Recycle bin";
	}
	return std::string();
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
