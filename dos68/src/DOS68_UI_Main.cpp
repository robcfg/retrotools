#include <cstdio>
#include <FL/Fl.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/x.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

#include "DOS68_FS.h"
#include "DOS68.xpm"
#include "DOS68_UI_Callbacks.h"
#include "RawDiskImage.h"

#ifndef __APPLE__
#define DOS68UI_MENUBARHEIGHT 30
#else
#define DOS68UI_MENUBARHEIGHT 0
#endif

void CreateMenuBar( int _menuBarWidth, int _menuBarHeight, int _modifierKey, SDOS68UI_Context* _context )
{
	// Menu bar
	#ifdef __APPLE__
	Fl_Sys_Menu_Bar* menuBar = new Fl_Sys_Menu_Bar( 0, 0, _menuBarWidth, _menuBarHeight );
	#else
	Fl_Menu_Bar* menuBar = new Fl_Menu_Bar( 0, 0, _menuBarWidth, _menuBarHeight );
	#endif

	// File menu
	menuBar->add( "&File/&New disk"              , _modifierKey+'n', newDisk_cb      , (void*)_context, 0 );
	menuBar->add( "&File/_&Open disk"            , _modifierKey+'o', openDisk_cb     , (void*)_context, 0 );
	menuBar->add( "&File/_&Save disk changes"    , _modifierKey+'s', saveDisk_cb     , (void*)_context, 0 );
	menuBar->add( "&File/Insert &Ascii file(s)"  , _modifierKey+'a', insertAscii_cb  , (void*)_context, 0 );
	menuBar->add( "&File/Insert &Binary file(s)" , _modifierKey+'b', insertBinary_cb , (void*)_context, 0 );
	menuBar->add( "&File/&Extract file(s)"       , _modifierKey+'e', extractFiles_cb , (void*)_context, 0 );
	#ifndef __APPLE__
    mMenuBar->add( "&File/&Quit"          , _modifierKey+'q', menuQuit_cb  , (void*)mMainWindow, 0 );
    #endif

    // Help menu
	#ifndef __APPLE__
    mMenuBar->add( "&Help/&About"         , _modifierKey+'h', menuAbout_cb  , (void*)mMainWindow, 0 );
    #endif
}

void CreateControls( int _width, int _menuBarHeight, SDOS68UI_Context* _context )
{
    int x = 10;
    int y = _menuBarHeight;

    // File Label
    _context->fileLabel = new Fl_Box( FL_NO_BOX, x, y, _width - 20, 30, "(No disk)" );
    _context->fileLabel->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
    y += 40;

	_context->browser = new Fl_Multi_Browser( x, y, 360, 540 );
    x += 370;

    Fl_Button* newDiskButton = new Fl_Button( x, y, 170, 30, "New disk");
    newDiskButton->callback( newDisk_cb, (void*)_context );
    y += 35;

    Fl_Button* openDiskButton = new Fl_Button( x, y, 170, 30, "Open disk");
    openDiskButton->callback( openDisk_cb, (void*)_context );
    y += 35;

    Fl_Button* saveDiskButton = new Fl_Button( x, y, 170, 30, "Save disk changes");
    saveDiskButton->callback( saveDisk_cb, (void*)_context );
    y += 35;

    Fl_Button* insertAsciiFilesButton = new Fl_Button( x, y, 170, 30, "Insert ASCII file(s)");
    insertAsciiFilesButton->callback( insertAscii_cb, (void*)_context );
    y += 35;

    Fl_Button* insertBinaryFilesButton = new Fl_Button( x, y, 170, 30, "Insert binary file(s)");
    insertBinaryFilesButton->callback( insertBinary_cb, (void*)_context );
    y += 35;

    Fl_Button* extractFilesButton = new Fl_Button( x, y, 170, 30, "Extract selected file(s)");
    extractFilesButton->callback( extractFiles_cb, (void*)_context );
    y += 35;

    Fl_Button* removeFilesButton = new Fl_Button( x, y, 170, 30, "Remove selected file(s)");
    removeFilesButton->callback( removeFiles_cb, (void*)_context );
    y += 40;

    _context->diskInfoLabel = new Fl_Box( FL_NO_BOX, x, y, 170, 140, "Disk info:\n- side(s)\n- density\n-\"\n- TPI\n-\n- free sectors\n- free bytes" );
    _context->diskInfoLabel->align( FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE );
    _context->diskInfoLabel->color2( FL_YELLOW );
    y += 150;

    // Decoration 160x140. Edit XPM file to set the data to be static const char[] and change the color names
    // like 'white' and 'black' for their hex values (#000000 and #FFFFFF).
    Fl_Box* logo = new Fl_Box( FL_NO_BOX, x + 5, y, 160, 140, 0 );
    logo->align( FL_ALIGN_INSIDE );
    logo->image( new Fl_Pixmap( DOS68_Logo ) );
}

int main( int argc, char** argv )
{
    Fl::visual(FL_RGB);

#ifdef __APPLE__
    int menuBarOffset = 0;
    int modifierKey = FL_COMMAND;
#else
    int menuBarOffset = 30;
    int modifierKey = FL_CTRL;
#endif 

    Fl_Double_Window* mainWindow = new Fl_Double_Window( 560, 600 + menuBarOffset, "DOS68 by Robcfg" );

    mainWindow->begin();

    CRAWDiskImage img;
    CDOS68_FS fs;
    
    img.SetSectorSize( DOS68_SECTOR_SIZE        );
    img.SetSectorsNum( DOS68_SECTORS_PER_TRACK  );
    img.SetSidesNum  ( 1   );
    img.SetTracksNum ( 35  );

    SDOS68UI_Context context;
    context.disk = &img;
    context.fs = &fs;

    CreateMenuBar( mainWindow->w(), DOS68UI_MENUBARHEIGHT, modifierKey, &context );

    CreateControls( mainWindow->w(), DOS68UI_MENUBARHEIGHT, &context );

    mainWindow->end();
/*
#ifndef __APPLE__
#ifdef WIN32
	mMainWindow->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#else
    Fl_Pixmap xpmIcon( mmbexplorer_64x64 );
    Fl_RGB_Image appIcon( &xpmIcon, Fl_Color(0) );
    mMainWindow->icon( &appIcon );
#endif
#endif

    mainWindow->resizable( mTable );*/
    mainWindow->show(0, nullptr);
    mainWindow->redraw();
    
    return Fl::run();
}