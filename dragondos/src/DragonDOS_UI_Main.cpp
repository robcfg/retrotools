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
#include <FL/Fl_Check_Button.H>

#include "../logo/DragonDOS_Logo.xpm"
#include "DragonDOS_UI_Callbacks.h"

// Disk image formats
#include "../../common/DiskImages/DiskImageFactory.h"
#include "../../common/DiskImages/VDKDiskImage.h"
#include "../../common/DiskImages/JVCDiskImage.h"
#include "../../common/DiskImages/IMDDiskImage.h"
#include "../../common/DiskImages/RawDiskImage.h"

// File systems
#include "../../common/FileSystems/DragonDOS_FS.h"

#ifndef __APPLE__
#define DRAGONDOSUI_MENUBARHEIGHT 30
#else
#define DRAGONDOSUI_MENUBARHEIGHT 0
#endif

const int DRAGONDOS_UI_VIEWFILE_WIDTH     = 620;  // Width of the View File window
const int DRAGONDOS_UI_VIEWFILE_HEIGHT    = 575;  // Height of the View File window

class CAppWindow : public Fl_Double_Window
{
public:
	CAppWindow( SDRAGONDOS_Context* _context, int _w, int _h, const char* _label = nullptr );
	CAppWindow( SDRAGONDOS_Context* _context, int _x, int _y, int _w, int _h, const char* _label = nullptr );
	virtual ~CAppWindow();

	virtual int handle(int _event);

private:
    SDRAGONDOS_Context* mContext = nullptr;
};

CAppWindow::CAppWindow( SDRAGONDOS_Context* _context, int _w, int _h, const char* _label ) : 
    Fl_Double_Window( _w, _h, _label ),
    mContext(_context)
{
    if( nullptr != mContext )
    {
        mContext->mainWindow = this;
    }
}

CAppWindow::CAppWindow( SDRAGONDOS_Context* _context, int _x, int _y, int _w, int _h, const char* _label ) : 
    Fl_Double_Window( _x, _y, _w, _h, _label ),
    mContext(_context)
{
    if( nullptr != mContext )
    {
        mContext->mainWindow = this;
    }
}

CAppWindow::~CAppWindow()
{

}

int CAppWindow::handle( int _event )
{
    int ret = Fl_Window::handle( _event );
    switch ( _event ) 
    {
        case FL_HIDE:
        {
            if( mContext->viewFileWindow )
            {
                mContext->viewFileWindow->hide();
            }
        }
    }
    return(ret);
}

void CreateMenuBar( int _menuBarWidth, int _menuBarHeight, int _modifierKey, SDRAGONDOS_Context* _context )
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
	menuBar->add( "&File/Insert &BASIC file(s)"  , _modifierKey+'a', insertBasic_cb  , (void*)_context, 0 );
	menuBar->add( "&File/Insert &Binary file(s)" , _modifierKey+'b', insertBinary_cb , (void*)_context, 0 );
	menuBar->add( "&File/Insert &Data file(s)"   , _modifierKey+'d', insertBinary_cb , (void*)_context, 0 );
	menuBar->add( "&File/&Extract file(s)"       , _modifierKey+'e', extractFiles_cb , (void*)_context, 0 );
	menuBar->add( "&File/&Remove file(s)"        , _modifierKey+'r', removeFiles_cb  , (void*)_context, 0 );
	menuBar->add( "&File/&View file(s)"          , _modifierKey+'v', viewFiles_cb    , (void*)_context, 0 );
	#ifndef __APPLE__
    menuBar->add( "&File/&Quit"          , _modifierKey+'q', menuQuit_cb  , (void*)_context, 0 );
    #endif

    // Help menu
	#ifndef __APPLE__
    menuBar->add( "&Help/&About"         , _modifierKey+'h', menuAbout_cb  , (void*)_context, 0 );
    #endif
}

