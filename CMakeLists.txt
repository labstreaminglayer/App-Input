cmake_minimum_required(VERSION 3.5)
project(Keyboard
	LANGUAGES CXX
	VERSION 1.13.0)
include(Findliblsl.cmake)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE
	keyboard_win.cpp
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE
	LSL::lsl
#	vendorsdk
)

installLSLApp(${PROJECT_NAME})
LSLGenerateCPackConfig()
