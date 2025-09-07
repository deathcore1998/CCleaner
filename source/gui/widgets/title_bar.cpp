#include "title_bar.hpp"

#include "imgui.h"

gui::TitleBar::TitleBar()
{
}

gui::TitleBar::~TitleBar()
{
}

void gui::TitleBar::draw()
{
    ImGui::Begin( "Title bar!" );
    ImGui::Text( "Title bar!" );
    ImGui::End();
}
