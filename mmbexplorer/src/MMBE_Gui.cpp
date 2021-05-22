#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <FL/Fl.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/x.H>
#include "MMBE_Gui.h"
#include "MMBE_Commands.h"
#include "MMBE_Callbacks.h"

// Icons
#include "../icons/empty.xpm"
#include "../icons/locked.xpm"
#include "../icons/unlocked.xpm"
#include "../logo/mmbexplorer_64x64.xpm"

using namespace std;

#ifdef WIN32
    const char PATH_SEPARATOR = '\\';
#else
    const char PATH_SEPARATOR = '/';
#endif

const int MMBEGUI_TABLECELL_HEIGHT = 26;   // default height of rows
const int MMBEGUI_TABLECELL_WIDTH  = 206;  // default width of columns

//******************************************
//* CRC32 calculation
//******************************************
uint32_t poly8_lookup[256] =
{
 0, 0x77073096, 0xEE0E612C, 0x990951BA,
 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
 0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
 0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
 0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
 0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
 0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
 0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
 0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
 0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
 0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
 0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
 0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
 0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
 0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

// calculate a checksum on a buffer -- start address = p, length = bytelength
uint32_t crc32_byte(uint8_t *p, uint32_t bytelength)
{
	uint32_t crc = 0xffffffff;
	while (bytelength-- !=0) crc = poly8_lookup[((uint8_t) crc ^ *(p++))] ^ (crc >> 8);
	// return (~crc); also works
	return (crc ^ 0xffffffff);
}
 
//Fill the lookup table -- table = the lookup table base address
void crc32_fill(uint32_t *table){
        uint8_t index=0,z;
        do{
                table[index]=index;
                for(z=8;z;z--) table[index]=(table[index]&1)?(table[index]>>1)^0xEDB88320:table[index]>>1;
        }while(++index);
}
 
//******************************************
//* CAppWindow class
//******************************************
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
    switch ( _event ) 
    {
        /*case FL_PUSH:               // do 'copy/dnd' when someone clicks on box
            Fl::copy(blabla.c_str(),(int)blabla.length(),0);//Fl::clipboard_image);
            Fl::dnd();
            ret = 1;
            break;*/
    }
    return(ret);
}

void CAppWindow::SetDiskContentWidget( Fl_Select_Browser* _diskContent )
{
    mDiskContent = _diskContent;
}

