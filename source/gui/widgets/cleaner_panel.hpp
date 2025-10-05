#pragma once

#include <memory>
#include <vector>

#include "common/cleaner_info.hpp"

#include "gui/widgets/widget.hpp"


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

	class CleanerPanel : public Widget
	{
	public:
		CleanerPanel();

		void draw() override;

	private:
		void drawOptions();
		void drawBrowserSettings();
		void drawBrowserItem( common::BrowserInfo& browser );
		void drawTempAndSystemSettings();
		void drawMain();

		void drawCheckbox( const char* label, bool& fl);

		std::unique_ptr< core::SystemCleaner > m_systemCleaner = nullptr;

		std::vector< common::BrowserInfo > m_browsersInfo;
		common::TempInfo m_tempInfo;
		common::SystemInfo m_systemInfo;

		ActiveContext m_activeContext = ActiveContext::TEMP_AND_SYSTEM;
	};
}