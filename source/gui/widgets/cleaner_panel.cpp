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
	ImGui::BeginChild( "Cleaner panel", ImVec2( 0, 0 ), false );

	if ( ImGui::Button( "Clear" ) )
	{
		m_systemCleaner->cleanTemp();
	}	

	ImGui::EndChild();
}
