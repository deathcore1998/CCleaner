#include "cleaner_panel.hpp"

#include <cmath>
#include <ranges>
#include <string>

#include <imgui.h>

#include "common/constants.hpp"
#include "common/scoped_guards.hpp"

namespace
{
	constexpr float ICON_SIZE = 16.f;
	constexpr float BUTTON_HEIGHT = 30.f;
	constexpr float VERTICAL_OFFSET = 20.f;
	constexpr float KILOBYTE = 1024.0f;
	constexpr float MEGABYTE = KILOBYTE * KILOBYTE;

	constexpr ImU32 GREEN_COLOR = IM_COL32( 0, 200, 0, 255 );

	std::string separateString( const std::string& str )
	{
		std::string result = str;
		for ( int i = result.size() - 3; i > 0; i -= 3 )
		{
			result.insert( i, " " );
		}
		return result;
	}

	void rightAlignedText( const std::string& text )
	{
		const float regionAvail = ImGui::GetContentRegionAvail().x;
		const float textSize = ImGui::CalcTextSize( text.c_str() ).x;
		ImGui::SetCursorPosX( ImGui::GetCursorPosX() + regionAvail - textSize );
		ImGui::Text( text.c_str() );
	}
}

gui::CleanerPanel::CleanerPanel()
{
	m_systemCleaner = std::make_unique< core::SystemCleaner >();

	std::vector< std::string > installedBrowsers = m_systemCleaner->getInstalledBrowsers();
	for ( std::string& browserName : installedBrowsers )
	{
		m_cleanTargets.browsers.push_back( { browserName, m_textureManager.getTexture( browserName ) } );
	}

	m_cleanTargets.temp.textureID = m_textureManager.getTexture( m_cleanTargets.temp.name );
	m_cleanTargets.system.textureID = m_textureManager.getTexture( m_cleanTargets.system.name );
}

void gui::CleanerPanel::draw()
{
	ImGui::StyleGuard styleGuard( ImGuiCol_ChildBg, IM_COL32( 100, 100, 100, 255 ) );
	
	ImGui::Child cleanerPanel( "Cleaner panel" );

	drawTabBar();

	constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_SizingFixedFit;
	const ImVec2 tableSize = ImGui::GetContentRegionAvail();

	if ( auto table = ImGui::Table( "CleanerTable", 2, tableFlags, tableSize ) )
	{
		constexpr ImGuiTableColumnFlags columnFlags = ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed;
		ImGui::TableSetupColumn( "Settings", columnFlags, tableSize.x * 0.25f );
		ImGui::TableSetupColumn( "Main", columnFlags, tableSize.x * 0.75f );

		ImGui::TableNextColumn();
		{
			ImGui::Child options( "OptionsColumn" );
			drawOptions();
		}

		ImGui::TableNextColumn();
		drawMain();
	}
}

void gui::CleanerPanel::drawTabBar()
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

void gui::CleanerPanel::drawMain()
{
	const float cursorPosX = ImGui::GetCursorPosX();
	const ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	const ImVec2 buttonSize = ImVec2( 100.f, BUTTON_HEIGHT );
	const float buttonPosY = contentAvail.y - VERTICAL_OFFSET;

	const common::CleanerState currentSystemState = m_systemCleaner->getCurrentState();
	const bool isSystemNotIdle = currentSystemState != common::CleanerState::IDLE;
	if ( currentSystemState == common::CleanerState::ANALYSIS_DONE ||
		 currentSystemState == common::CleanerState::CLEANING_DONE )
	{
		prepareResultsForDisplay();
	}

	if ( m_cleanSummary.type != common::SummaryType::NONE || isSystemNotIdle )
	{
		drawProgress();
		drawResultCleaningOrAnalysis();
	}
	
	ImGui::DisabledGuard disabledGuard( isSystemNotIdle );
	ImGui::SetCursorPosY( buttonPosY );
	if ( ImGui::Button( "Analysis", buttonSize ) )
	{
		m_cleanSummary.reset();
		m_systemCleaner->analysis( m_cleanTargets );
	}

	ImGui::SameLine( contentAvail.x - buttonSize.x );
	if ( ImGui::Button( "Clear", buttonSize ) )
	{
		m_cleanSummary.reset();
		m_systemCleaner->clear( m_cleanTargets );
	}	
}

