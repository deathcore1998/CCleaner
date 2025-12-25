#include "app.hpp"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "gui/gui.hpp"

namespace
{
	constexpr int HEIGHT = 800;
	constexpr int WIDTH = 600;
}

App::App() : m_window( HEIGHT, WIDTH )
{
	initGui();
}

App::~App()
{
	finiGui();
}

void App::run()
{
	gui::Gui gui( m_window );

	while ( !m_window.shouldClose() )
	{
		glfwPollEvents();

		// skip if window collapsed
		if ( m_window.getWindowAttrib() )
		{
			glfwWaitEvents();
			continue;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		gui.render();

		ImGui::Render();
		glViewport( 0, 0, 800, 600 );
		glClearColor( 0.2f, 0.3f, 0.4f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

		m_window.swapBuffers();
	}
}

void App::initGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL( m_window.getGlfwHandle(), true );
	ImGui_ImplOpenGL3_Init( "#version 130" );
}

void App::finiGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}