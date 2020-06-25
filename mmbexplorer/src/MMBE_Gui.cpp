#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <FL/Fl.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include "MMBE_Gui.h"
#include "MMBE_Callbacks.h"
#include "AcornDFS.h"

// Icons
#include "../icons/empty.xpm"
#include "../icons/locked.xpm"
#include "../icons/unlocked.xpm"

using namespace std;

CAppWindow::CAppWindow( int _w, int _h, const char* _label ) : 
        Fl_Window( _w, _h, _label ) 
{
}

CAppWindow::CAppWindow( int _x, int _y, int _w, int _h, const char* _label ) : 
    Fl_Window( _x, _y, _w, _h, _label )
{
}

CAppWindow::~CAppWindow()
{

}

int CAppWindow::handle( int _event )
{
    int ret = Fl_Window::handle( _event );
    /*switch ( _event ) 
    {
        case FL_PUSH:               // do 'copy/dnd' when someone clicks on box
            Fl::copy(blabla.c_str(),(int)blabla.length(),0);//Fl::clipboard_image);
            Fl::dnd();
            ret = 1;
            break;
    }*/
    return(ret);
}

void CAppWindow::SetDiscContentWidget( Fl_Select_Browser* _diskContent )
{
    mDiskContent = _diskContent;
}

void CAppWindow::RefreshDiscContent( unsigned char* _data, size_t _dataSize )
{
    if( nullptr == mDiskContent )
    {
        return;
    }

    mDiskContent->clear();    

    if( nullptr != _data )
    {
        stringstream strStream;
        DFSDisk disc;

        DFSRead( _data, _dataSize, disc );

        string discNameStr = "@fDisc name: " + disc.name;
        mDiskContent->add( discNameStr.c_str() );
        string bootOptionStr = "@fBoot option: ";
        bootOptionStr += BootOptionToString( disc.bootOption );
        mDiskContent->add( bootOptionStr.c_str() );
        mDiskContent->add( "@f-+----------+------+-----+-----");
        mDiskContent->add( "@fD|File name |Size  |Load |Exec ");
        mDiskContent->add( "@f-+----------+------+-----+-----");
        
        size_t sectorsUsed = 2; // Filesystem sectors

        for( auto dfsFile : disc.files )
        {
            string fileStr = "@f";
            fileStr += dfsFile.directory;
            fileStr += '.';

            // Pad file name to the maximum of 12 characters
            string paddedName = dfsFile.name;
            if( paddedName.length() < 10 )
            {
                paddedName.insert( paddedName.end(), 10 - paddedName.length(), ' ');
            }

            fileStr += paddedName;

            // Convert and pad size to a maximum of 6 characters
            string paddedSize = to_string( dfsFile.fileSize );
            if( paddedSize.length() < 6 )
            {
                paddedSize.insert( paddedSize.begin(), 6 - paddedSize.length(), ' ' );
            }
            fileStr += paddedSize;
            fileStr += ' ';
            sectorsUsed += dfsFile.fileSize / 256;
            if( 0 != dfsFile.fileSize % 256 )
            {
                ++sectorsUsed;
            }

            // Convert to hex and pad load address to a maximum of 4 characters
            strStream.str("");
            strStream << hex << dfsFile.loadAddress;
            string paddedLoad = strStream.str();
            if( paddedLoad.length() < 5 )
            {
                paddedLoad.insert( paddedLoad.begin(), 5 - paddedLoad.length(), '0' );
            }
            transform( paddedLoad.begin(), paddedLoad.end(), paddedLoad.begin(), ::toupper );
            fileStr += paddedLoad;
            fileStr += ' ';

            // Convert to hex and pad load address to a maximum of 4 characters
            strStream.str("");
            strStream << hex << dfsFile.execAddress;
            std::string paddedExec = strStream.str();
            if( paddedExec.length() < 5 )
            {
                paddedExec.insert( paddedExec.begin(), 5 - paddedExec.length(), '0' );
            }
            transform( paddedExec.begin(), paddedExec.end(), paddedExec.begin(), ::toupper );
            fileStr += paddedExec;
            fileStr += ' ';

            // Add line to widget
            mDiskContent->add( fileStr.c_str() );
        }

        mDiskContent->add( "@f-+----------+------+-----+-----");
        string freeSpaceStr = "@f  Free Space   ";
        string paddedFreeSpace = to_string( (disc.sectorsNum - min(sectorsUsed, (size_t)disc.sectorsNum)) * 256 );
        if( paddedFreeSpace.length() < 6 )
        {
            paddedFreeSpace.insert( paddedFreeSpace.begin(), 6 - paddedFreeSpace.length(), ' ' );
        }
        freeSpaceStr += paddedFreeSpace;
        mDiskContent->add( freeSpaceStr.c_str() );
    }
}

