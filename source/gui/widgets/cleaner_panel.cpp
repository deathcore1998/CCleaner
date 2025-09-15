#include "cleaner_panel.hpp"

#include <imgui.h>

gui::CleanerPanel::CleanerPanel() : Widget( "Cleaner Panel")
{
}

void gui::CleanerPanel::draw()
{
	const float offsetY = ImGui::GetCursorPosY();
	const ImVec2 panelSize = ImVec2( ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - offsetY);

	ImGui::SetNextWindowPos( ImVec2( 0, offsetY ) );
	ImGui::SetNextWindowSize( panelSize );

	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	ImGui::Begin( "Cleaner panel", nullptr, flags );
	{
		if ( ImGui::Button( "Clear" ) )
		{
			// 
		}
	}
	ImGui::End();
}