void CAppWindow::RefreshDiskContent( unsigned char* _data, size_t _dataSize )
{
    if( nullptr == mDiskContent )
    {
        return;
    }

    mDiskContent->clear();    

    if( nullptr != _data )
    {
        stringstream strStream;
        DFSDisk disk;

        DFSRead( _data, _dataSize, disk );

        string diskNameStr = "@fDisk name: " + disk.name;
        mDiskContent->add( diskNameStr.c_str() );
        string bootOptionStr = "@fBoot option: ";
        bootOptionStr += BootOptionToString( disk.bootOption );
        mDiskContent->add( bootOptionStr.c_str() );
        mDiskContent->add( "@f-+-+----------+------+-----+-----+--------");
        mDiskContent->add( "@fL|D|File name |Size  |Load |Exec |CRC32   ");
        mDiskContent->add( "@f-+-+----------+------+-----+-----+--------");
        
        size_t sectorsUsed = 2; // Filesystem sectors

        for( auto dfsFile : disk.files )
        {
            string fileStr = "@f";
            fileStr += dfsFile.locked ? "L " : "  ";
            fileStr += dfsFile.directory;
            fileStr += '.';

            // Pad file name to the maximum of 12 characters
            string paddedName = dfsFile.name;
            if( paddedName.length() < 11 )
            {
                paddedName.insert( paddedName.end(), 11 - paddedName.length(), ' ');
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

            // Compute and add CRC32
            uint32_t crc32 = crc32_byte( dfsFile.data.data(), dfsFile.data.size() );
            strStream.str("");
            strStream << hex << crc32;
            std::string crcStr = strStream.str();
            transform( crcStr.begin(), crcStr.end(), crcStr.begin(), ::toupper );
            fileStr += crcStr;

            // Add line to widget
            mDiskContent->add( fileStr.c_str() );
        }

        mDiskContent->add(    "@f-+-+----------+------+-----+-----+--------");
        string freeSpaceStr = "@f    Free Space ";
        string paddedFreeSpace = to_string( (disk.sectorsNum - min(sectorsUsed, (size_t)disk.sectorsNum)) * 256 );
        if( paddedFreeSpace.length() < 6 )
        {
            paddedFreeSpace.insert( paddedFreeSpace.begin(), 6 - paddedFreeSpace.length(), ' ' );
        }
        freeSpaceStr += paddedFreeSpace;
        mDiskContent->add( freeSpaceStr.c_str() );
    }
}

void CAppWindow::GetSelectedFiles( std::vector<int>& _dst )
{
    // Line numbers are 1 based. First 5 lines are the header lines,
    // last two lines are the footer.
    for( int line = 6; line <= mDiskContent->size() - 2; ++line )
    {
        if( mDiskContent->selected( line ) )
        {
            _dst.push_back( line - 6 );
        }
    }
}

//******************************************
//* CMMBESelectBrowser class
//******************************************
CMMBESelectBrowser::CMMBESelectBrowser( CMMBEGui* _gui, int _x, int _y, int _w, int _h, const char* _label ) : 
                    Fl_Select_Browser( _x, _y, _w, _h, _label ),
                    mGui( _gui )
{

}

CMMBESelectBrowser::~CMMBESelectBrowser()
{

}

int CMMBESelectBrowser::handle(int _event)
{
    int retVal = Fl_Select_Browser::handle( _event );

    switch( _event )
    {
        // Receive a dropped item
        case FL_DND_ENTER:
        case FL_DND_RELEASE:
        case FL_DND_LEAVE:
        case FL_DND_DRAG:
            retVal = 1;
            break;

        case FL_PASTE:
        {
            if( nullptr == mGui || mGui->GetSelectionSize() != 1 )
            {
                return 0;
            }

            std::string pastedText = Fl::event_text();

            // Split string in case we have several files to paste
            vector<string> fileNames;
            istringstream stringStream(pastedText);
            string tempString;
            bool wasInsertionOk = true;
            string failedFiles;

            while (getline(stringStream, tempString, '\n'))
            {
                fileNames.push_back(tempString);
            }
            std::string fileProtocol = "file://";

            size_t slot = mGui->GetSelection()[0];

            for( auto filename : fileNames )
            {
                // Check for file:// protocol
                if( 0 == filename.compare(0, fileProtocol.length(), fileProtocol) )
                {
                    filename = filename.substr( fileProtocol.length(), std::string::npos );
                }

                // Remove unwanted characters...
                filename.erase(std::remove(filename.begin(), filename.end(), 0xD), filename.end());
                filename.erase(std::remove(filename.begin(), filename.end(), 0xA), filename.end());

                mGui->InsertFile( slot, filename );
            }

            retVal = 1;
        }
        break;
    }

    return retVal;
}

//******************************************
//* CMMBETable class
//******************************************
CMMBETable::CMMBETable( CMMBEGui* _gui, CMMBFile* _mmb, int _x, int _y, int _w, int _h, const char* _label ) : Fl_Table( _x, _y, _w, _h, _label )
{
    mIconEmpty    = new Fl_Pixmap( iconEmpty );
    mIconLocked   = new Fl_Pixmap( iconLocked );
    mIconUnlocked = new Fl_Pixmap( iconUnlocked );
    mMMB          = _mmb;
    mGui          = _gui;
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
    int retVal = Fl_Table::handle( _event );

    // Compute the targeted slot
    int row = 0;
    int col = 0;
    ResizeFlag rf;
    cursor2rowcol ( row, col, rf );
    size_t slot = (col * rows()) + row;

    EMMBETable_SelectionType selectionType = EMMBETable_Single;
    if( Fl::event_shift() )
    {
        selectionType = EMMBETable_Multiple;
    }
    else if( Fl::event_command() | Fl::event_ctrl() )
    {
        selectionType = EMMBETable_SingleAdd;
    }

    switch( _event )
    {
        case FL_RELEASE:
        {
            if( Fl::event_x() < tix || Fl::event_x() >= tix + tiw ||
                Fl::event_y() < tiy || Fl::event_y() >= tiy + tih )
            {
                return Fl_Table::handle( _event );
            }

            switch( Fl::event_button() )
            {
                case FL_LEFT_MOUSE:
                    SelectSlot( slot, selectionType );
                    break;
                default:
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
            std::string pastedText = Fl::event_text();

            // Split string in case we have several files to paste
            vector<string> fileNames;
            istringstream stringStream(pastedText);
            string tempString;
            bool wasInsertionOk = true;
            string failedFiles;

            while (getline(stringStream, tempString, '\n'))
            {
                fileNames.push_back(tempString);
            }
            std::string fileProtocol = "file://";

            if( nullptr != mMMB && nullptr != mGui && !fileNames.empty() )
            {
                if( GetFileSize( fileNames[0] ) > MMB_DISKSIZE )
                {
                    string currentMMB = mMMB->GetFilename();
                    string errorString;
                    string name = fileNames[0];
                    
                    if( 0 == name.compare(0, fileProtocol.length(), fileProtocol) )
                    {
                        name = name.substr( fileProtocol.length(), std::string::npos );
                    }

                    if( mMMB->Open( fileNames[0], errorString ) )
                    {
                        SelectSlot( 0, CMMBETable::EMMBETable_Single );
                        return 1;
                    }
                    else
                    {
                        if( !currentMMB.empty() )
                        {
                            mMMB->Open( currentMMB, errorString );
                            return 1;
                        }
                    }
                }
            }

            if( nullptr == mMMB || slot >= mMMB->GetNumberOfDisks() )
            {
                return 1;
            }


            // Clear selection
            SelectSlot( MMB_MAXNUMBEROFDISKS, CMMBETable::EMMBETable_Single );
            bool isFirstSlot = true;

            for( auto filename : fileNames )
            {
                if( slot >= mMMB->GetNumberOfDisks() )
                {
                    break;
                }

                // Check for file:// protocol
                if( 0 == filename.compare(0, fileProtocol.length(), fileProtocol) )
                {
                    filename = filename.substr( fileProtocol.length(), std::string::npos );
                }

                // Remove unwanted characters...
                filename.erase(std::remove(filename.begin(), filename.end(), 0xD), filename.end());
                filename.erase(std::remove(filename.begin(), filename.end(), 0xA), filename.end());

                std::string errorString;
                if( !mMMB->InsertImageInSlot( filename, slot, errorString ) )
                {
                    wasInsertionOk = false;

                    failedFiles += filename;
                    failedFiles += "\n";
                }

                SelectSlot( slot, isFirstSlot ? CMMBETable::EMMBETable_Single : CMMBETable::EMMBETable_SingleAdd );
                isFirstSlot = false;
                ++slot;
            }

            if( !wasInsertionOk )
            {
                fl_alert("[ERROR] Could not add these files:\n%s",failedFiles.c_str());
            }

            return 1;
        }
        break;
    }

    return retVal;
}

void CMMBETable::draw_cell( TableContext context, int _row, int _col, int _x, int _y, int _w, int _h )
{
    string tmp;
    char cellData[32] = {0};
    size_t slot = (_col * Fl_Table::rows()) + _row;
    bool selected = IsSlotSelected( slot );

    switch ( context ) 
    {
    case CONTEXT_STARTPAGE:                   // before page is drawn..
        fl_font(FL_COURIER, 16);              // set the font for our drawing operations
        return; 
    case CONTEXT_COL_HEADER:                  // Draw column headers
        tmp = to_string( _col * Fl_Table::rows() );
        DrawHeader( tmp.c_str(), _x, _y, _w, _h );
        return; 
    case CONTEXT_ROW_HEADER:                  // Draw row headers
        tmp = to_string( _row );
        DrawHeader( tmp.c_str(), _x, _y, _w, _h );
        return; 
    case CONTEXT_CELL:                        // Draw data in cells
        if( slot > 510 )
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
                sprintf( cellData, "%03i: %s", (_col * Fl_Table::rows()) + _row, "" );
            }
        }
        return;
    default:
        return;
    }
}