CMMBETable::CMMBETable( CMMBFile* _mmb, int _x, int _y, int _w, int _h, const char* _label ) : Fl_Table( _x, _y, _w, _h, _label )
{
    mIconEmpty    = new Fl_Pixmap( iconEmpty );
    mIconLocked   = new Fl_Pixmap( iconLocked );
    mIconUnlocked = new Fl_Pixmap( iconUnlocked );
    mMMB          = _mmb;
}

CMMBETable::~CMMBETable()
{

}

void CMMBETable::DrawHeader( const char* _string, int _x, int _y, int _w, int _h )
{
    fl_push_clip( _x, _y, _w, _h );
    fl_draw_box( FL_THIN_UP_BOX, _x, _y, _w, _h, row_header_color() );
    fl_color( FL_BLACK );
    fl_draw( _string, _x, _y, _w, _h, FL_ALIGN_CENTER );
    fl_pop_clip();
} 

void CMMBETable::DrawData( const char* _string, Fl_Pixmap* _icon, int _x, int _y, int _w, int _h, bool _selected )
{
    fl_push_clip( _x, _y, _w, _h );

    // Draw cell bg
    fl_color(_selected ? FL_YELLOW : FL_WHITE); 
    fl_rectf( _x, _y, _w, _h );
    
    // Draw cell icon
    if( nullptr != _icon )
    {
        _icon->draw( _x + 1, _y + 1 );
    }

    // Draw cell data
    fl_color(FL_GRAY0); 
    fl_draw( _string, _x+26, _y, _w, _h, FL_ALIGN_LEFT );
    
    // Draw box border
    fl_color( color() );
    fl_rect( _x, _y, _w, _h );
    
    fl_pop_clip();
}

void CMMBETable::DrawUnused( int _x, int _y, int _w, int _h )
{
    fl_color(FL_GRAY); 
    fl_rectf( _x, _y, _w, _h );

    fl_color(FL_GRAY0); 
    fl_rect( _x, _y, _w, _h );
}

int CMMBETable::handle( int _event )
{
    // Compute the targeted slot
    int row = 0;
    int col = 0;
    ResizeFlag rf;
    cursor2rowcol ( row, col, rf );
    size_t slot = (col * rows()) + row;

    switch( _event )
    {
        case FL_RELEASE:
        {
            cout << "x=" << Fl::event_x() << " y=" << Fl::event_y() << " tix=" << tix << " tiy=" << tiy << " tiw=" << tiw << " tih=" << tih << endl ;
            if( Fl::event_x() < tix || Fl::event_x() >= tix + tiw ||
                Fl::event_y() < tiy || Fl::event_y() >= tiy + tih )
            {
                return Fl_Table::handle( _event );
            }

            switch( Fl::event_button() )
            {
                case FL_LEFT_MOUSE:
                    SelectSlot( slot );
                    cout << "FL_RELEASE Left Mouse Button (Table) at " << callback_col() << ", " << callback_row() << endl;
                    break;
                case FL_RIGHT_MOUSE:
                    SelectSlot( slot );
                    cout << "FL_RELEASE Right Mouse Button (Table) at " << callback_col() << ", " << callback_row() << endl;
                    break;
                default:
                    cout << "FL_RELEASE (Table) at " << callback_col() << ", " << callback_row() << endl;
                    break;
            }
            return 1;
        }
        break;
        // Receive a dropped item
        case FL_DND_ENTER:
        case FL_DND_RELEASE:
        case FL_DND_LEAVE:
        case FL_DND_DRAG:
            return 1;

        case FL_PASTE:
        {
            if( nullptr == mMMB || slot >= mMMB->GetNumberOfDisks() )
            {
                return 1;
            }

            std::string errorString;
            if( !mMMB->InsertImageInSlot( Fl::event_text(), slot, errorString ) )
            {
                fl_alert("[ERROR] %s",errorString.c_str());
            }

            SelectSlot( slot );

            return 1;
        }
        break;
    }

    return Fl_Table::handle( _event );
}

