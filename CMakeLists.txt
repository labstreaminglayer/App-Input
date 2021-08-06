cmake_minimum_required(VERSION 3.5)
project(Input
	LANGUAGES CXX C
	VERSION 1.14.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LSL REQUIRED
    HINTS ${LSL_INSTALL_ROOT}
    
    "${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/build/install"
    "${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/out/install/x64-Release"
    "${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/out/install/x64-Debug"
    "${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/out/build/x64-Release"
    "${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/out/build/x64-Debug"
    "${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/build/"
    PATH_SUFFIXES share/LSL)
get_filename_component(LSL_PATH ${LSL_CONFIG} DIRECTORY)

if(WIN32)
	add_executable(keyboard win_keyboard.cpp)
	target_link_libraries(keyboard PRIVATE LSL::lsl)
	add_executable(mouse win_mouse.cpp)
	target_link_libraries(mouse PRIVATE LSL::lsl)
    
    add_custom_command(TARGET keyboard POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  
            $<TARGET_FILE:LSL::lsl>
            $<TARGET_FILE_DIR:keyboard>)
            
    add_custom_command(TARGET mouse POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  
            $<TARGET_FILE:LSL::lsl>
            $<TARGET_FILE_DIR:mouse>)
            
	installLSLApp(keyboard)
	installLSLApp(mouse)
endif()
LSLGenerateCPackConfig()
