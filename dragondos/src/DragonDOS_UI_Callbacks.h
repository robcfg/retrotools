#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Multi_Browser.H>
#include <string>

#include "DiskImageInterface.h"
#include "FileSystemInterface.h"

#include "DragonDOS_ViewFileWindow.h"

struct SDRAGONDOS_Context
{
    std::string                 diskFilename;
    IDiskImageInterface*        disk;
    IFilesystemInterface*       fs;

    Fl_Box*                     fileLabel       = nullptr;
    Fl_Box*                     diskInfoLabel   = nullptr;
    Fl_Multi_Browser*           browser         = nullptr;

    Fl_Window*                  mainWindow      = nullptr;
    Fl_Window*                  aboutWindow     = nullptr;
    CDragonDOSViewFileWindow*   viewFileWindow  = nullptr;

    Fl_Check_Button*            extractBinaryHeadersButton = nullptr;

    unsigned short int          loadAddress     = 0;
    unsigned short int          execAddress     = 0;
};

void UpdateUI                   ( const SDRAGONDOS_Context* _context );
bool ChooseFilename             ( std::string& fileName, bool bSaveAsDialog, bool bDirectory );

#ifndef __APPLE__
void menuQuit_cb                (Fl_Widget* pWidget,void* _context);
void menuAbout_cb               (Fl_Widget* pWidget,void* _context);
#endif
void newDisk_cb                 (Fl_Widget* pWidget,void* _context);
void openDisk_cb                (Fl_Widget* pWidget,void* _context);
void saveDisk_cb                (Fl_Widget* pWidget,void* _context);
void insertBasic_cb             (Fl_Widget* pWidget,void* _context);
void insertBinary_cb            (Fl_Widget* pWidget,void* _context);
void insertData_cb              (Fl_Widget* pWidget,void* _context);
void extractFiles_cb            (Fl_Widget* pWidget,void* _context);
void removeFiles_cb             (Fl_Widget* pWidget,void* _context);
void viewFiles_cb               (Fl_Widget* pWidget,void* _context);

// View file window callbacks
void viewFileAsHex_cb           ( Fl_Widget* pWidget, void* _viewFileWindow );
void viewFileAsText_cb          ( Fl_Widget* pWidget, void* _viewFileWindow );
void viewFileAsBasic_cb         ( Fl_Widget* pWidget, void* _viewFileWindow );
void viewFileAsImage_cb         ( Fl_Widget* pWidget, void* _viewFileWindow );
void viewFileAsDisassembly_cb   ( Fl_Widget* pWidget, void* _viewFileWindow );
void viewFileModeChanged_cb     ( Fl_Widget* pWidget, void* _viewFileWindow );
void viewFilePaletteChanged_cb  ( Fl_Widget* pWidget, void* _viewFileWindow );
void viewFileExportPNG_cb       ( Fl_Widget* pWidget, void* _viewFileWindow );
