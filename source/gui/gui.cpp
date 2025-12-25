#include "gui.hpp"

#include <imgui.h>

#include "core/window.hpp"

gui::Gui::Gui( core::Window& window ) : 
    m_window( window ), m_titleBar ( window )
{
    initStyle();
}

void gui::Gui::render()
{
    const ImVec2 viewportSize = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
    ImGui::SetNextWindowSize( viewportSize );

    ImGui::Begin( "MainWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings );

    m_titleBar.draw();
    m_cleanerPanel.draw();

    ImGui::End();
}

void gui::Gui::initStyle()
{
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScrollbarSize = 10.0f;
}