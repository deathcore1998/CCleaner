#pragma once

#include <GLFW/glfw3.h>
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
		Widget* m_titleBar = nullptr;
	};
}