#include "gui.hpp"

#include <imgui.h>

gui::Gui::Gui()
{

}

gui::Gui::~Gui()
{

}

void gui::Gui::render()
{
    ImGui::Begin( "Hello, world!" );
    ImGui::Text( "This ImGui in OpenGL :)" );
    ImGui::End();
}