void CMMBETable::draw_cell( TableContext context, int _row, int _col, int _x, int _y, int _w, int _h )
{
    string tmp;
    char cellData[32] = {0};
    size_t slot = (_col*32)+_row;
    bool selected = slot == mSelectedSlot;

    switch ( context ) 
    {
    case CONTEXT_STARTPAGE:                   // before page is drawn..
        fl_font(FL_COURIER, 16);              // set the font for our drawing operations
        return; 
    case CONTEXT_COL_HEADER:                  // Draw column headers
        tmp = to_string( _col * 32 );
        DrawHeader( tmp.c_str(), _x, _y, _w, _h );
        return; 
    case CONTEXT_ROW_HEADER:                  // Draw row headers
        tmp = to_string( _row );
        DrawHeader( tmp.c_str(), _x, _y, _w, _h );
        return; 
    case CONTEXT_CELL:                        // Draw data in cells
        if( _row == 31 && _col == 15 )
        {
            DrawUnused( _x, _y, _w, _h );
        }
        else
        {
            Fl_Pixmap* cellIcon = nullptr;

            if( nullptr != mMMB )
            {
                sprintf( cellData, "%03zu: %s", slot, mMMB->GetEntryName( slot ) );

                switch( mMMB->GetEntryAttribute( slot ) )
                {
                    case MMB_DISKATTRIBUTE_UNFORMATTED:
                        cellIcon = mIconEmpty;
                        DrawData( cellData, cellIcon, _x, _y, _w, _h, selected );
                        break;
                    case MMB_DISKATTRIBUTE_UNLOCKED:
                        cellIcon = mIconUnlocked;
                        DrawData( cellData, cellIcon, _x, _y, _w, _h, selected );
                        break;
                    case MMB_DISKATTRIBUTE_LOCKED:
                        cellIcon = mIconLocked;
                        DrawData( cellData, cellIcon, _x, _y, _w, _h, selected );
                        break;
                    default:
                        DrawUnused( _x, _y, _w, _h );
                        break;
                }

            }
            else
            {
                sprintf( cellData, "%03i: %s", (_col*32)+_row, "" );
            }
        }
        return;
    default:
        return;
    }
}

void CMMBETable::SelectSlot( size_t _slot )
{
    if( nullptr == mMMB || _slot >= mMMB->GetNumberOfDisks() )
    {
        mSelectedSlot = (size_t)-1;
        ((CAppWindow*)this->window())->RefreshDiscContent( nullptr, 0 );
    }
    else
    {
        mSelectedSlot = (mSelectedSlot == _slot) ? (size_t)-1 : _slot;
        if( mSelectedSlot < mMMB->GetNumberOfDisks() )
        {
            string errorString;
            unsigned char* data = new unsigned char[MMB_DISKSIZE];
            mMMB->ExtractImageInSlot( data, mSelectedSlot, errorString );
            ((CAppWindow*)this->window())->RefreshDiscContent( data, MMB_DISKSIZE );
            delete[] data;
        }
        else
        {
            ((CAppWindow*)this->window())->RefreshDiscContent( nullptr, 0 );
        }
    }

    redraw();
}

size_t CMMBETable::GetSelectedSlot()
{
    return mSelectedSlot;
}

CMMBEGui::CMMBEGui( int _w, int _h, const char* _label )
{
    Fl::visual(FL_RGB);

    mMainWindow = new CAppWindow( _w, _h, _label );

    mMainWindow->begin();

    CreateMenuBar();

    CreateControls();

    mMainWindow->end();

    mMainWindow->resizable( mTable );
}

