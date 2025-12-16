#pragma once

#include <filesystem>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/constants.hpp"

namespace common
{
	enum class CleanCategory
	{
		CACHE,
		COOKIES,
		HISTORY,
		TEMP_FILES,
		LOGS,
		UPDATE_CACHE,
		PREFETCH,
		RECYCLE_BIN,
	};

	enum class CleanerState
	{
		IDLE,
		ANALYZING,
		ANALYSIS_DONE,
		CLEANING,
		CLEANING_DONE
	};

	enum class SummaryType
	{
		NONE,
		ANALYSIS,
		CLEANING,
	};

	struct CleanOption
	{
		bool enabled = true;
		std::string displayName;
	};

	using CleanOptionsMap = std::unordered_map< CleanCategory, CleanOption >;

	struct BaseInfo
	{
		BaseInfo( std::string name ) : name ( std::move( name ) )
		{
		}

		bool isNeedClean() const noexcept
		{
			for ( const auto& [ enabled, displayName ] : cleanOptions | std::views::values )
			{
				if ( enabled )
				{
					return true;
				}	
			}
			return false;
		}

		std::string name;
		uint64_t textureID = 0;
		CleanOptionsMap cleanOptions;
	};

	struct BrowserInfo : public BaseInfo
	{
		BrowserInfo( std::string name, uint64_t textureID = 0 ) : BaseInfo( std::move( name ) )
		{
			cleanOptions =
			{
				{ CleanCategory::CACHE, { true, "Cache" } },
				{ CleanCategory::COOKIES, { true, "Cookies" } },
				{ CleanCategory::HISTORY, { true, "History" } },
			};
		}
	};

	struct TempInfo : public BaseInfo
	{
		TempInfo() : BaseInfo( common::TEMP )
		{
			cleanOptions =
			{
				{ CleanCategory::TEMP_FILES, { true, "Temp files" } },
				{ CleanCategory::UPDATE_CACHE, { true, "Update cache" } },
				{ CleanCategory::LOGS, { true, "Logs" } },
			};
		}
	};

	struct SystemInfo : public BaseInfo
	{
		SystemInfo() : BaseInfo( common::SYSTEM )
		{
			cleanOptions =
			{
				{ CleanCategory::PREFETCH, { true, "Prefetch" } },
				{ CleanCategory::RECYCLE_BIN, { true, "Recycle bin" } },
			};
		}
	};

	struct CleanResult
	{
		std::string propertyName;
		std::string categoryName;
		uint64_t cleanedFiles = 0;
		uint64_t cleanedSize = 0;
		uint64_t textureID = 0;
	};

	struct Summary
	{
		SummaryType type = SummaryType::NONE;

		float totalTime = 0.f;
		uint64_t totalFiles = 0;
		uint64_t totalSize = 0;

		std::vector< CleanResult > results;

		void reset()
		{
			type = SummaryType::NONE;
			totalTime = 0.f;
			totalFiles = 0;
			totalSize = 0;
			results.clear();
		}
	};

	struct CleanTargets
	{
		std::vector< BrowserInfo > browsers;
		TempInfo temp;
		SystemInfo system;
	};
}