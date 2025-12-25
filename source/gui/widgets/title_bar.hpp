#pragma once

#include <imgui.h>

namespace core
{
	class Window;
}

namespace gui
{
	struct DraggingData
	{
		bool isWindowDragging = false;
		ImVec2 startMouse = ImVec2( 0.f, 0.f );
		ImVec2 startWindow = ImVec2( 0.f, 0.f );
	};

	class TitleBar
	{
	public:
		TitleBar( core::Window& window );

		void draw();

	private:
		void drawButtons();
		void draggingWindow();

		ImVec2 getCursorScreenPos() const;

		DraggingData m_draggingData;
		core::Window& m_mainWindow;
	};
}