#pragma once

#include <string>

#include "core/window.hpp"

class App
{
public:
	App();
	~App();

	void run();

private:
	void initGui();
	void finiGui();

	core::Window m_window;
};