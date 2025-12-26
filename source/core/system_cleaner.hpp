#pragma once

#include <atomic>
#include <mutex>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/cleaner_info.hpp"

namespace fs = std::filesystem;

namespace core
{
	struct DirInfo
	{
		uint64_t dirSize = 0;
		uint64_t countFile = 0;
	};

	using Category = common::CleanCategory;
	using CleanGroup = std::unordered_map< Category, fs::path >;
	using CleanMap = std::unordered_map< std::string, CleanGroup >;

	class SystemCleaner
	{
	public:
		SystemCleaner();
		~SystemCleaner(){}

		[[nodiscard]] std::vector< std::string > getInstalledBrowsers();
		[[nodiscard]] common::Summary getSummary();

		void clear( const common::CleanTargets& cleanTargets );
		void analysis( const common::CleanTargets& cleanTargets );

		common::CleanerState getCurrentState();
		float getCurrentProgress();
	private:
		void initializeBrowserData();
		void initializeSystemTempData();

		[[nodiscard]] DirInfo processPath( const fs::path& pathDir, bool deleteFiles = false );

		void analysisTargets( const common::CleanTargets& cleanTargets );		
		void analysisBrowsersInfo( const std::vector< common::BrowserInfo >& browsers );
		void analysisTemporaryData( const common::TempInfo& tempInfo );
		void analysisSystemComponents( const common::SystemInfo& systemInfo );
		void analysisOptions( const common::CleanOptionsMap& cleanOptions, const CleanGroup& cleanGroup, const std::string& optionsName );
		void accumulateResult( std::string itemName, std::string category, const core::DirInfo dirInfo );
	
		void clearTargets( const common::CleanTargets& cleanTargets );
		void cleanBrowsersInfo( const std::vector< common::BrowserInfo >& browsers );
		void cleanTemporaryData( const common::TempInfo& tempInfo );
		void cleanSystemComponents( const common::SystemInfo& systemInfo );
		void cleanOptions( const common::CleanOptionsMap& cleanOptions, const CleanGroup& cleanGroup, const std::string& optionsName );

		void resetData();

		std::atomic< uint64_t > m_cleanedFiles { 0 };
		std::atomic< float > m_progress { 0.f };
		std::atomic< size_t > countAnalysTasks { 0 };

		std::mutex m_summaryMutex;
		common::Summary m_summary;

		CleanMap m_browsersCleanMap;
		CleanMap m_tempSystemCleanMap;
		std::atomic < common::CleanerState > m_currentState = common::CleanerState::IDLE;
	};
}