#pragma once

#include <memory>

#include "common/cleaner_info.hpp"
#include "core/system_cleaner.hpp"
#include "core/texture_manager.hpp"

namespace gui
{
	enum class ActiveContext : size_t
	{
		BROWSER,
		TEMP_AND_SYSTEM,
		CUSTOM
	};

	class CleanerPanel
	{
	public:
		CleanerPanel();

		void draw();

	private:
		void drawTabBar();
		void drawMain();

		void drawOptions();
		void drawOption( common::CleaningItem& cleaningItem );
		void drawCustomPathsMenu();
		void drawProgress();
		void drawResultCleaningOrAnalysis();

		void addCustomPath( std::string newCustomPath );

		void prepareResultsForDisplay();

		core::SystemCleaner m_systemCleaner;
		std::vector< common::CleaningItem > m_cleaningItems;
		common::Summary m_cleanSummary;

		ActiveContext m_activeContext = ActiveContext::TEMP_AND_SYSTEM;

		core::TextureManager m_textureManager;
	};
}