#include "cleaner_panel.hpp"

#include <imgui.h>
#include "core/system_cleaner.hpp"

gui::CleanerPanel::CleanerPanel() : Widget( "Cleaner Panel")
{
	m_systemCleaner = std::make_unique< core::SystemCleaner >();
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
			m_systemCleaner->cleanTemp();
		}
	}
	ImGui::End();
}
