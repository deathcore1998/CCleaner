#include "title_bar.hpp"

#include <cmath>

#include <GLFW/glfw3.h>

namespace
{
	constexpr float TITLE_HEIGHT = 20.0f;
}

gui::TitleBar::TitleBar( GLFWwindow* window ) : m_mainWindow( window )
{
}

gui::TitleBar::~TitleBar()
{
}

void gui::TitleBar::draw()
{
	ImGui::BeginChild( "TitleBar", ImVec2( 0, TITLE_HEIGHT ), false );

	draggingWindow();

	ImGui::Text( "System Cleaner" );

	drawButtons();

	ImVec2 winPos = ImGui::GetWindowPos();
	ImVec2 winSize = ImGui::GetWindowSize();

	// FIXME add separet

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
		glfwSetWindowShouldClose( m_mainWindow, GLFW_TRUE );
	}

	ImGui::SameLine( viewportSize.x - 40.0f );
	if ( ImGui::Button( "_" ) )
	{
		glfwIconifyWindow( m_mainWindow );
	}
}

ImVec2 gui::TitleBar::getCursorScreenPos() const
{
	// cursor position in glfw
	double cursorPosX = 0.0;
	double cursorPosY = 0.0;
	glfwGetCursorPos( m_mainWindow, &cursorPosX, &cursorPosY );

	// left top window position glfw
	int winPosX = 0;
	int winPosY = 0;
	glfwGetWindowPos( m_mainWindow, &winPosX, &winPosY );

	int left = 0;
	int top = 0;
	int right = 0;
	int bottom = 0;
	glfwGetWindowFrameSize( m_mainWindow, &left, &top, &right, &bottom );

	ImVec2 cursorScreenPos( static_cast< float >( winPosX + left + static_cast< int >( std::lround( cursorPosX ) ) ),
							static_cast< float >( winPosY + top + static_cast< int >( std::lround( cursorPosY ) ) ) );
	return cursorScreenPos;
}

void gui::TitleBar::draggingWindow()
{
	if ( m_isWindowDragging )
	{
		if ( ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
		{
			const ImVec2 now = getCursorScreenPos();
			const ImVec2 delta = ImVec2( now.x - m_dragStartMouse.x, now.y - m_dragStartMouse.y );

			const float newX = m_dragStartWindow.x + delta.x;
			const float newY = m_dragStartWindow.y + delta.y;

			glfwSetWindowPos( m_mainWindow,
				static_cast< int >( std::lround( newX ) ),
				static_cast< int >( std::lround( newY ) ) );
		}
		else if ( ImGui::IsMouseReleased( ImGuiMouseButton_Left ) )
		{
			m_isWindowDragging = false;
		}
	}
	else if ( ImGui::IsWindowHovered() && ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
	{
		m_dragStartMouse = getCursorScreenPos();

		int winPosX = 0;
		int winPosY = 0;
		glfwGetWindowPos( m_mainWindow, &winPosX, &winPosY );
		m_dragStartWindow = ImVec2( static_cast< float >( winPosX ), static_cast< float >( winPosY ) );

		m_isWindowDragging = true;
	}
}