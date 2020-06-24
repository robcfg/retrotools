cmake_minimum_required(VERSION 3.10)

# Set project name
project(MMBExplorer)

# Find required packages
if( WIN32 )
	# Set here the path to the FLTK folder
	set(FLTK_INCLUDE_DIR "C:\\Users\\example\\fltk-1.3.5\\")
endif()
set(FLTK_SKIP_FLUID True)
FIND_PACKAGE(FLTK QUIET REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
add_executable(
	MMBExplorer ${CMAKE_CURRENT_SOURCE_DIR}/src/MMBExplorer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MMBFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MMBE_Commands.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MMBE_Gui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MMBE_Callbacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AcornDFS.cpp
	)

# Generate source control version file
add_custom_target(
	getscmversion 
	ALL
	COMMAND rm -f ${CMAKE_CURRENT_SOURCE_DIR}/src/scmversion.h
	COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/src/getscmversion.sh
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/src/getscmversion.sh
	VERBATIM
	BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/src/scmversion.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
	)
add_dependencies(MMBExplorer getscmversion)

# Set debug postfix
set(CMAKE_DEBUG_POSTFIX _d)
set_target_properties(MMBExplorer PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Include directories
include_directories(MMBExplorer SYSTEM PUBLIC ${FLTK_INCLUDE_DIR})

# Link libraries
TARGET_LINK_LIBRARIES(MMBExplorer ${FLTK_LIBRARIES})
TARGET_LINK_LIBRARIES(MMBExplorer ${OPENGL_LIBRARIES})

# Cheat sheet
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# cmake -DCMAKE_BUILD_TYPE=Release ..