void CMMBETable::resize( int _x, int _y ,int _w ,int _h )
{
    int cols = max( 1, _w / MMBEGUI_TABLECELL_WIDTH );
    int rows = 512 / cols;
    if( 0 != (512 % cols) )
    {
        ++rows;
    }
    Fl_Table::rows(rows);
    Fl_Table::cols(cols);

    Fl_Table::resize( _x, _y, _w, _h );

    mGui->OnWindowResize();
}

void CMMBETable::SelectSlot( size_t _slot, EMMBETable_SelectionType _selectionType )
{
    if( nullptr == mMMB || _slot >= mMMB->GetNumberOfDisks() )
    {
        mLastSelectedSlot = (size_t)-1;
        mSelectedSlots.clear();
        ((CAppWindow*)this->window())->RefreshDiskContent( nullptr, 0 );
    }
    else
    {
        if( _selectionType == EMMBETable_Single )
        {
            // Single click.
            // Deselect all other slots, select the clicked one.
            size_t selectionSize = GetSelectionSize();
            if( selectionSize > 1 || selectionSize == 0 )
            {
                mSelectedSlots.clear();
                AddSlotToSelection( _slot );
            }
            else if( selectionSize == 1 )
            {
                if( IsSlotSelected( _slot ) )
                {
                    RemoveSlotFromSelection( _slot );
                }
                else
                {
                    mSelectedSlots.clear();
                    AddSlotToSelection( _slot );
                }
            }
        }
        else if( _selectionType == EMMBETable_SingleAdd )
        {
            // Single click with Command or Control keys pressed.
            // Change the clicked widget state.
            if( IsSlotSelected( _slot ) )
            {
                RemoveSlotFromSelection( _slot );
            }
            else
            {
                AddSlotToSelection( _slot );
            }
        }
        else // EMMBETable_Multiple
        {
            // Selects range
            if( mLastSelectedSlot != (size_t)-1 && mLastSelectedSlot != _slot )
            {
                mSelectedSlots.clear();

                size_t startSlot = min( mLastSelectedSlot, _slot );
                size_t endSlot   = max( mLastSelectedSlot, _slot );

                for( ; startSlot <= endSlot; ++startSlot )
                {
                    mSelectedSlots.push_back( startSlot );
                }
            }
        }
    }

    // Refresh contents
    mLastSelectedSlot = _slot;

    if( mSelectedSlots.size() == 1 )
    {
        string errorString;
        unsigned char* data = new unsigned char[MMB_DISKSIZE];
        mMMB->ExtractImageInSlot( data, mSelectedSlots[0], errorString );
        ((CAppWindow*)this->window())->RefreshDiskContent( data, MMB_DISKSIZE );
        delete[] data;
    }
    else
    {
        ((CAppWindow*)this->window())->RefreshDiskContent( nullptr, 0 );
    }

    redraw();
}

bool CMMBETable::IsSlotSelected( size_t _slot )
{
    return std::find( mSelectedSlots.begin(), mSelectedSlots.end(), _slot ) != mSelectedSlots.end();
}

void CMMBETable::AddSlotToSelection( size_t _slot )
{
    vector<size_t>::const_iterator slotIterator = std::find( mSelectedSlots.begin(), mSelectedSlots.end(), _slot );
    if( slotIterator == mSelectedSlots.end() )
    {
        mSelectedSlots.push_back( _slot );
    }
}

void CMMBETable::RemoveSlotFromSelection( size_t _slot )
{
    vector<size_t>::const_iterator slotIterator = std::find( mSelectedSlots.begin(), mSelectedSlots.end(), _slot );
    if( slotIterator != mSelectedSlots.end() )
    {
        mSelectedSlots.erase( slotIterator );
    }
}

void CMMBETable::DoRedraw()
{
    redraw();
}

size_t CMMBETable::GetSelectionSize()
{
    return mSelectedSlots.size();
}

const std::vector<size_t>& CMMBETable::GetSelection()
{
    return mSelectedSlots;
}

void CMMBETable::ClearSelection()
{
    mSelectedSlots.clear();
}

//******************************************
//* CMMBEGui class
//******************************************
CMMBEGui::CMMBEGui( int _w, int _h, const char* _label )
{
    Fl::visual(FL_RGB);

    mMainWindow = new CAppWindow( _w, _h + mMenuBarOffset, _label );

    mMainWindow->begin();

    CreateMenuBar();

    CreateControls();

    mMainWindow->end();

#ifndef __APPLE__
#ifdef WIN32
	mMainWindow->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#else
    Fl_Pixmap xpmIcon( mmbexplorer_64x64 );
    Fl_RGB_Image appIcon( &xpmIcon, Fl_Color(0) );
    mMainWindow->icon( &appIcon );
#endif
#endif

    mMainWindow->resizable( mTable );
}

