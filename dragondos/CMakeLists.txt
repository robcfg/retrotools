cmake_minimum_required(VERSION 3.10)

# Set project name
project(DRAGONDOS)

# Find required packages
if( WIN32 )
	# Set here the path to the FLTK folder
	set(FLTK_INCLUDE_DIR "C:\\Users\\example\\fltk-1.3.8\\")
endif()
set(FLTK_SKIP_FLUID True)
FIND_PACKAGE(FLTK QUIET REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable(s)
add_executable(
	dragondos ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_Main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_Commands.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_FS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_BASIC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawDiskImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VDKDiskImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FS_Utils.cpp
	)

add_executable(
    dragondosui ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_UI_Main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_UI_Callbacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_FS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_BASIC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_ViewFileWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawDiskImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VDKDiskImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FS_Utils.cpp
    )
    
# Set debug postfix
set(CMAKE_DEBUG_POSTFIX _d)
set_target_properties(dragondos PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(dragondosui PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Include directories
include_directories(dragondosui SYSTEM PUBLIC ${FLTK_INCLUDE_DIR})

# Link libraries
TARGET_LINK_LIBRARIES(dragondosui ${FLTK_LIBRARIES})
TARGET_LINK_LIBRARIES(dragondosui ${OPENGL_LIBRARIES})

# Cheat sheet
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# cmake -DCMAKE_BUILD_TYPE=Release ..