void CreateControls( int _width, int _menuBarHeight, SDRAGONDOS_Context* _context )
{
    int x = 10;
    int y = _menuBarHeight;

    // File Label
    _context->fileLabel = new Fl_Box( FL_NO_BOX, x, y, _width - 20, 30, "(No disk)" );
    _context->fileLabel->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
    y += 40;

	_context->browser = new Fl_Multi_Browser( x, y, 380, 565 );
    x += 390;

    Fl_Button* newDiskButton = new Fl_Button( x, y, 170, 30, "New disk");
    newDiskButton->callback( newDisk_cb, (void*)_context );
    y += 35;

    Fl_Button* openDiskButton = new Fl_Button( x, y, 170, 30, "Open disk");
    openDiskButton->callback( openDisk_cb, (void*)_context );
    y += 35;

    Fl_Button* saveDiskButton = new Fl_Button( x, y, 170, 30, "Save disk changes");
    saveDiskButton->callback( saveDisk_cb, (void*)_context );
    y += 35;

    Fl_Button* insertBasicFilesButton = new Fl_Button( x, y, 170, 30, "Insert BASIC file(s)");
    insertBasicFilesButton->callback( insertBasic_cb, (void*)_context );
    y += 35;

    Fl_Button* insertBinaryFilesButton = new Fl_Button( x, y, 170, 30, "Insert binary file(s)");
    insertBinaryFilesButton->callback( insertBinary_cb, (void*)_context );
    y += 35;

    Fl_Button* insertDataFilesButton = new Fl_Button( x, y, 170, 30, "Insert data file(s)");
    insertDataFilesButton->callback( insertData_cb, (void*)_context );
    y += 35;

    Fl_Button* extractFilesButton = new Fl_Button( x, y, 170, 30, "Extract selected file(s)");
    extractFilesButton->callback( extractFiles_cb, (void*)_context );
    y += 35;

    Fl_Button* removeFilesButton = new Fl_Button( x, y, 170, 30, "Remove selected file(s)");
    removeFilesButton->callback( removeFiles_cb, (void*)_context );
    y += 35;
    
    Fl_Button* viewFilesButton = new Fl_Button( x, y, 170, 30, "View selected file(s)");
    viewFilesButton->callback( viewFiles_cb, (void*)_context );
    y += 35;
    
    _context->extractBinaryHeadersButton = new Fl_Check_Button( x, y, 170, 30, "Extract binary headers");
    _context->extractBinaryHeadersButton->value(1);
    y += 35;

    _context->diskInfoLabel = new Fl_Box( FL_NO_BOX, x, y, 170, 140, "Disk info:\n- side(s)\n- tracks\n- total bytes\n- free bytes\n- free sectors" );
    _context->diskInfoLabel->align( FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE );
    _context->diskInfoLabel->color2( FL_YELLOW );
    y += 90;

    // Decoration 160x140. Edit XPM file to set the data to be static const char[] and change the color names
    // like 'white' and 'black' for their hex values (#000000 and #FFFFFF).
    Fl_Box* logo = new Fl_Box( FL_NO_BOX, x + 5, y, 160, 140, 0 );
    logo->align( FL_ALIGN_INSIDE );
    logo->image( new Fl_Pixmap( DragonDOS_Logo ) );

    // View file window
    _context->viewFileWindow = new CDragonDOSViewFileWindow( DRAGONDOS_UI_VIEWFILE_WIDTH, DRAGONDOS_UI_VIEWFILE_HEIGHT, "View file(s)" );

	// New disk window
	_context->newDiskWindow = new CDragonDOSNewDiskWindow( 310, 125, "Create new disk" );
	_context->newDiskWindow->set_modal();
	_context->newDiskWindow->SetData( _context );
}

