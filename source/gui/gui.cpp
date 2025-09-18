#include "gui.hpp"

#include <imgui.h>

gui::Gui::Gui( GLFWwindow* window ) :
    m_window( window )
{
    m_titleBar = new TitleBar( m_window );
    m_cleanerPanel = new CleanerPanel();
}

gui::Gui::~Gui()
{

}

void gui::Gui::render()
{
    const ImVec2 viewportSize = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
    ImGui::SetNextWindowSize( viewportSize );

    ImGui::Begin( "MainWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings );

    m_titleBar->draw();
    m_cleanerPanel->draw();

    ImGui::End();
}