CMMBEGui::~CMMBEGui()
{

}

int CMMBEGui::Run( int argc, char** argv )
{
    if( argc > 1 )
    {
        OpenMMB( argv[1] );
    }

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
    mTable->SelectSlot( 0, CMMBETable::EMMBETable_Single );
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

    mTable->SelectSlot( MMB_MAXNUMBEROFDISKS, CMMBETable::EMMBETable_Single );

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
	#ifndef __APPLE__
    mMenuBar->add( "&File/&Quit"          , mModifierKey+'q', menuQuit_cb  , (void*)mMainWindow, 0 );
    #endif

    // Slot menu
	mMenuBar->add( "&Slot/&Insert disk(s)", mModifierKey+'i', insertDisk_cb  , (void*)this, 0 ); // If no slot selected, ask for slot
	mMenuBar->add( "&Slot/&Extract disk"  , mModifierKey+'e', extractDisk_cb , (void*)this, 0 );
	mMenuBar->add( "&Slot/&Remove disk"   , mModifierKey+'r', removeDisk_cb  , (void*)this, 0 );
	mMenuBar->add( "&Slot/&Lock disk"     , mModifierKey+'l', lockDisk_cb    , (void*)this, 0 );
	mMenuBar->add( "&Slot/&Unlock disk"   , mModifierKey+'u', unlockDisk_cb  , (void*)this, 0 );

    // Disk menu
    mMenuBar->add( "&Disk/&Format disk"   , FL_SHIFT+mModifierKey+'f', formatDisk_cb  , (void*)this, 0 );
    mMenuBar->add( "&Disk/&Name disk"     , FL_SHIFT+mModifierKey+'n', nameDisk_cb    , (void*)this, 0 );
    mMenuBar->add( "&Disk/&Insert file"   , FL_SHIFT+mModifierKey+'i', insertFile_cb  , (void*)this, 0 );
    mMenuBar->add( "&Disk/&Extract file"  , FL_SHIFT+mModifierKey+'e', extractFile_cb , (void*)this, 0 );
    mMenuBar->add( "&Disk/&Remove file"   , FL_SHIFT+mModifierKey+'r', removeFile_cb  , (void*)this, 0 );
    mMenuBar->add( "&Disk/&Lock file"     , FL_SHIFT+mModifierKey+'l', lockFile_cb    , (void*)this, 0 );
    mMenuBar->add( "&Disk/&Unlock file"   , FL_SHIFT+mModifierKey+'u', unlockFile_cb  , (void*)this, 0 );

    mMenuBar->add( "&Disk/&Boot option/&None"   , mModifierKey+'0', setBootOption0_cb  , (void*)this, 0 );
    mMenuBar->add( "&Disk/&Boot option/&Load"   , mModifierKey+'1', setBootOption1_cb  , (void*)this, 0 );
    mMenuBar->add( "&Disk/&Boot option/&Run"    , mModifierKey+'2', setBootOption2_cb  , (void*)this, 0 );
    mMenuBar->add( "&Disk/&Boot option/&Exec"   , mModifierKey+'3', setBootOption3_cb  , (void*)this, 0 );

    // Help menu
	#ifndef __APPLE__
    mMenuBar->add( "&Help/&About"         , mModifierKey+'a', menuAbout_cb  , (void*)mMainWindow, 0 );
    #endif
}

