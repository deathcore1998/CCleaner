#include "gui.hpp"

#include <imgui.h>

gui::Gui::Gui()
{
    m_titleBar = new TitleBar();
}

gui::Gui::~Gui()
{

}

void gui::Gui::render()
{
    ImGui::Begin( "Hello, world!" );
    ImGui::Text( "This ImGui in OpenGL :)" );
    ImGui::End();

    m_titleBar->draw();
}