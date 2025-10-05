#include "cleaner_panel.hpp"

#include <imgui.h>
#include "core/system_cleaner.hpp"
#include "common/scoped_guards.hpp"

gui::CleanerPanel::CleanerPanel() : Widget( "Cleaner Panel")
{
	m_systemCleaner = std::make_unique< core::SystemCleaner >();

	for ( std::string& browserName : m_systemCleaner->getInstalledBrowsers() )
	{
		m_browsersInfo.push_back( { std::move( browserName ) } );
	}
}

void gui::CleanerPanel::draw()
{
	ImGui::StyleGuard styleGuard( ImGuiCol_ChildBg, IM_COL32( 100, 100, 100, 255 ) );
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
		ImGui::Selectable( "Temp" );

		ImGui::TableNextColumn();
		drawBrowsersPanel();
	}
	ImGui::EndTable();

	ImGui::EndChild();
}

void gui::CleanerPanel::drawBrowsersPanel()
{
	for ( common::BrowserInfo& browserInfo : m_browsersInfo )
	{
		drawBrowserItem( browserInfo );	
	}
}

void gui::CleanerPanel::drawBrowserItem( common::BrowserInfo& browser )
{
	ImGui::IDGuard( browser.name );

	if ( ImGui::CollapsingHeader( browser.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		auto drawCheckBox = []( const char* label, bool& fl )
		{
			const float indent = 10.f;
			ImGui::Indent( indent );
			ImGui::Checkbox( label, &fl);
			ImGui::Unindent( indent );
		};

		drawCheckBox( "Internet Cache", browser.clearCache );
		drawCheckBox( "Internet History", browser.clearHistory );
		drawCheckBox( "Cookies", browser.clearCookies );
		drawCheckBox( "Download History", browser.clearDownloadHistory );
	}
}

void gui::CleanerPanel::drawTempCleaningSettings()
{
}
