#include "cleaner_panel.hpp"

#include <string>

#include <imgui.h>

#include "common/scoped_guards.hpp"
#include "core/system_cleaner.hpp"

gui::CleanerPanel::CleanerPanel()
{
	m_systemCleaner = std::make_unique< core::SystemCleaner >();

	std::vector< std::string > installedBrowsers = m_systemCleaner->getInstalledBrowsers();
	for ( std::string& browserName : installedBrowsers )
	{
		m_cleanTargets.browsers.push_back( { std::move( browserName ) } );
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
		//FIXME no resizeble collumb width
		ImGui::TableSetupColumn( "Options", ImGuiTableColumnFlags_WidthStretch, 1.0f );
		ImGui::TableSetupColumn( "Settings", ImGuiTableColumnFlags_WidthStretch, 2.0f );
		ImGui::TableSetupColumn( "Main", ImGuiTableColumnFlags_WidthStretch, 3.0f );

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		drawOptions();

		{
			ImGui::TableNextColumn();
			const ImVec2 columnSize = ImGui::GetContentRegionAvail();
			ImGui::BeginChild( "OptionsColumn" );
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

		ImGui::TableNextColumn();
		drawMain();
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
	for ( common::BrowserInfo& browserInfo : m_cleanTargets.browsers )
	{
		drawBrowserItem( browserInfo );	
	}
}

void gui::CleanerPanel::drawBrowserItem( common::BrowserInfo& browser )
{
	ImGui::IDGuard guard( browser.name );

	if ( ImGui::CollapsingHeader( browser.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		std::unordered_map< common::CleanCategory, bool >& cleanOptions = browser.cleanOptions;
		drawCheckbox( "Internet Cache", cleanOptions[ common::CleanCategory::CACHE ]);
		drawCheckbox( "Internet History", cleanOptions[ common::CleanCategory::HISTORY ] );
		drawCheckbox( "Cookies", cleanOptions[ common::CleanCategory::COOKIES ] );
	}
}

void gui::CleanerPanel::drawTempAndSystemSettings()
{
	if ( ImGui::CollapsingHeader( "Temp", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		std::unordered_map< common::CleanCategory, bool >& cleanOptions = m_cleanTargets.temp.cleanOptions;
		drawCheckbox( "Temp Files", cleanOptions[ common::CleanCategory::TEMP_FILES ] );
		drawCheckbox( "Update Cache", cleanOptions[ common::CleanCategory::UPDATE_CACHE ] );
		drawCheckbox( "Logs", cleanOptions[ common::CleanCategory::LOGS ] );
	}

	if ( ImGui::CollapsingHeader( "System", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		std::unordered_map< common::CleanCategory, bool >& cleanOptions = m_cleanTargets.system.cleanOptions;
		drawCheckbox( "Prefetch", cleanOptions[ common::CleanCategory::PREFETCH ] );
		drawCheckbox( "Recycle Bin", cleanOptions[ common::CleanCategory::RECYCLE_BIN ] );
	}
}

void gui::CleanerPanel::drawMain()
{
	const float cursorPosX = ImGui::GetCursorPosX();
	const ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	const ImVec2 buttonSize = ImVec2( 100.f, 30.f );
	const float offset = 10.f;
	const float buttonPosY = contentAvail.y - buttonSize.y - offset;

	common::CleanerState currentSystemState = m_systemCleaner->getCurrentState();
	if ( currentSystemState == common::CleanerState::ANALYSIS_DONE ||
		 currentSystemState == common::CleanerState::CLEANING_DONE )
	{
		m_cleanSummary = m_systemCleaner->getSummary();
	}

	if ( m_cleanSummary.type != common::SummaryType::NONE ||
		 currentSystemState != common::CleanerState::IDLE )
	{
		drawProgress();
		drawResultCleaningOrAnalysis();
	}

	ImGui::SetCursorPos( ImVec2( cursorPosX + offset, buttonPosY ) );
	if ( ImGui::Button( "Analysis", buttonSize ) )
	{
		m_cleanSummary.reset();
		m_systemCleaner->analysis( m_cleanTargets );
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX( contentAvail.x + cursorPosX - offset - buttonSize.x );
	if ( ImGui::Button( "Clear", buttonSize ) )
	{
		m_cleanSummary.reset();
		m_systemCleaner->clear( m_cleanTargets );
	}
}

void gui::CleanerPanel::drawCheckbox( const char* label, bool& fl )
{
	const float indent = 10.f;
	ImGui::Indent( indent );
	ImGui::Checkbox( label, &fl );
	ImGui::Unindent( indent );
}

void gui::CleanerPanel::drawProgress()
{
	ImGui::StyleGuard progressStyle( ImGuiCol_PlotHistogram, IM_COL32( 0, 200, 0, 255 ) );
	const float length = ImGui::GetContentRegionAvail().x;
	ImGui::ProgressBar( m_systemCleaner->getCurrentProgress(), ImVec2( length, 20.f ) );
}

void gui::CleanerPanel::drawResultCleaningOrAnalysis()
{
	const bool isSummaryAnalysis = m_cleanSummary.type == common::SummaryType::ANALYSIS;
	ImGui::Text( isSummaryAnalysis ? "Analysis completed" : "Cleaning is complete" );
	ImGui::SameLine();	
	ImGui::Text( std::format( "({}s)", m_cleanSummary.totalTime ).c_str() );

	ImGui::TextUnformatted( isSummaryAnalysis ? "Will be cleared approximately:" : "Cleared:" );
	ImGui::SameLine();
	ImGui::Text( "%.2f MB", static_cast< float >( m_cleanSummary.totalSize ) / ( 1024.0f * 1024.0f ) );

	ImGui::Spacing();
	ImGui::Separator();

	const ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
	if ( ImGui::BeginTable( "CleanSummaryTable", 4, flags, ImVec2( 0, contentAvail.y - 100 ) ) )
	{
		for ( const auto& result : m_cleanSummary.results )
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted( result.propertyName.c_str() );

			ImGui::TableNextColumn();
			ImGui::TextUnformatted( result.categoryName.c_str() );

			ImGui::TableNextColumn();
			ImGui::Text( "%.2f", static_cast< float >( result.cleanedSize ) / ( 1024.0f * 1024.0f ) );

			ImGui::TableNextColumn();
			ImGui::Text( "%llu", static_cast< unsigned long long >( result.cleanedFiles ) );
		}
	}
	ImGui::EndTable();
}