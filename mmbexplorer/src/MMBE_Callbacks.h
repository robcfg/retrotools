#pragma once

#include <string>
#include <FL/Fl.H>

// Callbacks
void menuQuit_cb  ( Fl_Widget* pWidget, void* );
void openFile_cb  ( Fl_Widget* pWidget, void* _gui );
void closeFile_cb ( Fl_Widget* pWidget, void* _gui );
void createFile_cb( Fl_Widget* pWidget, void* _gui );

// Auxiliary functions
bool ChooseFilename( std::string& fileName, const std::string& filter, const std::string& preset, bool bSaveAsDialog );

