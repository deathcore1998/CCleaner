#pragma once

#include <memory>

#include "common/cleaner_info.hpp"

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
		void drawOptions();
		void drawBrowserSettings();
		void drawTempAndSystemSettings();
		void drawMain();

		void drawCheckbox( const char* label, bool& fl );
		void drawProgress();
		void drawResultCleaningOrAnalysis();

		std::unique_ptr< core::SystemCleaner > m_systemCleaner = nullptr;

		common::CleanTargets m_cleanTargets;
		common::Summary m_cleanSummary;

		ActiveContext m_activeContext = ActiveContext::TEMP_AND_SYSTEM;
	};
}