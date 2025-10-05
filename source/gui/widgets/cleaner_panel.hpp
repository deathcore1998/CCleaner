#pragma once

#include <memory>
#include <string>
#include <vector>

#include "gui/widgets/widget.hpp"

#include "common/browser_info.hpp"

namespace core
{
	class SystemCleaner;
}

namespace gui
{
	class CleanerPanel : public Widget
	{
	public:
		CleanerPanel();

		void draw() override;

	private:
		void drawBrowsersPanel();
		void drawBrowserItem( common::BrowserInfo& browser );
		void drawTempCleaningSettings();

		std::unique_ptr< core::SystemCleaner > m_systemCleaner = nullptr;
		std::vector< common::BrowserInfo > m_browsersInfo;
	};
}