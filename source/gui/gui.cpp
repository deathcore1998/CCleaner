#include "gui.hpp"

#include <imgui.h>

gui::Gui::Gui( GLFWwindow* window ) :
    m_window( window )
{
    m_titleBar = new TitleBar( m_window );
}

gui::Gui::~Gui()
{

}

void gui::Gui::render()
{
    m_titleBar->draw();

    ImGui::Begin( "Hello, world!" );
    ImGui::Text( "This ImGui in OpenGL :)" );
    ImGui::End();
}