#include "app.hpp"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "gui/gui.hpp"

App::App()
{
	initWindow();
	initGui();
}

void App::run()
{
	gui::Gui gui( m_window );

	while ( !glfwWindowShouldClose( m_window ) )
	{
		glfwPollEvents();

		// skip if window collapsed
		if ( glfwGetWindowAttrib( m_window, GLFW_ICONIFIED ) )
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

		glfwSwapBuffers( m_window );
	}
}

App::~App()
{
	cleanUp();
}

void App::initWindow()
{
	if ( !glfwInit() )
	{
		exit( 1 );
	}

	// disable up border
	glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );
	
	// create glfw window
	m_window = glfwCreateWindow( 800, 600, "", NULL, NULL );
	if ( !m_window )
	{
		glfwTerminate();
		exit( 1 );
	}

	centerWindow();

	glfwMakeContextCurrent( m_window );
	glfwSwapInterval( 1 );
}

void App::initGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL( m_window, true );
	ImGui_ImplOpenGL3_Init( "#version 130" );
}

void App::cleanUp()
{

}

void App::centerWindow()
{
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode( primary );

	int windowWidth;
	int	windowHeight;
	glfwGetWindowSize( m_window, &windowWidth, &windowHeight );

	const int posX = ( mode->width - windowWidth ) / 2;
	const int posY = ( mode->height - windowHeight ) / 2;
	glfwSetWindowPos( m_window, posX, posY );
}