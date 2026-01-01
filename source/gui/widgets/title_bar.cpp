#include "title_bar.hpp"

#include <cmath>

#include "common/scoped_guards.hpp"
#include "core/window.hpp"

namespace
{
	constexpr float TITLE_HEIGHT = 20.0f;
	constexpr char CLOSE[] = "X";
	constexpr char MINIMIZE[] = "_";
}

gui::TitleBar::TitleBar( core::Window& window ) :
	m_mainWindow( window )
{}

void gui::TitleBar::draw()
{
	ImGui::Child titleBar( "TitleBar", ImVec2( 0, TITLE_HEIGHT ) );
	ImGui::Text( "System Cleaner" );
	drawButtons();

	draggingWindow();
}

void gui::TitleBar::drawButtons()
{
	constexpr ImVec2 buttonSize = ImVec2( 20.0f, 0 );
	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	const float totalWidth = buttonSize.x * 2 + spacing;
	const float availableWidth = ImGui::GetContentRegionAvail().x;

	ImGui::SameLine( availableWidth - totalWidth );
	if ( ImGui::Button( MINIMIZE, buttonSize ) )
	{
		m_mainWindow.rollUp();
	}

	ImGui::SameLine();
	if ( ImGui::Button( CLOSE, buttonSize ) )
	{
		m_mainWindow.close();
	}
}

ImVec2 gui::TitleBar::getCursorScreenPos() const
{
	const auto [ cursorPosX, cursorPosY ] = m_mainWindow.getCursorPos();
	const auto [ winPosX, winPosY ] = m_mainWindow.getPos();
	const core::Window::FrameSize frameSize = m_mainWindow.getFrameSize();

	const ImVec2 cursorScreenPos( static_cast< float >( winPosX + frameSize.left + static_cast< int >( std::lround( cursorPosX ) ) ),
		static_cast< float >( winPosY + frameSize.top + static_cast< int >( std::lround( cursorPosY ) ) ) );
	return cursorScreenPos;
}

void gui::TitleBar::draggingWindow()
{
	if ( m_draggingData.isWindowDragging )
	{
		if ( ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
		{
			const ImVec2 currentPos = getCursorScreenPos();
			const ImVec2 delta = ImVec2( currentPos.x - m_draggingData.startMouse.x,
				currentPos.y - m_draggingData.startMouse.y );

			const float newX = m_draggingData.startWindow.x + delta.x;
			const float newY = m_draggingData.startWindow.y + delta.y;

			m_mainWindow.setPos( static_cast< int >( std::lround( newX ) ),
				static_cast< int >( std::lround( newY ) ) );
		}
		else if ( ImGui::IsMouseReleased( ImGuiMouseButton_Left ) )
		{
			m_draggingData.isWindowDragging = false;
		}
	}
	else if ( ImGui::IsWindowHovered() && ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
	{
		m_draggingData.startMouse = getCursorScreenPos();
		const auto [ winPosX, winPosY ] = m_mainWindow.getPos();

		m_draggingData.startWindow = ImVec2( static_cast< float >( winPosX ), static_cast< float >( winPosY ) );
		m_draggingData.isWindowDragging = true;
	}
}