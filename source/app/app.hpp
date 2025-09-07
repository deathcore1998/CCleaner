#pragma once

#include <string>

#include <GLFW/glfw3.h>

class App
{
public:
	App();

	void run();

	~App();
private:
	void initWindow();
	void initGui();
	void cleanUp();

	GLFWwindow* m_window;

	std::string m_title;
};