CMMBEGui::~CMMBEGui()
{

}

int CMMBEGui::Run()
{
    if( nullptr != mMainWindow )
    {
        mMainWindow->show(0, nullptr);
        mMainWindow->redraw();
        return Fl::run();
    }

    return -1;
}

void CMMBEGui::OpenMMB( const std::string& _filename )
{
    std::string errorString;

    if( !mMMB.Open( _filename, errorString ) )
    {
        fl_alert("[ERROR] %s",errorString.c_str());
    }

    string filenameStr = "File: ";
    filenameStr += mMMB.GetFilename().c_str();
    mFilenameBox->copy_label( filenameStr.c_str() );

    // Refresh contents
    mMainWindow->RefreshDiscContent(nullptr,0);
    mTable->redraw();
}

void CMMBEGui::CreateMMB( const std::string& _filename )
{
    std::string errorString;

    CloseMMB();

    // Ask for MMB size
    const char* result = fl_input( "Enter number of slots on the new MMB file (1 - 511)", nullptr );
    if( nullptr == result )
    {
        return;
    }

    size_t numberOfDisks = (size_t)strtoul( result, nullptr, 0 );
    if( numberOfDisks == 0 || numberOfDisks > 511 )
    {
        return;
    }

    if( !mMMB.Create( _filename, numberOfDisks, errorString ) )
    {
        fl_alert("[ERROR] %s",errorString.c_str());

        string filenameStr = "File: ";
        mFilenameBox->copy_label( filenameStr.c_str() );
        mTable->redraw();
        return;
    }

    OpenMMB( _filename );

    // Refresh contents
    mTable->redraw();
}

void CMMBEGui::CloseMMB()
{
    mMMB.Close();

    string filenameStr = "File: ";
    mFilenameBox->copy_label( filenameStr.c_str() );

    mTable->SelectSlot( MMB_MAXNUMBEROFDISKS );

    // Refresh contents
    mTable->redraw();
}

void CMMBEGui::CreateMenuBar()
{
	// Menu bar
	#ifdef __APPLE__
	mMenuBar = new Fl_Sys_Menu_Bar(0,0,mMainWindow->w(),30);
	#else
	mMenuBar = new Fl_Menu_Bar(0,0,mMainWindow->w(),30);
	#endif

	// File menu
	mMenuBar->add( "&File/&Open MMB"      , mModifierKey+'o', openFile_cb  , (void*)this, 0 );
	mMenuBar->add( "&File/&Close MMB"     , mModifierKey+'c', closeFile_cb , (void*)this, 0 );
	mMenuBar->add( "&File/Create &new MMB", mModifierKey+'n', createFile_cb, (void*)this, 0 );
    // TODO:Add disk table/rebuild table option

	mMenuBar->add( "&File/&Quit", mModifierKey+'q', menuQuit_cb, (void*)mMainWindow, 0 );
}

void CMMBEGui::CreateControls()
{
    int x = 10;
    int y = 10 + mMenuBarOffset;

    // File name
    mFilenameBox = new Fl_Box( 10, y, 620, 26, "File: " );
    mFilenameBox->align( FL_ALIGN_INSIDE | FL_ALIGN_LEFT );
    y += 21;

    mTable = new CMMBETable( &mMMB, 10, y, 492, 384, 0 );
    mTable->begin();

    // Rows
    mTable->rows(32);             // how many rows
    mTable->row_header(0);        // enable row headers (along left)
    mTable->row_height_all(26);   // default height of rows
    mTable->row_resize(0);        // disable row resizing
    
    // Columns
    mTable->cols(16);             // how many columns
    mTable->col_header(0);        // enable column headers (along top)
    mTable->col_width_all(206);   // default width of columns
    mTable->col_resize(0);        // enable column resizing
    mTable->end();			      // end the Fl_Table group

    x += 492 + 10;
    Fl_Select_Browser* diskContent = new Fl_Select_Browser( x, y, 280, 384, "Disc content" );
	diskContent->align( FL_ALIGN_TOP );
	diskContent->type(FL_HOLD_BROWSER);
    mMainWindow->SetDiscContentWidget( diskContent );
}