void CMMBEGui::CreateControls()
{
    int x = 10;
    int y = 10 + mMenuBarOffset;

    // File name
    mFilenameBox = new Fl_Box( 10, y, 620, 26, "File: " );
    mFilenameBox->align( FL_ALIGN_INSIDE | FL_ALIGN_LEFT );
    y += 21;
    
    mTable = new CMMBETable( this, &mMMB, 10, y, 472, 384, 0 );
    mTable->begin();

    mSlotContextMenu = new Fl_Menu_Button( 10, y, 472, 384, "Slot actions");
    mSlotContextMenu->type(Fl_Menu_Button::POPUP3);         // pops menu on right click        
    mSlotContextMenu->add("Format disk(s)" , 0, formatDisk_cb , (void*)this, 0 );
    mSlotContextMenu->add("Name disk"      , 0, nameDisk_cb   , (void*)this, 0 );
    mSlotContextMenu->add("Insert disk"    , 0, insertDisk_cb , (void*)this, 0 );
    mSlotContextMenu->add("Extract disk(s)", 0, extractDisk_cb, (void*)this, 0 );
    mSlotContextMenu->add("Remove disk(s)" , 0, removeDisk_cb , (void*)this, 0 );
    mSlotContextMenu->add("Lock disk(s)"   , 0, lockDisk_cb   , (void*)this, 0 );
    mSlotContextMenu->add("Unlock disk(s)" , 0, unlockDisk_cb , (void*)this, 0 );

    // Rows
    mTable->rows(32);                                   // how many rows
    mTable->row_header(0);                              // enable row headers (along left)
    mTable->row_height_all(MMBEGUI_TABLECELL_HEIGHT);   // default height of rows
    mTable->row_resize(0);                              // disable row resizing
    
    // Columns
    mTable->cols(16);                                   // how many columns
    mTable->col_header(0);                              // enable column headers (along top)
    mTable->col_width_all(MMBEGUI_TABLECELL_WIDTH);     // default width of columns
    mTable->col_resize(0);                              // enable column resizing
    
    mTable->end();			                            // end the Fl_Table group

    x += 472 + 10;
    mDiskContent = new CMMBESelectBrowser( this, x, y, 350, 384, "Disk content" );
	mDiskContent->color( FL_WHITE );
    mDiskContent->align( FL_ALIGN_TOP );
	mDiskContent->type(FL_MULTI_BROWSER);
    mMainWindow->SetDiskContentWidget( mDiskContent );

    mDiskContextMenu = new Fl_Menu_Button( mDiskContent->x(), mDiskContent->y(), mDiskContent->w(), mDiskContent->h(), "Disk actions");
    mDiskContextMenu->type(Fl_Menu_Button::POPUP3);         // pops menu on right click        
    mDiskContextMenu->add("Format disk(s)"   , 0, formatDisk_cb      , (void*)this, 0 );
    mDiskContextMenu->add("Name disk"        , 0, nameDisk_cb        , (void*)this, 0 );
    mDiskContextMenu->add("Insert file(s)"   , 0, insertFile_cb      , (void*)this, 0 );
    mDiskContextMenu->add("Extract file(s)"  , 0, extractFile_cb     , (void*)this, 0 );
    mDiskContextMenu->add("Remove file(s)"   , 0, removeFile_cb      , (void*)this, 0 );
    mDiskContextMenu->add("Lock file(s)"     , 0, lockFile_cb        , (void*)this, 0 );
    mDiskContextMenu->add("Unlock file(s)"   , 0, unlockFile_cb      , (void*)this, 0 );
    mDiskContextMenu->add("_Copy file(s) CRC", 0, copyFilesCRC_cb    , (void*)this, 0 );
    mDiskContextMenu->add("Boot option None" , 0, setBootOption0_cb  , (void*)this, 0 );
    mDiskContextMenu->add("Boot option Load" , 0, setBootOption1_cb  , (void*)this, 0 );
    mDiskContextMenu->add("Boot option Run"  , 0, setBootOption2_cb  , (void*)this, 0 );
    mDiskContextMenu->add("Boot option Exec" , 0, setBootOption3_cb  , (void*)this, 0 );
}

size_t CMMBEGui::GetNumberOfSlots() const
{
    return mMMB.GetNumberOfDisks();    
}

void CMMBEGui::InsertDisk ( const std::string& _filename, size_t _slot )
{
    std::string errorString;

    if( !mMMB.InsertImageInSlot( _filename, _slot, errorString ) )
    {
        fl_alert( "[ERROR] %s",errorString.c_str() );
    }
    else
    {
        if( GetSelectionSize() > 1 )
        {
            mTable->redraw();
            return;
        }

        unsigned char* pData = new unsigned char[MMB_DISKSIZE];
        if( nullptr == pData )
        {
            fl_alert( "[ERROR] Could't allocate %zu bytes for temporal storage.",MMB_DISKSIZE );
            return;
        }

        if( !mMMB.ExtractImageInSlot( pData, _slot, errorString ) )
        {
            delete[] pData;
            fl_alert( "[ERROR] %s", errorString.c_str() );
        }

        if( mTable->IsSlotSelected(_slot) )
        {
            mMainWindow->RefreshDiskContent( pData, MMB_DISKSIZE);    
        }
        else
        {
            mMainWindow->RefreshDiskContent( nullptr, 0);
        }

        delete[] pData;
    }
    

    // Refresh contents
    mTable->redraw();
}

void CMMBEGui::ExtractDisk( const std::string& _filename, size_t _slot )
{
    std::string errorString;

    if( !mMMB.ExtractImageInSlot( _filename, _slot, errorString ) )
    {
        fl_alert("[ERROR] %s",errorString.c_str());        
    }

    // Refresh contents
    mTable->redraw();
}

void CMMBEGui::RemoveDisk ( size_t _slot )
{
    std::string errorString;

    if( !mMMB.RemoveImageFromSlot( _slot, errorString ) )
    {
        fl_alert("[ERROR] %s",errorString.c_str());        
    }
    else
    {
        mMainWindow->RefreshDiskContent( nullptr, 0 );
    }

    // Refresh contents
    mTable->redraw();
}

void CMMBEGui::LockDisk   ( size_t _slot )
{
    std::string errorString;

    if( !mMMB.LockImageInSlot( _slot, errorString ) )
    {
        fl_alert("[ERROR] %s",errorString.c_str());        
    }

    // Refresh contents
    mTable->redraw();
}

void CMMBEGui::UnlockDisk ( size_t _slot )
{
    std::string errorString;

    if( !mMMB.UnlockImageInSlot( _slot, errorString ) )
    {
        fl_alert("[ERROR] %s",errorString.c_str());        
    }

    // Refresh contents
    mTable->redraw();
}

void CMMBEGui::ExtractSelectedDisks()
{
    if( GetSelectionSize() == 0 )
    {
        size_t slot = ChooseSlot();
		std::string filename;
		if( !ChooseFilename( filename, "SSD files\t*.ssd\n", ".", true) )
			return;

		ExtractDisk( filename, slot );
    }
    else if( GetSelectionSize() == 1 )
    {
   		std::string filename;
		if( !ChooseFilename( filename, "SSD files\t*.ssd\n", ".", true) )
			return;

        ExtractDisk( filename, mTable->GetSelection()[0] );
    }
    else
    {
        // Choose export folder
        string folderName;
        if( !ChooseFilename( folderName, "", "", true, true ) )
            return;

    #ifdef WIN32
        if( folderName.back() != '\\' ) folderName += '\\';
    #else
        if( folderName.back() != '/' ) folderName += '/';
    #endif

        for( auto slot : mTable->GetSelection() )
        {
            unsigned char diskAttribute = mMMB.GetEntryAttribute(slot);
            if( MMB_DISKATTRIBUTE_INVALID == diskAttribute || MMB_DISKATTRIBUTE_UNFORMATTED == diskAttribute )
            {
                continue;
            }

            string filename = folderName;

            string slotStr = to_string(slot);
            if( slotStr.length() < 3 )
            {
                slotStr.insert( slotStr.begin(), 3 - slotStr.length(), '0');
            }
            filename += slotStr;
            filename += '_';
            filename += mMMB.GetEntryName( slot );
            filename += ".ssd";

            ExtractDisk( filename, slot );
        }
    }
}

