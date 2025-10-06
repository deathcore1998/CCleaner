#pragma once

#include <string>

#include <GLFW/glfw3.h>

class App
{
public:
	App();
	~App();

	void run();

private:
	void initWindow();
	void initGui();
	void cleanUp();

	void centerWindow();

	// FIXME need create a wrapper
	GLFWwindow* m_window;
};