void gui::CleanerPanel::drawOptions()
{
	if ( m_activeContext == ActiveContext::TEMP_AND_SYSTEM )
	{
		common::TempInfo& tempInfo = m_cleanTargets.temp;
		drawOption( tempInfo.name, tempInfo.cleanOptions );

		common::SystemInfo& systemInfo = m_cleanTargets.system;
		drawOption( systemInfo.name, systemInfo.cleanOptions );
	}
	else if ( m_activeContext == ActiveContext::BROWSER )
	{
		for ( common::BrowserInfo& browserInfo : m_cleanTargets.browsers )
		{
			drawOption( browserInfo.name, browserInfo.cleanOptions );
		}
	}
}

void gui::CleanerPanel::drawOption( const std::string& optionsName, common::CleanOptionsMap& cleanOptions )
{
	ImGui::IDGuard guard( optionsName );
	ImGui::Image( m_textureManager.getTexture( optionsName ), ImVec2( ICON_SIZE, ICON_SIZE ) );	
	const float checkboxOffset = ImGui::GetCursorPosX();

	ImGui::SameLine();
	ImGui::AlignTextToFramePadding();
	ImGui::Text( optionsName.c_str() );
	{
		ImGui::IndentGuard indent( checkboxOffset );
		for ( common::CleanOption& cleanOption : cleanOptions | std::views::values )
		{
			ImGui::Checkbox( cleanOption.displayName.c_str(), &cleanOption.enabled );
		}
	}
}

void gui::CleanerPanel::drawProgress()
{
	ImGui::StyleGuard progressStyle( ImGuiCol_PlotHistogram, GREEN_COLOR );
	const float length = ImGui::GetContentRegionAvail().x;
	ImGui::ProgressBar( m_systemCleaner->getCurrentProgress(), ImVec2( length, 20.f ) );
}

void gui::CleanerPanel::drawResultCleaningOrAnalysis()
{
	{
		const bool isSummaryAnalysis = m_cleanSummary.type == common::SummaryType::ANALYSIS;

		ImGui::IndentGuard indent( 10.f );
		ImGui::Text( isSummaryAnalysis ? "Analysis completed" : "Cleaning is complete" );
		ImGui::SameLine();
		ImGui::Text( "(%.3fs)", m_cleanSummary.totalTime );

		ImGui::Text( isSummaryAnalysis ? "Will be cleared approximately:" : "Cleared:" );
		ImGui::SameLine();
		ImGui::Text( "%.2f MB", static_cast< float >( m_cleanSummary.totalSize ) / MEGABYTE );
	}

	ImGui::Spacing();
	ImGui::Separator();

	const ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	const float childHieght = contentAvail.y - BUTTON_HEIGHT - VERTICAL_OFFSET * 2;
	ImGui::Child result( "ResultCleaningOrAnalysis", ImVec2( 0, childHieght ) );
	
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;
	if ( auto table = ImGui::Table( "CleanSummaryTable", 3, flags ) )
	{
		constexpr ImGuiTableColumnFlags columnFlags = ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed;
		ImGui::TableSetupColumn( "##name", columnFlags, contentAvail.x * 0.5 );
		ImGui::TableSetupColumn( "##cleanedSize", columnFlags, contentAvail.x * 0.30 );
		ImGui::TableSetupColumn( "##cleanedFiles", columnFlags, contentAvail.x * 0.20 );

		for ( const auto& result : m_cleanSummary.results )
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			
			if ( result.textureID != ImTextureID_Invalid )
			{
				ImGui::Image( result.textureID, ImVec2( ICON_SIZE, ICON_SIZE ) );
				ImGui::SameLine();
			}
			ImGui::Text( "%s - %s", result.propertyName.c_str(), result.categoryName.c_str() );

			ImGui::TableNextColumn();
			const std::string cleanedSize = std::to_string( static_cast< int >( std::ceil( result.cleanedSize / KILOBYTE ) ) );
			rightAlignedText( separateString( cleanedSize ) + " KB" );

			ImGui::TableNextColumn();
			const std::string cleanedFiles = separateString( std::to_string( result.cleanedFiles ) );
			rightAlignedText( cleanedFiles );
		}
	}
}

void gui::CleanerPanel::prepareResultsForDisplay()
{
	m_cleanSummary = m_systemCleaner->getSummary();

	// sort results
	std::vector< common::CleanResult >& results = m_cleanSummary.results;
	std::sort( results.begin(), results.end(),
	[] ( const common::CleanResult& r1, const common::CleanResult& r2 )
	{
		return r1.propertyName < r2.propertyName;
	} );

	// assign icons
	for ( auto& result : m_cleanSummary.results )
	{
		result.textureID = m_textureManager.getTexture( result.propertyName );
	}
}