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
    m_titleBar->draw();
    m_cleanerPanel->draw();
}