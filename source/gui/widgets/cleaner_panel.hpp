#pragma once

#include <memory>

#include "common/cleaner_info.hpp"
#include "core/texture_manager.hpp"

namespace core
{
	class SystemCleaner;
}

namespace gui
{
	enum class ActiveContext : size_t
	{
		BROWSER,
		TEMP_AND_SYSTEM
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
		void drawOption( const std::string& optionsName, common::CleanOptionsMap& cleanOptions );
		void drawProgress();
		void drawResultCleaningOrAnalysis();

		void prepareResultsForDisplay();

		std::unique_ptr< core::SystemCleaner > m_systemCleaner = nullptr;
		common::CleanTargets m_cleanTargets;
		common::Summary m_cleanSummary;

		ActiveContext m_activeContext = ActiveContext::TEMP_AND_SYSTEM;

		core::TextureManager m_textureManager;
	};
}