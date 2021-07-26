#pragma once

#include <string>
#include <vector>
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
void unlockDisk_cb  ( Fl_Widget* pWidget, void* _gui );

// Disk menu callbacks
void formatDisk_cb        ( Fl_Widget* pWidget, void* _gui );
void nameDisk_cb          ( Fl_Widget* pWidget, void* _gui );
void insertFile_cb        ( Fl_Widget* pWidget, void* _gui );
void extractFile_cb       ( Fl_Widget* pWidget, void* _gui );
void removeFile_cb        ( Fl_Widget* pWidget, void* _gui );
void lockFile_cb          ( Fl_Widget* pWidget, void* _gui );
void unlockFile_cb        ( Fl_Widget* pWidget, void* _gui );
void copyFilesCRC_cb      ( Fl_Widget* pWidget, void* _gui );
void setBootOption0_cb    ( Fl_Widget* pWidget, void* _gui );
void setBootOption1_cb    ( Fl_Widget* pWidget, void* _gui );
void setBootOption2_cb    ( Fl_Widget* pWidget, void* _gui );
void setBootOption3_cb    ( Fl_Widget* pWidget, void* _gui );
void exportDirectoryCSV_cb( Fl_Widget* pWidget, void* _gui );

// Auxiliary functions
bool   ChooseFilename( std::string& fileName, const std::string& filter, const std::string& preset, bool bSaveAsDialog, bool bDirectory = false );
bool   ChooseFilename( std::vector<std::string>& fileNames, const std::string& filter, const std::string& preset, bool bSaveAsDialog, bool bDirectory = false );
size_t ChooseSlot    ();
size_t GetFileSize   ( const std::string& _fileName );