cmake_minimum_required(VERSION 3.16)
project(CCleaner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG v1.92.2b-docking
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

add_executable(CCleaner
	source/main.cpp
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp

	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

set(GUI_DIR ${CMAKE_SOURCE_DIR}/source/gui)
set(APP_DIR ${CMAKE_SOURCE_DIR}/source/app)
set(WIDGETS_DIR ${GUI_DIR}/widgets)

target_include_directories(CCleaner PRIVATE
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends

	${CMAKE_SOURCE_DIR}/source
)

target_sources(CCleaner PRIVATE
	${GUI_DIR}/gui.cpp
	${GUI_DIR}/gui.hpp

	${APP_DIR}/app.cpp
	${APP_DIR}/app.hpp

	${WIDGETS_DIR}/widget.cpp
	${WIDGETS_DIR}/widget.hpp
	${WIDGETS_DIR}/title_bar.cpp
	${WIDGETS_DIR}/title_bar.hpp
)

target_link_libraries(CCleaner PRIVATE
	glfw
	opengl32
)