void CMMBEGui::ExtractSelectedFiles()
{
    if( GetSelectionSize() != 1 )
    {
        return;
    }

    std::vector<int> selectedFiles;
    GetSelectedFiles( selectedFiles );
    if( selectedFiles.empty() )
    {
        return;
    }

    // Choose export folder
    string folderName;
    if( !ChooseFilename( folderName, "", "", true, true ) )
        return;

    if( folderName.back() != PATH_SEPARATOR ) folderName += PATH_SEPARATOR;

    DFSDisk disk;
    char buffer[256] = { 0 };
    size_t slot = GetSelection()[0];
    std::string errorString;
    std::vector<unsigned char> data;
    data.resize( MMB_DISKSIZE );

    if( !mMMB.ExtractImageInSlot( data.data(), slot, errorString ) )
    {
        fl_alert( "[ERROR] %s", errorString.c_str() );
        return;
    }
    DFSRead( data.data(), MMB_DISKSIZE, disk );

    for( auto file : selectedFiles )
    {
        std::string filename;
        filename += (char)disk.files[file].directory;
        filename += ".";
        filename += disk.files[file].name;
        std::string bbcFilename = filename; // Original BBC name, for inf file
        BBCString2Host( filename );
        auto stringIter = filename.crbegin();
        while( filename.length() && filename.back() == ' ' )
        {
            filename.resize( filename.length() - 1 );
        }
        std::string destFilename = folderName + filename;

        // Write file
        FILE* pFile = fopen( destFilename.c_str(), "wb" );
        if( nullptr == pFile )
        {
           fl_alert( "[ERROR] Could not write file %s", destFilename.c_str() ); 
           continue;
        }

        fwrite( disk.files[file].data.data(), 1, disk.files[file].data.size(), pFile );
        fclose( pFile );

        // Write inf file
        sprintf( buffer, "%s %X %X %s", bbcFilename.c_str(), 
                                        disk.files[file].loadAddress,
                                        disk.files[file].execAddress,
                                        disk.files[file].locked ? "L" : "" );
        destFilename += ".inf";
        FILE* pInfFile = fopen( destFilename.c_str(), "w" );
        if( nullptr == pInfFile )
        {
           fl_alert( "[ERROR] Could not write inf file %s", destFilename.c_str() ); 
           continue;
        }

        fwrite( buffer, 1, strlen(buffer), pInfFile );
        fclose( pInfFile );
    }
}

void CMMBEGui::RemoveSelectedDisks()
{
    for( auto slot : mTable->GetSelection() )
    {
        RemoveDisk( slot );
    }
}

void CMMBEGui::LockSelectedDisks()
{
    for( auto slot : mTable->GetSelection() )
    {
        unsigned char diskAttribute = mMMB.GetEntryAttribute(slot);
        if( MMB_DISKATTRIBUTE_INVALID != diskAttribute && MMB_DISKATTRIBUTE_UNFORMATTED != diskAttribute )
        {
            LockDisk( slot );
        }
    }
}

void CMMBEGui::UnlockSelectedDisks()
{
    for( auto slot : mTable->GetSelection() )
    {
        unsigned char diskAttribute = mMMB.GetEntryAttribute(slot);
        if( MMB_DISKATTRIBUTE_INVALID != diskAttribute && MMB_DISKATTRIBUTE_UNFORMATTED != diskAttribute )
        {
            UnlockDisk( slot );
        }
    }
}

size_t CMMBEGui::GetSelectionSize()
{
    return mTable->GetSelectionSize();
}

void CMMBEGui::ShowAboutDialog()
{
    if( nullptr != Fl::modal() )
    {
        return;
    }

    int aboutDialogWidth  = 284;
    int aboutDialogHeight = 195;

    Fl_Window* aboutDialog = new Fl_Window( aboutDialogWidth, aboutDialogHeight, nullptr );
    if( nullptr == aboutDialog )
    {
        return;
    }
#ifdef WIN32
    aboutDialog->resizable( aboutDialog );
#endif
    aboutDialog->box(FL_FLAT_BOX);
    aboutDialog->color( FL_DARK2 );
    aboutDialog->set_modal();
    aboutDialog->begin();

    int ySeparation = 8;
    int y = ySeparation;

    Fl_Pixmap* aboutIcon = new Fl_Pixmap( mmbexplorer_64x64 );
    Fl_Box* aboutIconBox = new Fl_Box( (aboutDialogWidth - aboutIcon->w()) / 2, y, aboutIcon->w(), aboutIcon->h() );
    aboutIconBox->image(aboutIcon);
    y += aboutIcon->h() + ySeparation;

    int textWidth  = 104;
    int textHeight = 15;

    Fl_Box* aboutText1 = new Fl_Box( (aboutDialogWidth - textWidth)/2, y, textWidth, textHeight, "MMBExplorer" );
    aboutText1->align( FL_ALIGN_CENTER );
    aboutText1->labelcolor( FL_BLACK );
    aboutText1->labelsize( 14 );
    aboutText1->labelfont( FL_HELVETICA_BOLD );
    y += textHeight + ySeparation;

    std::string versionStr = "Version 1.2.6 (";
    versionStr += GetSCMVersion();
    versionStr += ")";
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

    Fl_Box* aboutText4 = new Fl_Box( (aboutDialogWidth - textWidth)/2, y, textWidth, textHeight * 2, "Copyright 2020 by Roberto Carlos\nFernández Gerhardt" );
    aboutText4->align( FL_ALIGN_CENTER );
    aboutText4->labelcolor( FL_BLACK );
    aboutText4->labelsize( 12 );
    aboutText4->labelfont( FL_HELVETICA );

    aboutDialog->end();
    aboutDialog->show();
}

