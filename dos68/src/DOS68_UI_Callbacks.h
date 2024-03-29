#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Multi_Browser.H>
#include <string>
#include "DiskImageInterface.h"
#include "FileSystemInterface.h"

using namespace std;

struct SDOS68UI_Context
{
    string                diskFilename;
    IDiskImageInterface*  disk;
    IFilesystemInterface* fs;

    Fl_Box*            fileLabel     = nullptr;
    Fl_Box*            diskInfoLabel =  nullptr;
    Fl_Multi_Browser* browser       = nullptr;
};

void UpdateUI( const SDOS68UI_Context* _context );
void newDisk_cb(Fl_Widget* pWidget,void* _context);
void openDisk_cb(Fl_Widget* pWidget,void* _context);
void saveDisk_cb(Fl_Widget* pWidget,void* _context);
void insertAscii_cb(Fl_Widget* pWidget,void* _context);
void insertBinary_cb(Fl_Widget* pWidget,void* _context);
void extractFiles_cb(Fl_Widget* pWidget,void* _context);
void removeFiles_cb(Fl_Widget* pWidget,void* _context);
