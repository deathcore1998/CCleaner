#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdio.h>

#include <GLFW/glfw3.h>

#include "app/app.hpp"

int main()
{
	App app;
	app.run();

	return 0;
}
