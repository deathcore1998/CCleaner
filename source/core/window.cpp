#include "window.hpp"

core::Window::Window( int width, int height, const char* title )
{
	if ( !glfwInit() )
	{
		exit( 1 );
	}

	// disable up border
	glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );

	// create glfw window
	m_window = glfwCreateWindow( width, height, title, NULL, NULL );
	if ( !m_window )
	{
		glfwTerminate();
		exit( 1 );
	}

	centerOnScreen();

	glfwMakeContextCurrent( m_window );
	glfwSwapInterval( 1 );
}

core::Window::~Window()
{
	glfwDestroyWindow( m_window );
}

bool core::Window::shouldClose()
{
	return glfwWindowShouldClose( m_window );
}

bool core::Window::getWindowAttrib()
{
	return glfwGetWindowAttrib( m_window, GLFW_ICONIFIED );
}

void core::Window::swapBuffers()
{
	glfwSwapBuffers( m_window );
}

void core::Window::close()
{
	glfwSetWindowShouldClose( m_window, GLFW_TRUE );
}

void core::Window::rollUp()
{
	glfwIconifyWindow( m_window );
}

core::DoublePair core::Window::getCursorPos() const
{
	core::DoublePair cursorPos;
	glfwGetCursorPos( m_window, &cursorPos.first, &cursorPos.second );
	return cursorPos;
}

core::IntPair core::Window::getPos() const
{
	core::IntPair windowPos;
	glfwGetWindowPos( m_window, &windowPos.first, &windowPos.second );
	return windowPos;
}

core::IntPair core::Window::getSize() const
{
	core::IntPair windowSize;
	glfwGetWindowSize( m_window, &windowSize.first, &windowSize.second );
	return windowSize;
}

void core::Window::setPos( int posX, int posY )
{
	glfwSetWindowPos( m_window, posX, posY );
}

core::Window::FrameSize core::Window::getFrameSize() const
{
	FrameSize size {};
	glfwGetWindowFrameSize( m_window, &size.left, &size.top, &size.right, &size.bottom );
	return size;
}

GLFWwindow* core::Window::getGlfwHandle() const
{
	return m_window;
};

void core::Window::centerOnScreen()
{
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode( primary );

	const auto [ windowWidth, windowHeight ] = getSize();
	const int posX = ( mode->width - windowWidth ) / 2;
	const int posY = ( mode->height - windowHeight ) / 2;
	setPos( posX, posY );
}
