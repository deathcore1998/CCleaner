#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

int main()
{
	if ( !glfwInit() )
	{
		return 1;
	}  

	GLFWwindow* window = glfwCreateWindow( 800, 600, "ImGui + OpenGL", NULL, NULL );
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 );

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); ( void ) io;

	ImGui_ImplGlfw_InitForOpenGL( window, true );
	ImGui_ImplOpenGL3_Init( "#version 130" );

	// Main loop
	while ( !glfwWindowShouldClose( window ) )
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin( "Hello, world!" );
		ImGui::Text( "This ImGui in OpenGL :)" );
		ImGui::End();

		ImGui::Render();
		glViewport( 0, 0, 800, 600 );
		glClearColor( 0.2f, 0.3f, 0.4f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

		glfwSwapBuffers( window );
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow( window );
	glfwTerminate();
	return 0;
}
