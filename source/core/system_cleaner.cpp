#include "system_cleaner.hpp"

#include <windows.h>

#include "common/cleaner_info.hpp"
#include "common/constants.hpp"
#include "core/task_manager.hpp"
#include "utils/filesystem.hpp"


namespace
{
	constexpr std::string_view RECYCLE_BIN = "Recycle bin";
	constexpr std::string_view CACHE = "Cache";
	constexpr std::string_view HISTORY = "History";

	constexpr float EPS = 0.001f;
}

common::Summary core::SystemCleaner::getSummary()
{
	std::scoped_lock lock( m_summaryMutex );
	m_currentState = common::CleanerState::IDLE;
	return m_summary;
}

void core::SystemCleaner::clear( const CleaningItems& cleanTargets )
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

void core::SystemCleaner::analysis( const CleaningItems& cleanTargets )
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

core::CleaningItems core::SystemCleaner::collectCleaningItems()
{
	CleaningItems cleaningItems;
	initializeBrowserData( cleaningItems );
	initializeSystemTempData( cleaningItems );

	return cleaningItems;
}

void core::SystemCleaner::initializeBrowserData( CleaningItems& cleaningItems )
{
	const fs::path local = utils::FileSystem::instance().getLocalAppDataDir();
	const fs::path roaming = utils::FileSystem::instance().getRoamingAppDataDir();

	auto isBrowserInstalled = [ & ] ( std::string_view folderName )
	{
		return fs::exists( local / folderName ) || fs::exists( roaming / folderName );
	};

	auto addBrowserInfo = [ this, &cleaningItems ] ( std::string_view browserName,
									 const fs::path& basePath,
									 std::string_view cachePath = CACHE,
									 std::string_view cookiesPath = "Network\\Cookies",
									 std::string_view historyPath = HISTORY )
	{
		common::CleaningItem item( browserName.data(), common::ItemType::BROWSER );
		std::vector< std::pair< std::string_view, fs::path > > options =
		{
			{ CACHE, basePath / cachePath },
			{ "Cookies", basePath / cookiesPath},
			{ HISTORY, basePath / historyPath }
		};

		for ( const auto& [ displayName, fullPath ] : options )
		{
			if ( fs::exists( fullPath ) )
			{
				common::CleanOption option { .displayName = displayName.data() };
				m_cleanPathCache[ option.id ] = fullPath;
				item.cleanOptions.push_back( std::move( option ) );
			}
		}

		if ( !item.cleanOptions.empty() )
		{
			cleaningItems.push_back( std::move( item ) );
		}
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
				addBrowserInfo( common::MOZILLA_FIREFOX, profilePath, 
								"cache2\\entries", "cookies.sqlite", "places.sqlite" );
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

void core::SystemCleaner::initializeSystemTempData( CleaningItems& cleaningItems )
{
	auto& fs = utils::FileSystem::instance();

	const auto addCleaningItem = [ & ] ( std::string name, common::ItemType type, 
		const std::vector< std::pair< std::string_view, fs::path > >& options )
	{
		common::CleaningItem item( name, type );
		for ( const auto& [ displayName, fullPath ] : options )
		{
			common::CleanOption option { .displayName = displayName.data() };
			m_cleanPathCache[ option.id ] = fullPath;
			item.cleanOptions.push_back( std::move( option ) );			
		}
		cleaningItems.push_back( std::move( item ) );
	};

	addCleaningItem( common::TEMP, common::ItemType::TEMP,
	{
		{ "Temp files", fs.getTempDir() },
		{ "Update cache", fs.getUpdateCacheDir() },
		{ "Logs", fs.getLogsDir() }
	} );

	addCleaningItem( common::SYSTEM, common::ItemType::SYSTEM,
	{
		{ "Prefetch", fs.getPrefetchDir() },
		{ RECYCLE_BIN, "" }
	} );
}

void core::SystemCleaner::analysisTargets( const CleaningItems& cleaningItems )
{
	resetData();
	m_currentState = common::CleanerState::ANALYZING;

	for ( const common::CleaningItem& cleaningItem : cleaningItems )
	{
		if ( !cleaningItem.isNeedClean() )
		{
			continue;
		}
		++countAnalysTasks;
		
		TaskManager::instance().addTask( [ this, cleaningItem ] ()
		{
			analysisOptions( cleaningItem );
		} );
	}
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

void core::SystemCleaner::analysisOptions( const common::CleaningItem& cleaningItem )
{
	for ( const common::CleanOption& cleanOption : cleaningItem.cleanOptions )
	{
		if ( !cleanOption.enabled )
		{
			continue;
		}

		if ( cleanOption.displayName == RECYCLE_BIN )
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

		const core::DirInfo dirInfo = processPath( m_cleanPathCache[ cleanOption.id ] );
		accumulateResult( cleaningItem.name, cleanOption.displayName, dirInfo );
	}
}

void core::SystemCleaner::clearTargets( const CleaningItems& cleaningItems )
{
	resetData();
	m_currentState = common::CleanerState::CLEANING;

	for ( const common::CleaningItem& cleaningItem : cleaningItems )
	{
		if ( !cleaningItem.isNeedClean() )
		{
			continue;
		}

		TaskManager::instance().addTask( [ this, cleaningItem ] ()
		{
			cleanOptions( cleaningItem );
		} );
	}
}

void core::SystemCleaner::cleanOptions( const common::CleaningItem& cleaningItem )
{
	for ( const common::CleanOption& cleanOption : cleaningItem.cleanOptions )
	{
		if ( !cleanOption.enabled )
		{
			continue;
		}

		if ( cleanOption.displayName == RECYCLE_BIN )
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

		const core::DirInfo dirInfo = processPath( m_cleanPathCache[ cleanOption.id ], true );
		accumulateResult( cleaningItem.name, cleanOption.displayName, dirInfo );
	}
}

void core::SystemCleaner::accumulateResult( std::string itemName, std::string category, const core::DirInfo dirInfo )
{
	std::scoped_lock lock( m_summaryMutex );
	m_summary.totalFiles += dirInfo.countFile;
	m_summary.totalSize += dirInfo.dirSize;
	m_summary.results.push_back( { std::move( itemName ), std::move( category ), dirInfo.countFile, dirInfo.dirSize } );
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