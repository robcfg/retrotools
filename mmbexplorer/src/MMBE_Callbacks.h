#pragma once

#include <string>
#include <FL/Fl.H>

// File menu callbacks
void menuQuit_cb  ( Fl_Widget* pWidget, void* );
void menuAbout_cb ( Fl_Widget* pWidget, void* _gui );
void openFile_cb  ( Fl_Widget* pWidget, void* _gui );
void closeFile_cb ( Fl_Widget* pWidget, void* _gui );
void createFile_cb( Fl_Widget* pWidget, void* _gui );

// Slot menu callbacks
void insertDisk_cb  ( Fl_Widget* pWidget, void* _gui );
void extractDisk_cb ( Fl_Widget* pWidget, void* _gui );
void removeDisk_cb  ( Fl_Widget* pWidget, void* _gui );
void lockDisk_cb    ( Fl_Widget* pWidget, void* _gui );
void unlock_cb      ( Fl_Widget* pWidget, void* _gui );

// Auxiliary functions
bool   ChooseFilename( std::string& fileName, const std::string& filter, const std::string& preset, bool bSaveAsDialog, bool bDirectory = false );
size_t ChooseSlot    ();