const std::vector<size_t>& CMMBEGui::GetSelection()
{
    return mTable->GetSelection();
}

void CMMBEGui::FormatDisk( size_t _slot )
{
    std::vector<unsigned char> data;
    data.resize( MMB_DISKSIZE, 0 );

    DFSDisk tmpDisk;
    if( !DFSWrite( data.data(), MMB_DISKSIZE, tmpDisk ) )
    {
        fl_alert( "[ERROR] Number of sectors in image and image size don't match." );
        return;
    }

    std::string errorString;
    if( !mMMB.InsertImageInSlot( data.data(), MMB_DISKSIZE, _slot, errorString ) )
    {
        fl_alert( "[ERROR] %s", errorString.c_str() );
    }

    mTable->redraw();

    if( mTable->GetSelectionSize() == 1 )
    {
        size_t slot = mTable->GetSelection()[0];
        mTable->ClearSelection();
        mTable->SelectSlot( slot, CMMBETable::EMMBETable_Single );
    }
}

void CMMBEGui::NameDisk( size_t _slot, const std::string& _diskname )
{
    std::string errorString;
    if( !mMMB.NameDisk( _slot, _diskname, errorString) )
    {
        fl_alert( "[ERROR] %s", errorString.c_str() );
    }

    mTable->redraw();

    if( mTable->GetSelectionSize() == 1 )
    {
        size_t slot = mTable->GetSelection()[0];
        mTable->ClearSelection();
        mTable->SelectSlot( slot, CMMBETable::EMMBETable_Single );
    }
}

void CMMBEGui::InsertFile( size_t _slot, const std::string& _filename )
{
    std::string errorString;
    unsigned char* data = new unsigned char[MMB_DISKSIZE];
    if( nullptr == data )
    {
        return;
    }
    if( 0 != (mMMB.GetEntryAttribute( _slot ) & 0xF0) )
    {
        FormatDisk( _slot );
    }
    if( !mMMB.ExtractImageInSlot( data, _slot, errorString ) )
    {
        delete[] data;
        fl_alert( "[ERROR] %s", errorString.c_str() );
        return;
    }
    
    DFSDisk disk;
    DFSRead( data, MMB_DISKSIZE, disk );

    DFSEntry dfsFile;
    if( !LoadFile( _filename, dfsFile, errorString) )
    {
        delete[] data;
        fl_alert( "[ERROR] %s", errorString.c_str() );
        return;
    }

    disk.files.push_back( dfsFile );
    DFSPackFiles( disk );
    if( !DFSWrite( data, MMB_DISKSIZE, disk) )
    {
        delete[] data;
        return;
    }

    if( !mMMB.InsertImageInSlot( data, MMB_DISKSIZE, _slot, errorString ) )
    {
        delete[] data;
        fl_alert( "[ERROR] %s", errorString.c_str() );
        return;
    }

    RefreshDiskContent( _slot );

    delete[] data;
}

void CMMBEGui::RemoveFile( size_t _slot, size_t _fileIndex )
{
    std::string errorString;
    unsigned char* data = new unsigned char[MMB_DISKSIZE];
    if( nullptr == data )
    {
        return;
    }
    if( !mMMB.ExtractImageInSlot( data, _slot, errorString ) )
    {
        delete[] data;
        fl_alert( "[ERROR] %s", errorString.c_str() );
        return;
    }
    
    DFSDisk disk;
    DFSRead( data, MMB_DISKSIZE, disk );

    if( _fileIndex >= disk.files.size() )
    {
        delete[] data;
        return;
    }

    disk.files.erase( disk.files.begin() + _fileIndex );
    DFSPackFiles( disk );
    if( !DFSWrite( data, MMB_DISKSIZE, disk) )
    {
        delete[] data;
        return;
    }

    if( !mMMB.InsertImageInSlot( data, MMB_DISKSIZE, _slot, errorString ) )
    {
        delete[] data;
        fl_alert( "[ERROR] %s", errorString.c_str() );
        return;
    }

    RefreshDiskContent( _slot );

    delete[] data;
}

void CMMBEGui::LockFile( size_t _slot, size_t _fileIndex )
{
    std::string errorString;
    if( !mMMB.LockFile( _slot, _fileIndex, errorString ) )
    {
        fl_alert( "[ERROR] %s", errorString.c_str() );
    }

    mTable->redraw();

    RefreshDiskContent( _slot );
}

void CMMBEGui::UnlockFile( size_t _slot, size_t _fileIndex )
{
    std::string errorString;
    if( !mMMB.UnlockFile( _slot, _fileIndex, errorString ) )
    {
        fl_alert( "[ERROR] %s", errorString.c_str() );
    }

    mTable->redraw();

    RefreshDiskContent( _slot );
}

void CMMBEGui::GetSelectedFiles( std::vector<int>& _dst )
{
    return mMainWindow->GetSelectedFiles( _dst );
}

