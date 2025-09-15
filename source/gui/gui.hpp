#pragma once

#include <GLFW/glfw3.h>

#include "widgets/cleaner_panel.hpp"
#include "widgets/title_bar.hpp"

namespace gui
{
	class Gui
	{
	public:
		Gui( GLFWwindow* window );
		~Gui();

		void render();

	private:
		GLFWwindow* m_window = nullptr;
		TitleBar* m_titleBar = nullptr;
		CleanerPanel* m_cleanerPanel = nullptr;
	};
}