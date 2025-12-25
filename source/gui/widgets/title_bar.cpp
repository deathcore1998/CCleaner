#include "title_bar.hpp"

#include <cmath>

#include "core/window.hpp"

namespace
{
	constexpr float TITLE_HEIGHT = 20.0f;
}

gui::TitleBar::TitleBar( core::Window& window ) :
	m_mainWindow( window )
{}

void gui::TitleBar::draw()
{
	ImGui::BeginChild( "TitleBar", ImVec2( 0, TITLE_HEIGHT ) );

	ImGui::Text( "System Cleaner" );
	drawButtons();

	draggingWindow();

	ImGui::EndChild();
}

void gui::TitleBar::drawButtons()
{
	// FIXME disabeling dragging for the buttons
	const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

	// FIXME calculate button size
	ImGui::SameLine( viewportSize.x - 20.0f );
	if ( ImGui::Button( "X" ) )
	{
		m_mainWindow.close();
	}

	ImGui::SameLine( viewportSize.x - 40.0f );
	if ( ImGui::Button( "_" ) )
	{
		m_mainWindow.rollUp();
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