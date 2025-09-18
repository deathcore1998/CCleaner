#pragma once

#include <memory>
#include <string>
#include <vector>

#include "gui/widgets/widget.hpp"


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
		std::unique_ptr< core::SystemCleaner > m_systemCleaner = nullptr;

		std::vector< std::string > m_browsersList;
	};
}