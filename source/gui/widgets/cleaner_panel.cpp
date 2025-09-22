#include "cleaner_panel.hpp"

#include <imgui.h>
#include "core/system_cleaner.hpp"

gui::CleanerPanel::CleanerPanel() : Widget( "Cleaner Panel")
{
	m_systemCleaner = std::make_unique< core::SystemCleaner >();
	m_browsersList = std::move ( m_systemCleaner->getInstalledBrowsers() );
}

void gui::CleanerPanel::draw()
{
	// FIXME need RAII wrappers
	ImGui::PushStyleColor( ImGuiCol_ChildBg, IM_COL32( 100, 100, 100, 255 ) );
	ImGui::BeginChild( "Cleaner panel", ImVec2( 0, 0 ), false );

	constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
	ImGui::BeginTable( "CleanerTable", 3, tableFlags, ImVec2( 0, 0 ) );
	{
		ImGui::TableSetupColumn( "Options", ImGuiTableColumnFlags_WidthStretch, 1.0f );
		ImGui::TableSetupColumn( "Settings", ImGuiTableColumnFlags_WidthStretch, 1.0f );
		ImGui::TableSetupColumn( "Main", ImGuiTableColumnFlags_WidthStretch, 3.0f );

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Selectable( "Browsers" );

		ImGui::TableNextColumn();
		for ( const std::string browser : m_browsersList )
		{
			ImGui::Text( browser.c_str() );
		}
	}
	ImGui::EndTable();

	ImGui::EndChild();
	ImGui::PopStyleColor();
}
