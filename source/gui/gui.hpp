#pragma once

#include <GLFW/glfw3.h>

#include "widgets/cleaner_panel.hpp"
#include "widgets/title_bar.hpp"

namespace core
{
	class Window;
}

namespace gui
{
	class Gui
	{
	public:
		Gui( core::Window& window );

		void render();

	private:
		void initStyle();

		core::Window& m_window;
		TitleBar m_titleBar;
		CleanerPanel m_cleanerPanel;
	};
}