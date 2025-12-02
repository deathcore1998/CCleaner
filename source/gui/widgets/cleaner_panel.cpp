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
	ImGui::BeginChild( "Cleaner panel" );

	drawOptions();

	constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_SizingFixedFit;
	const ImVec2 tableSize = ImGui::GetContentRegionAvail();

	if ( auto table = ImGui::Table( "CleanerTable", 2, tableFlags, tableSize ) )
	{
		constexpr ImGuiTableColumnFlags columnFlags = ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed;
		ImGui::TableSetupColumn( "Settings", columnFlags, tableSize.x * 0.25f );
		ImGui::TableSetupColumn( "Main", columnFlags, tableSize.x * 0.75f );

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

		ImGui::TableNextColumn();
		drawMain();
	}

	ImGui::EndChild();
}

void gui::CleanerPanel::drawOptions()
{
	if ( ImGui::BeginTabBar( "CleanerTabs" ) )
	{
		if ( ImGui::BeginTabItem( "Temp/System" ) )
		{
			m_activeContext = ActiveContext::TEMP_AND_SYSTEM;
			ImGui::EndTabItem();
		}

		if ( ImGui::BeginTabItem( "Browsers" ) )
		{
			m_activeContext = ActiveContext::BROWSER;
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void gui::CleanerPanel::drawBrowserSettings()
{
	for ( common::BrowserInfo& browserInfo : m_cleanTargets.browsers )
	{
		ImGui::IDGuard guard( browserInfo.name );
		if ( ImGui::CollapsingHeader( browserInfo.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			std::unordered_map< common::CleanCategory, bool >& cleanOptions = browserInfo.cleanOptions;
			drawCheckbox( "Internet Cache", cleanOptions[ common::CleanCategory::CACHE ] );
			drawCheckbox( "Internet History", cleanOptions[ common::CleanCategory::HISTORY ] );
			drawCheckbox( "Cookies", cleanOptions[ common::CleanCategory::COOKIES ] );
		}
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

	const common::CleanerState currentSystemState = m_systemCleaner->getCurrentState();
	const bool isSystemNotIdle = currentSystemState != common::CleanerState::IDLE;
	if ( currentSystemState == common::CleanerState::ANALYSIS_DONE ||
		 currentSystemState == common::CleanerState::CLEANING_DONE )
	{
		m_cleanSummary = m_systemCleaner->getSummary();
	}

	if ( m_cleanSummary.type != common::SummaryType::NONE || isSystemNotIdle )
	{
		drawProgress();
		drawResultCleaningOrAnalysis();
	}

	{
		ImGui::DisabledGuard disabledGuard( isSystemNotIdle );
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

	if ( auto table = ImGui::Table( "CleanSummaryTable", 4, flags, ImVec2( 0, contentAvail.y - 100 ) ) )
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
}