#ifndef __APPLE__
void CreateAboutDialog( SDRAGONDOS_Context* _context )
{
    int aboutDialogWidth  = 284;
    int aboutDialogHeight = 235;

    _context->aboutWindow = new Fl_Window( aboutDialogWidth, aboutDialogHeight, nullptr );
    if( nullptr == _context->aboutWindow )
    {
        return;
    }
#ifdef WIN32
    _context->aboutWindow->resizable( _context->aboutWindow );
#endif
    _context->aboutWindow->box(FL_FLAT_BOX);
    _context->aboutWindow->color( FL_DARK2 );
    _context->aboutWindow->set_modal();
    _context->aboutWindow->begin();

    int ySeparation = 8;
    int y = ySeparation;

    Fl_Pixmap* aboutIcon = new Fl_Pixmap( DragonDOS_Logo );
    Fl_Box* aboutIconBox = new Fl_Box( (aboutDialogWidth - aboutIcon->w()) / 2, y, aboutIcon->w(), aboutIcon->h() );
    aboutIconBox->image(aboutIcon);
    y += aboutIcon->h() + ySeparation;

    int textWidth  = 104;
    int textHeight = 15;

    Fl_Box* aboutText1 = new Fl_Box( (aboutDialogWidth - textWidth)/2, y, textWidth, textHeight, "DragonDOS" );
    aboutText1->align( FL_ALIGN_CENTER );
    aboutText1->labelcolor( FL_BLACK );
    aboutText1->labelsize( 14 );
    aboutText1->labelfont( FL_HELVETICA_BOLD );
    y += textHeight + ySeparation;

    std::string versionStr = "Version 1.0.4";
    Fl_Box* aboutText2 = new Fl_Box( (aboutDialogWidth - textWidth)/2, y, textWidth, textHeight, nullptr );
    aboutText2->align( FL_ALIGN_CENTER );
    aboutText2->labelcolor( FL_BLACK );
    aboutText2->labelsize( 12 );
    aboutText2->labelfont( FL_HELVETICA );
    aboutText2->copy_label( versionStr.c_str() );
    y += textHeight + ySeparation;

    std::string fltkVersionStr = "GUI with FLTK ";
    fltkVersionStr += std::to_string( FL_MAJOR_VERSION );
    fltkVersionStr += ".";
    fltkVersionStr += std::to_string( FL_MINOR_VERSION );
    Fl_Box* aboutText3 = new Fl_Box( (aboutDialogWidth - textWidth)/2, y, textWidth, textHeight, "GUI with FLTK 1.3" );
    aboutText3->align( FL_ALIGN_CENTER );
    aboutText3->labelcolor( FL_BLACK );
    aboutText3->labelsize( 12 );
    aboutText3->labelfont( FL_HELVETICA_BOLD );
    y += textHeight + ySeparation;

    Fl_Box* aboutText4 = new Fl_Box( (aboutDialogWidth - textWidth)/2, y, textWidth, textHeight * 2, "Copyright 2025 by Roberto Carlos\nFernández Gerhardt" );
    aboutText4->align( FL_ALIGN_CENTER );
    aboutText4->labelcolor( FL_BLACK );
    aboutText4->labelsize( 12 );
    aboutText4->labelfont( FL_HELVETICA );

    _context->aboutWindow->end();
}
#endif

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

    SDRAGONDOS_Context context;

    CAppWindow* mainWindow = new CAppWindow( &context, 585, 615 + menuBarOffset, "DragonDOS 1.0.4 by Robcfg" );

    mainWindow->begin();

	DiskImageFactory diskFactory;
	diskFactory.RegisterDiskImageFormat( new CVDKDiskImage );
	diskFactory.RegisterDiskImageFormat( new CJVCDiskImage );
	diskFactory.RegisterDiskImageFormat( new CIMDDiskImage );
	diskFactory.RegisterDiskImageFormat( new CRAWDiskImage );
	context.diskImageFactory = &diskFactory;

    CDragonDOS_FS fs;
    context.fs = &fs;

    CreateMenuBar( mainWindow->w(), DRAGONDOSUI_MENUBARHEIGHT, modifierKey, &context );

    CreateControls( mainWindow->w(), DRAGONDOSUI_MENUBARHEIGHT, &context );

#ifndef __APPLE__
    CreateAboutDialog( &context );
#endif

    mainWindow->end();

#ifndef __APPLE__
#ifdef WIN32
	mainWindow->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#else
    Fl_Pixmap xpmIcon( DragonDOS_Logo );
    Fl_RGB_Image appIcon( &xpmIcon, Fl_Color(0) );
    mainWindow->icon( &appIcon );
#endif
#endif

    UpdateUI( (const SDRAGONDOS_Context*)&context );
    context.fileLabel->copy_label("(No disk)");
    mainWindow->show(argc, argv);
    mainWindow->redraw();

	int retVal = Fl::run();

	// Cleanup
	if( context.disk != nullptr )
	{
		delete context.disk;
	}

    return retVal;
}