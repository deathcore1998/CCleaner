#include "cleaner_panel.hpp"

#include <string>

#include <imgui.h>

#include "common/scoped_guards.hpp"
#include "core/system_cleaner.hpp"

gui::CleanerPanel::CleanerPanel() : Widget( "Cleaner Panel" )
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

	constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_Resizable;
	const ImVec2 tableSize = ImGui::GetContentRegionAvail();
	ImGui::BeginTable( "CleanerTable", 3, tableFlags, tableSize );
	{
		ImGui::TableSetupColumn( "Options", ImGuiTableColumnFlags_WidthStretch, 1.0f );
		ImGui::TableSetupColumn( "Settings", ImGuiTableColumnFlags_WidthStretch, 2.0f );
		ImGui::TableSetupColumn( "Main", ImGuiTableColumnFlags_WidthStretch, 3.0f );

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		drawOptions();

		ImGui::TableNextColumn();
		const ImVec2 columnSize = ImGui::GetContentRegionAvail();
		ImGui::BeginChild( "OptionsColumn", ImVec2( 0, 0 ), false, ImGuiWindowFlags_AlwaysVerticalScrollbar );
		{
			if ( m_activeContext == ActiveContext::TEMP_AND_SYSTEM )
			{
				drawTempAndSystemSettings();
			}
			else if ( m_activeContext == ActiveContext::BROWSER )
			{
				drawBrowserSettings();
			}
		}
		ImGui::EndChild();
	}
	ImGui::EndTable();

	ImGui::EndChild();
}

void gui::CleanerPanel::drawOptions()
{
	if ( ImGui::Selectable( "Temp/System", m_activeContext == ActiveContext::TEMP_AND_SYSTEM ) )
	{
		m_activeContext = ActiveContext::TEMP_AND_SYSTEM;
	}

	if ( ImGui::Selectable( "Browsers", m_activeContext == ActiveContext::BROWSER ) )
	{
		m_activeContext = ActiveContext::BROWSER;
	}
}

void gui::CleanerPanel::drawBrowserSettings()
{
	for ( common::BrowserInfo& browserInfo : m_browsersInfo )
	{
		drawBrowserItem( browserInfo );	
	}

	for ( common::BrowserInfo& browserInfo : m_browsersInfo )
	{
		drawBrowserItem( browserInfo );
	}
}

void gui::CleanerPanel::drawBrowserItem( common::BrowserInfo& browser )
{
	ImGui::IDGuard guard( browser.name );

	if ( ImGui::CollapsingHeader( browser.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		drawCheckbox( "Internet Cache", browser.clearCache );
		drawCheckbox( "Internet History", browser.clearHistory );
		drawCheckbox( "Cookies", browser.clearCookies );
		drawCheckbox( "Download History", browser.clearDownloadHistory );
	}
}

void gui::CleanerPanel::drawTempAndSystemSettings()
{
	if ( ImGui::CollapsingHeader( "Temp", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		drawCheckbox( "Temp Files", m_tempInfo.cleanTempFiles );
		drawCheckbox( "Update Cache", m_tempInfo.cleanUpdateCache );
		drawCheckbox( "Logs", m_tempInfo.cleanLogs );
	}

	if ( ImGui::CollapsingHeader( "System", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		drawCheckbox( "Prefetch", m_systemInfo.cleanPrefetch );
		drawCheckbox( "Recycle Bin", m_systemInfo.cleanRecycleBin );
	}
}

void gui::CleanerPanel::drawCheckbox( const char* label, bool& fl )
{
	const float indent = 10.f;
	ImGui::Indent( indent );
	ImGui::Checkbox( label, &fl );
	ImGui::Unindent( indent );
}