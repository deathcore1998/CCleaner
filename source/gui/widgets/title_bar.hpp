#pragma once

#include <imgui.h>

#include <GLFW/glfw3.h>

#include "gui/widgets/widget.hpp"

namespace gui
{
	class TitleBar : public Widget
	{
	public:
		TitleBar( GLFWwindow* window );
		~TitleBar();

		void draw() override;

	private:
		void drawButtons();
		void draggingWindow();

		ImVec2 getCursorScreenPos() const;

		// FIXME move to struct
		bool m_isWindowDragging = false;
		ImVec2 m_dragStartMouse;
		ImVec2 m_dragStartWindow;

		GLFWwindow* m_mainWindow = nullptr;
	};
}