void CMMBEGui::RefreshDiskContent( size_t _slot )
{
    std::string errorString;
    unsigned char* data = new unsigned char[MMB_DISKSIZE];
    if( nullptr == data )
    {
        return;
    }
    mMMB.ExtractImageInSlot( data, _slot, errorString );
    mMainWindow->RefreshDiskContent( data, MMB_DISKSIZE );
    delete[] data;
}

void CMMBEGui::SetBootOption( size_t _slot, unsigned char _bootOption )
{
    std::string errorString;
    if( !mMMB.SetBootOption( _slot, _bootOption, errorString ) )
    {
        fl_alert( "[ERROR] %s", errorString.c_str() );
    }

    mTable->redraw();

    if( GetSelectionSize() == 1 )
    {
        RefreshDiskContent( _slot );
    }
}

uint32_t CMMBEGui::GetFileCRC( size_t _slot, size_t _fileIndex )
{
    uint32_t retVal = 0;
    std::string errorString;
    unsigned char* data = new unsigned char[MMB_DISKSIZE];
    if( nullptr == data )
    {
        return retVal;
    }
    mMMB.ExtractImageInSlot( data, _slot, errorString );

    DFSDisk disk;

    DFSRead( data, MMB_DISKSIZE, disk );
    if( _fileIndex < disk.files.size() )
    {
        retVal = crc32_byte( disk.files[_fileIndex].data.data(), disk.files[_fileIndex].data.size() );
    }

    delete[] data;

    return retVal;
}

bool CMMBEGui::LoadFile( const std::string& _filename, DFSEntry& _dst, std::string& _errorString )
{
    FILE* pFile = fopen( _filename.c_str(), "rb" );
    if( nullptr == pFile )
    {
        _errorString = "Could not open file ";
        _errorString += _filename;
        return false;
    }

    fseek( pFile, 0, SEEK_END );
    _dst.fileSize = ftell( pFile );
    fseek( pFile, 0, SEEK_SET );

    _dst.data.resize( _dst.fileSize );
    size_t bytesRead = fread( _dst.data.data(), 1, _dst.fileSize, pFile );
    fclose( pFile );
    
    size_t pos = _filename.find_last_of( PATH_SEPARATOR );
    if( pos == std::string::npos )
    {
        pos = 0;
    }
    _dst.directory = '$';
    _dst.name = _filename.substr( pos + 1 );
    if( _dst.name.length() > 1 && _dst.name[1] == '.' )
    {
        _dst.directory = _dst.name[0];
        _dst.name = _dst.name.substr( 2 );
    }
    HostString2BBC( _dst.name );
    _dst.locked = false;

    // Try to read inf file
    std::string infFilename = _filename + ".inf";
    FILE* pInfFile = fopen( infFilename.c_str(), "r" );
    if( pInfFile )
    {
        fseek( pInfFile, 0, SEEK_END );
        size_t infFileSize = ftell( pInfFile );
        fseek( pInfFile, 0, SEEK_SET );
        
        char* infData = new char[infFileSize];
        size_t infBytesRead = fread( infData, 1, infFileSize, pInfFile );
        fclose( pInfFile );

        // Tokenize inf data
        vector<string> tokens;
        istringstream infStream( infData );
        string s;    
        while (getline(infStream, s,' ')) 
        {
            if( !s.empty() )
            {
                tokens.push_back(s);
            }
        }

        if( tokens.size() > 0 )
        {
            _dst.name = tokens[0];
            if( _dst.name.length() > 1 && _dst.name[1] == '.' )
            {
                _dst.directory = _dst.name[0];
                _dst.name = _dst.name.substr( 2 );
            }
            //HostString2BBC( _dst.name ); // This is not needed as the correct BBC file name should be stored here.
        }
        if( tokens.size() > 1 )
        {
            _dst.loadAddress = strtoul( tokens[1].c_str(), nullptr, 16 );
        }
        if( tokens.size() > 2 )
        {
            _dst.execAddress = strtoul( tokens[2].c_str(), nullptr, 16 );
        }
        if( tokens.size() > 3 && tokens[3].length() > 0 && tokens[3].at(0) == 'L' )
        {
            _dst.locked = true;
        }

        // Fix load and exec addresses
        if( (_dst.loadAddress & 0x30000) == 0x30000 )
        {
            _dst.loadAddress |= 0xFF0000;
        }
        if( (_dst.execAddress & 0x30000) == 0x30000 )
        {
            _dst.execAddress |= 0xFF0000;
        }

        delete[] infData;
    }

    return true;
}

void CMMBEGui::BBCString2Host( std::string& _string )
{
    for( auto& character : _string )
    {
        switch( character )
        {
            case '?':character = '#'; break; 
            case '/':character = '.'; break; 
            case '<':character = '$'; break; 
            case '>':character = '^'; break; 
            case '+':character = '&'; break; 
            case '=':character = '@'; break; 
            case ';':character = '%'; break; 
        }
    }
}

void CMMBEGui::HostString2BBC( std::string& _string )
{
    for( auto& character : _string )
    {
        switch( character )
        {
            case '#':character = '?'; break; 
            case '.':character = '/'; break; 
            case '$':character = '<'; break; 
            case '^':character = '>'; break; 
            case '&':character = '+'; break; 
            case '@':character = '='; break; 
            case '%':character = ';'; break;
            case ' ':character = '_'; break; 
        }
    }
}

void CMMBEGui::OnWindowResize()
{
    mSlotContextMenu->resize( mTable->x()      , mTable->y()      , mTable->w()      , mTable->h()       );
    mDiskContextMenu->resize( mDiskContent->x(), mDiskContent->y(), mDiskContent->w(), mDiskContent->h() );
}