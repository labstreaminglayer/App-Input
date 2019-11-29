cmake_minimum_required(VERSION 3.5)
project(Input
	LANGUAGES CXX
	VERSION 1.13.0)
include(Findliblsl.cmake)

if(WIN32)
	add_executable(keyboard win_keyboard.cpp)
	target_link_libraries(keyboard PRIVATE LSL::lsl)
	add_executable(mouse win_mouse.cpp)
	target_link_libraries(mouse PRIVATE LSL::lsl)
	installLSLApp(keyboard)
	installLSLApp(mouse)
endif()
LSLGenerateCPackConfig()
