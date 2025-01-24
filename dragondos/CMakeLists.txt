cmake_minimum_required(VERSION 3.11)

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

if( WIN32 )
    add_executable( dragondosui WIN32 )
    target_sources( dragondosui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc)
elseif( APPLE )
    add_executable( dragondosui )
    target_sources( dragondosui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/logo/DragonDOS_Logo.icns)
else()
    add_executable( dragondosui )
endif()

target_sources( dragondosui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_UI_Main.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_UI_Callbacks.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_FS.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_BASIC.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/DragonDOS_ViewFileWindow.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/RawDiskImage.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/VDKDiskImage.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/FS_Utils.cpp
    )

if( APPLE )
set_target_properties( dragondosui
    PROPERTIES MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/MacOSXBundleInfo.plist.in 
    MACOSX_BUNDLE_BUNDLE_NAME "DragonDOS"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0.2"
    MACOSX_BUNDLE_COPYRIGHT "2025 by Roberto Carlos Fernández Gerhardt"
    MACOSX_BUNDLE_GUI_IDENTIFIER "org.rcfg.ddos"
    MACOSX_BUNDLE_ICON_FILE "DragonDOS_Logo.icns"
    MACOSX_BUNDLE_INFO_STRING "DragonDOS by Robcfg"
    MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.2, 2025 by Roberto Carlos Fernández Gerhardt"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.2"
    )
set_source_files_properties( ./logo/DragonDOS_Logo.icns
    PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()

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