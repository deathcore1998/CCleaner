#pragma once

#include <utility>

#include <GLFW/glfw3.h>

class App;

namespace core
{
	using DoublePair = std::pair< double, double >;
	using IntPair = std::pair< int, int >;

	class Window
	{
		friend class App;

	public:

		Window( int width, int height, const char* title = "" );
		~Window();

		Window( Window& other ) = delete;
		Window& operator=( const Window& other ) = delete;

		bool shouldClose();
		bool getWindowAttrib();
		void swapBuffers();

		void close();
		void rollUp();

		DoublePair getCursorPos() const;
		IntPair getPos() const;
		IntPair getSize() const;
		void setPos( int posX, int posY );

		struct FrameSize
		{
			int left = 0;
			int	top = 0;
			int	right = 0;
			int bottom = 0;
		};
		FrameSize getFrameSize() const;


	private:
		GLFWwindow* getGlfwHandle() const;
		void centerOnScreen();

		GLFWwindow* m_window = nullptr;
	};
}