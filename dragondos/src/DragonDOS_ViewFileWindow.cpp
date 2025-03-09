////////////////////////////////////////////////////////////////////
//
// DragonDOS_ViewFileWindow.cpp - Implementation of
//                                CDragonDOSViewFileWindow, a class
//                                that manages a window to visualize
//                                the content of different files.
//
// Dragon graphics modes:
//
//      Mode    | Resolution | Pixel size | Colors | Memory
//      --------+------------+------------+--------+-------
//      PMODE 0 | 128x96     | 2x2        | 2      | 1536
//      --------+------------+------------+--------+-------
//      PMODE 1 | 128x96     | 2x2        | 4      | 3072
//      --------+------------+------------+--------+-------
//      PMODE 2 | 128x192    | 2x1        | 2      | 3072
//      --------+------------+------------+--------+-------
//      PMODE 3 | 128x192    | 2x1        | 4      | 6144
//      --------+------------+------------+--------+-------
//      PMODE 4 | 256x192    | 1x1        | 2      | 6144
//      --------+------------+------------+--------+-------
//
////////////////////////////////////////////////////////////////////

#include <iomanip>
#include <sstream>

#include <FL/Fl_Image.H>

#include "DragonDOS_BASIC.h"
#include "DragonDOS_UI_Callbacks.h"
#include "DragonDOS_ViewFileWindow.h"

#include "../logo/DragonDOS_Logo.xpm"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Style table
Fl_Text_Display::Style_Table_Entry stable[] = {
    // FONT COLOR      FONT FACE   FONT SIZE
    // --------------- ----------- --------------
    {  FL_BLACK,       FL_COURIER, 14 }, // A - Black
    {  FL_RED,         FL_COURIER, 14 }, // B - Red
    {  FL_DARK_YELLOW, FL_COURIER, 14 }, // C - Yellow
    {  FL_DARK_GREEN,  FL_COURIER, 14 }, // D - Green
    {  FL_BLUE,        FL_COURIER, 14 }  // E - Blue
};

#define DRAGONDOSVFW_STYLE_TABLE_SIZE        sizeof(stable)/sizeof(stable[0])
#define DRAGONDOSVFW_MAX_FILENAME_LENGTH     12  // Name (8), dot separator (1), extension (3)
#define DRAGONDOSVFW_FILE_HEADER_FILLER_NUM  42  // Number of '-' to append at the end of the file separator header.
#define DRAGONDOSVFW_WINDOW_MARGIN           10
#define DRAGONDOSVFW_RADIO_BUTTON_WIDTH      80
#define DRAGONDOSVFW_RADIO_BUTTON_HEIGHT     25
#define DRAGONDOSVFW_RADIO_BUTTON_GAP        5
#define DRAGONDOSVFW_COMBO_WIDTH             98  // To fit 4 under the 512pc wide image, with 3 margins of separation.
#define DRAGONDOSVFW_COLOR_TEXT              'A'
#define DRAGONDOSVFW_COLOR_ERROR             'B'
#define DRAGONDOSVFW_COLOR_BASIC_TOKEN       'C'
#define DRAGONDOSVFW_COLOR_STRING            'D'
#define DRAGONDOSVFW_COLOR_LINE_NUMBER       'E'
#define DRAGONDOSVFW_KEY_B                   98
#define DRAGONDOSVFW_KEY_H                   104
#define DRAGONDOSVFW_KEY_I                   105
#define DRAGONDOSVFW_KEY_T                   116
#define DRAGONDOSVFW_IMAGE_VIEW_WIDTH        512
#define DRAGONDOSVFW_IMAGE_VIEW_HEIGHT       384
#define DRAGONDOSVFW_IMAGE_VIEW_DEPTH        3
#define DRAGONDOSVFW_IMAGE_VIEW_SIZE         DRAGONDOSVFW_IMAGE_VIEW_WIDTH*DRAGONDOSVFW_IMAGE_VIEW_HEIGHT*DRAGONDOSVFW_IMAGE_VIEW_DEPTH
#define DRAGONDOSVFW_IMAGE_VIEW_STRIDE       DRAGONDOSVFW_IMAGE_VIEW_WIDTH*DRAGONDOSVFW_IMAGE_VIEW_DEPTH
#define DRAGONDOSVFW_PMODE0                  0
#define DRAGONDOSVFW_PMODE1                  1
#define DRAGONDOSVFW_PMODE2                  2
#define DRAGONDOSVFW_PMODE3                  3
#define DRAGONDOSVFW_PMODE4                  4

// Palettes
const unsigned char TwoColorPalettes[2][2][3] = { 0x4C, 0x56, 0x3C, 0xBE, 0xC8, 0xAC,
                                                  0x28, 0x58, 0x10, 0x2F, 0xD2, 0x00 };
//const unsigned char TwoColorPalettes[2][2][3] = { 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
//                                                  0x00, 0x00, 0x00, 0x00, 0xFF, 0x00 };

const unsigned char FourColorPalettes[2][4][3] = { 0xBE, 0xC8, 0xAC, 0x40, 0xAF, 0x71,
                                                   0xC7, 0x4E, 0xF0, 0xD4, 0x7F, 0x00,
                                                   0x2F, 0xD2, 0x00, 0xC0, 0xE5, 0x01,
                                                   0x4C, 0x3A, 0xC4, 0x9A, 0x31, 0x36 };

CDragonDOSViewFileWindow::CDragonDOSViewFileWindow( int _w, int _h, const char* _label ) : 
    Fl_Window( _w, _h, _label ) 
{
    mImageData = new unsigned char[DRAGONDOSVFW_IMAGE_VIEW_SIZE];
    CreateControls();
}

CDragonDOSViewFileWindow::CDragonDOSViewFileWindow( int _x, int _y, int _w, int _h, const char* _label ) : 
    Fl_Window( _x, _y, _w, _h, _label )
{
    mImageData = new unsigned char[DRAGONDOSVFW_IMAGE_VIEW_SIZE];
    CreateControls();
}

CDragonDOSViewFileWindow::~CDragonDOSViewFileWindow()
{
    if( nullptr != mImageData )
    {
        delete[] mImageData;
    }
}

int CDragonDOSViewFileWindow::handle(int _event)
{
	if( _event == FL_KEYUP )
	{
        int key = Fl::event_key();
        if( key == DRAGONDOSVFW_KEY_H )
        {
            ViewAsHex();
            mViewAsHexButton->value(1);
            mViewAsTextButton->value(0);
            mViewAsBasicButton->value(0);
            mViewAsImageButton->value(0);
            return 1;
        }
        else if( key == DRAGONDOSVFW_KEY_T )
        {
            ViewAsText();
            mViewAsHexButton->value(0);
            mViewAsTextButton->value(1);
            mViewAsBasicButton->value(0);
            mViewAsImageButton->value(0);
            return 1;
        }
        else if( key == DRAGONDOSVFW_KEY_B )
        {
            ViewAsBasic();
            mViewAsHexButton->value(0);
            mViewAsTextButton->value(0);
            mViewAsBasicButton->value(1);
            mViewAsImageButton->value(0);
            return 1;
        }
        else if( key == DRAGONDOSVFW_KEY_I )
        {
            ViewAsImage();
            mViewAsHexButton->value(0);
            mViewAsTextButton->value(0);
            mViewAsBasicButton->value(0);
            mViewAsImageButton->value(1);
            return 1;
        }
    }

    int ret = Fl_Window::handle( _event );

    return( ret );
}

void CDragonDOSViewFileWindow::show()
{
    switch( mViewMode )
    {
        case VM_HEX  :ViewAsHex();   break;
        case VM_TEXT :ViewAsText();  break;
        case VM_BASIC:ViewAsBasic(); break;
        case VM_IMAGE:ViewAsImage(); break;
        default:      ViewAsHex();   break;
    }

    Fl_Window::show();
}

void CDragonDOSViewFileWindow::CreateControls()
{
    mTextBuffer = new Fl_Text_Buffer();
    mTextColor  = new Fl_Text_Buffer();

    box(FL_FLAT_BOX);
    color(FL_DARK2);
    begin();

    int x = DRAGONDOSVFW_WINDOW_MARGIN;
    int y = DRAGONDOSVFW_WINDOW_MARGIN;

    mViewAsLabel = new Fl_Box( FL_NO_BOX, x, y, 90, DRAGONDOSVFW_RADIO_BUTTON_HEIGHT, "View as:" );
    mViewAsLabel->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
    x += 90;
    
    mViewAsHexButton = new Fl_Radio_Light_Button( x, y, DRAGONDOSVFW_RADIO_BUTTON_WIDTH, DRAGONDOSVFW_RADIO_BUTTON_HEIGHT, "Hex (H)");
    mViewAsHexButton->callback( viewFileAsHex_cb, (void*)this );
    mViewAsHexButton->value(1);
    x += DRAGONDOSVFW_RADIO_BUTTON_WIDTH + DRAGONDOSVFW_RADIO_BUTTON_GAP;
    
    mViewAsTextButton = new Fl_Radio_Light_Button( x, y, DRAGONDOSVFW_RADIO_BUTTON_WIDTH, DRAGONDOSVFW_RADIO_BUTTON_HEIGHT, "Text (T)");
    mViewAsTextButton->callback( viewFileAsText_cb, (void*)this );
    x += DRAGONDOSVFW_RADIO_BUTTON_WIDTH + DRAGONDOSVFW_RADIO_BUTTON_GAP;
    
    mViewAsBasicButton = new Fl_Radio_Light_Button( x, y, DRAGONDOSVFW_RADIO_BUTTON_WIDTH, DRAGONDOSVFW_RADIO_BUTTON_HEIGHT, "Basic (B)");
    mViewAsBasicButton->callback( viewFileAsBasic_cb, (void*)this );
    x += DRAGONDOSVFW_RADIO_BUTTON_WIDTH + DRAGONDOSVFW_RADIO_BUTTON_GAP;
    
    mViewAsImageButton = new Fl_Radio_Light_Button( x, y, DRAGONDOSVFW_RADIO_BUTTON_WIDTH, DRAGONDOSVFW_RADIO_BUTTON_HEIGHT, "Image (I)");
    mViewAsImageButton->callback( viewFileAsImage_cb, (void*)this );
    x = DRAGONDOSVFW_WINDOW_MARGIN;
    y += 35;

    mTextDisplay = new Fl_Text_Display( x, y, w() - (DRAGONDOSVFW_WINDOW_MARGIN * 2), h() - y - DRAGONDOSVFW_WINDOW_MARGIN, nullptr );
    mTextDisplay->textfont(FL_COURIER);
    mTextDisplay->buffer( mTextBuffer );

    mTextDisplay->highlight_data( mTextColor, stable, DRAGONDOSVFW_STYLE_TABLE_SIZE, 'A', 0, 0 );

    // Image view controls
    x = (w() - DRAGONDOSVFW_IMAGE_VIEW_WIDTH)/2;
    mImage = new Fl_Box( FL_BORDER_BOX, x-1, y-1, DRAGONDOSVFW_IMAGE_VIEW_WIDTH+2, DRAGONDOSVFW_IMAGE_VIEW_HEIGHT+2, nullptr );
    mImage->align( FL_ALIGN_INSIDE );
    mImage->hide();
    y += DRAGONDOSVFW_IMAGE_VIEW_HEIGHT + 25;

    mVideoMode = new Fl_Choice( x, y, DRAGONDOSVFW_COMBO_WIDTH, DRAGONDOSVFW_RADIO_BUTTON_HEIGHT, nullptr );
    mVideoMode->add("PMODE 0");
    mVideoMode->add("PMODE 1");
    mVideoMode->add("PMODE 2");
    mVideoMode->add("PMODE 3");
    mVideoMode->add("PMODE 4");
    mVideoMode->value(4);
    mVideoMode->align( FL_ALIGN_TOP_LEFT );
    mVideoMode->copy_label("Video mode");
    mVideoMode->callback( viewFileModeChanged_cb, (void*)this );
    x += DRAGONDOSVFW_COMBO_WIDTH + DRAGONDOSVFW_WINDOW_MARGIN;

    mVideoPalette = new Fl_Choice( x, y, DRAGONDOSVFW_COMBO_WIDTH, DRAGONDOSVFW_RADIO_BUTTON_HEIGHT, nullptr );
    mVideoPalette->add("White");
    mVideoPalette->add("Green");
    mVideoPalette->value(1);
    mVideoPalette->align( FL_ALIGN_TOP_LEFT );
    mVideoPalette->copy_label("Palette");
    mVideoPalette->callback( viewFilePaletteChanged_cb, (void*)this );
    x += DRAGONDOSVFW_COMBO_WIDTH + DRAGONDOSVFW_WINDOW_MARGIN;

    mExportAsPNG = new Fl_Button( x, y, 100, DRAGONDOSVFW_RADIO_BUTTON_HEIGHT, "Export as PNG" );
    mExportAsPNG->callback( viewFileExportPNG_cb, (void*)this );
    y += 23;

    end();
}

void CDragonDOSViewFileWindow::SetData( const CDragonDOS_FS* _fs, const std::vector<int>& _selectedFiles )
{
    mHexView.clear();
    mHexViewColors.clear();
    mTextView.clear();
    mTextViewColors.clear();
    mBasicView.clear();
    mBasicViewColors.clear();

    for( auto file : _selectedFiles )
    {
        std::string fileName = _fs->GetFileName( file );
        if( fileName.length() > DRAGONDOSVFW_MAX_FILENAME_LENGTH )
        {
            fileName =fileName.substr( 0, DRAGONDOSVFW_MAX_FILENAME_LENGTH );
        }

        std::string fileHeader;
        if( _selectedFiles.size() > 1 )
        {
            fileHeader = "-[";
            fileHeader += fileName;
            fileHeader.append( DRAGONDOSVFW_MAX_FILENAME_LENGTH - fileName.length(), ' ' );
            fileHeader += "]";
            fileHeader.append( DRAGONDOSVFW_FILE_HEADER_FILLER_NUM, '-' );
        }

        std::vector<unsigned char> fileData;
        _fs->ExtractFile( fileName, fileData, false );
        AddHexViewData  ( fileHeader, fileData );
        AddTextViewData ( fileHeader, fileData );
        AddBasicViewData( fileHeader, fileData );
    }

    mHexViewColors.append ( mHexView.length() , DRAGONDOSVFW_COLOR_TEXT );
    mTextViewColors.append( mTextView.length(), DRAGONDOSVFW_COLOR_TEXT );

    // Create image data from 1st file selected
    if( !_selectedFiles.empty() )
    {
        std::string fileName = _fs->GetFileName( _selectedFiles[0] );
        mImageFileData.clear();
        _fs->ExtractFile( fileName, mImageFileData, false );

        DecodeImage();
    }
}

void CDragonDOSViewFileWindow::ViewAsHex()
{
    mViewMode = VM_HEX;
    ShowImageControls( false );
    mTextBuffer->text( mHexView.c_str() );
    mTextColor->text( mHexViewColors.c_str() );
}

void CDragonDOSViewFileWindow::ViewAsText()
{
    mViewMode = VM_TEXT;
    ShowImageControls( false );
    mTextBuffer->text( mTextView.c_str() );
    mTextColor->text( mTextViewColors.c_str() );
}

void CDragonDOSViewFileWindow::ViewAsBasic()
{
    mViewMode = VM_BASIC;
    ShowImageControls( false );
    mTextBuffer->text( mBasicView.c_str() );
    mTextColor->text( mBasicViewColors.c_str() );
}

void CDragonDOSViewFileWindow::ViewAsImage()
{
    mViewMode = VM_IMAGE;
    ShowImageControls( true );
}

void CDragonDOSViewFileWindow::AddHexViewData( const std::string _fileHeader, const std::vector<unsigned char>& _fileData )
{
    std::stringstream strStream;
    if( !_fileHeader.empty() )
    {
        strStream << _fileHeader << std::endl;
    }

    size_t c = 0;
    size_t filesize = _fileData.size();

    while( c < _fileData.size() )
    {
        strStream << std::hex << std::uppercase << std::setfill('0') << std::setw(5);
        strStream << c << ": ";

        for( int x = 0; x < 16; ++x )
        {
            if( c < filesize )
            {
                strStream << std::setfill('0') << std::setw(2) << (size_t)_fileData[c] << " ";
            }
            else {
                strStream << "   ";
            }
            ++c;
        }
        c -= 16;

        for( int x = 0; x < 16; ++x )
        {
            if( (c < filesize) && (_fileData[c] > 31) && (_fileData[c] < 128) )
            {
                strStream << (char)_fileData[c];
            }
            else if( c < filesize )
            {
                strStream << ".";
            }
            ++c;
        }

        strStream << std::endl;
    }

    strStream << std::endl;

    mHexView += strStream.str();
}

void CDragonDOSViewFileWindow::AddTextViewData ( const std::string _fileHeader, const std::vector<unsigned char>& _fileData )
{
    if( _fileData.empty() )
    {
        mTextView = "";
        return;
    }
    std::stringstream strStream;
    if( !_fileHeader.empty() )
    {
        strStream << _fileHeader << std::endl;
    }

    size_t c = 0;
    size_t charsPerLineCount = 0;
    size_t filesize = _fileData.size();

    while( c < filesize )
    {
        if( (_fileData[c] > 31) && (_fileData[c] < 128) )
        {
            strStream << (char)_fileData[c];
            ++charsPerLineCount;
        }
        else if( _fileData[c] == '\n' )
        {
            strStream << std::endl;
            charsPerLineCount = 0;
        }
        c++;

        if( charsPerLineCount && 0 == charsPerLineCount % 64 )
        {
            strStream << std::endl;
        }
    }

    if( _fileData.back() != '\n' )
    {
        strStream << std::endl;
    }

    strStream << std::endl;

    mTextView += strStream.str();
}

void CDragonDOSViewFileWindow::AddBasicViewData( const std::string _fileHeader, const std::vector<unsigned char>& _fileData )
{
    if( _fileData.empty() )
    {
        mBasicView = "";
        mBasicViewColors = "";
        return;
    }

    std::stringstream strStream;
    std::stringstream clrStream;
    std::string textColors;

    if( !_fileHeader.empty() )
    {
        strStream << _fileHeader << std::endl;
        textColors.append( _fileHeader.length(), DRAGONDOSVFW_COLOR_TEXT );
        textColors += "\n";
    }

    unsigned short int programStart = DRAGONDOS_BASIC_PROGRAM_START;
    DragonDOS_BASIC::Decode( _fileData, strStream, textColors, programStart, false, false );

    strStream << std::endl;
    textColors += "\n";

    mBasicView += strStream.str();
    mBasicViewColors += textColors;
}

void CDragonDOSViewFileWindow::ShowImageControls( bool _status )
{
    if( _status )
    {
        mTextDisplay->hide();
        mImage->show();
        mVideoMode->show();
        mVideoPalette->show();
        mExportAsPNG->show();
    }
    else
    {
        mTextDisplay->show();
        mImage->hide();
        mVideoMode->hide();
        mVideoPalette->hide();
        mExportAsPNG->hide();
    }
}

void CDragonDOSViewFileWindow::Decode_PMODE0_Image( const std::vector<unsigned char>& _src )
{
    size_t lineBytes = 16; // PMODE0 line size in bytes.
    size_t rgbBytes = 512 * 3;
    size_t rgbBytes2 = rgbBytes * 2;
    size_t rgbBytes3 = rgbBytes * 3;
    size_t linesNum = _src.size() / lineBytes;
    if( 0 == linesNum )
    {
        lineBytes = _src.size();
        linesNum = 1;
    }
    if( linesNum >= 192 )
    {
        linesNum = 192;
    }

    size_t dataPtr = 0;
    size_t imagePtr = 0;
    int paletteIdx = mVideoPalette->value();
    int palettePixelIdx = 0;
    unsigned char pixelR = 0;
    unsigned char pixelG = 0;
    unsigned char pixelB = 0;
    unsigned char pixelMask = 0b11000000;

    for( size_t y = 0; y < linesNum; ++y )
    {
        for( size_t x = 0; x < lineBytes; ++x, ++dataPtr )
        {
            for( unsigned char bit = 0; bit < 8; ++bit )
            {
                palettePixelIdx = (_src[dataPtr] & (1 << (7 - bit)) ) ? 1 : 0;
                pixelR = TwoColorPalettes[paletteIdx][palettePixelIdx][0];
                pixelG = TwoColorPalettes[paletteIdx][palettePixelIdx][1];
                pixelB = TwoColorPalettes[paletteIdx][palettePixelIdx][2];

                for( size_t dstPixel = 0; dstPixel < 12; dstPixel += 3 )
                {
                    mImageData[imagePtr+dstPixel  ] = pixelR;
                    mImageData[imagePtr+dstPixel+1] = pixelG;
                    mImageData[imagePtr+dstPixel+2] = pixelB;

                    mImageData[imagePtr+dstPixel  +rgbBytes] = pixelR;
                    mImageData[imagePtr+dstPixel+1+rgbBytes] = pixelG;
                    mImageData[imagePtr+dstPixel+2+rgbBytes] = pixelB;

                    mImageData[imagePtr+dstPixel  +rgbBytes2] = pixelR;
                    mImageData[imagePtr+dstPixel+1+rgbBytes2] = pixelG;
                    mImageData[imagePtr+dstPixel+2+rgbBytes2] = pixelB;

                    mImageData[imagePtr+dstPixel  +rgbBytes3] = pixelR;
                    mImageData[imagePtr+dstPixel+1+rgbBytes3] = pixelG;
                    mImageData[imagePtr+dstPixel+2+rgbBytes3] = pixelB;
                }

                imagePtr += 12;
            }
        }
        imagePtr += rgbBytes3;
        if( imagePtr >= DRAGONDOSVFW_IMAGE_VIEW_SIZE )
        {
            return;
        }
    }
}

void CDragonDOSViewFileWindow::Decode_PMODE1_Image( const std::vector<unsigned char>& _src )
{
    size_t lineBytes = 32; // PMODE1 line size in bytes.
    size_t rgbBytes = 512 * 3;
    size_t rgbBytes2 = rgbBytes * 2;
    size_t rgbBytes3 = rgbBytes * 3;
    size_t linesNum = _src.size() / lineBytes;
    if( 0 == linesNum )
    {
        lineBytes = _src.size();
        linesNum = 1;
    }
    if( linesNum >= 192 )
    {
        linesNum = 192;
    }

    size_t dataPtr = 0;
    size_t imagePtr = 0;
    int paletteIdx = mVideoPalette->value();
    int palettePixelIdx = 0;
    unsigned char pixelR = 0;
    unsigned char pixelG = 0;
    unsigned char pixelB = 0;
    unsigned char pixelMask = 0b11000000;

    for( size_t y = 0; y < linesNum; ++y )
    {
        for( size_t x = 0; x < lineBytes; ++x, ++dataPtr )
        {
            pixelMask = 0b11000000;
            for( int pixel = 3; pixel >= 0; --pixel )
            {
                palettePixelIdx = ((_src[dataPtr] & pixelMask) >> (pixel * 2));
                pixelR = FourColorPalettes[paletteIdx][palettePixelIdx][0];
                pixelG = FourColorPalettes[paletteIdx][palettePixelIdx][1];
                pixelB = FourColorPalettes[paletteIdx][palettePixelIdx][2];

                for( size_t dstPixel = 0; dstPixel < 12; dstPixel += 3 )
                {
                    mImageData[imagePtr+dstPixel  ] = pixelR;
                    mImageData[imagePtr+dstPixel+1] = pixelG;
                    mImageData[imagePtr+dstPixel+2] = pixelB;

                    mImageData[imagePtr+dstPixel  +rgbBytes] = pixelR;
                    mImageData[imagePtr+dstPixel+1+rgbBytes] = pixelG;
                    mImageData[imagePtr+dstPixel+2+rgbBytes] = pixelB;

                    mImageData[imagePtr+dstPixel  +rgbBytes2] = pixelR;
                    mImageData[imagePtr+dstPixel+1+rgbBytes2] = pixelG;
                    mImageData[imagePtr+dstPixel+2+rgbBytes2] = pixelB;

                    mImageData[imagePtr+dstPixel  +rgbBytes3] = pixelR;
                    mImageData[imagePtr+dstPixel+1+rgbBytes3] = pixelG;
                    mImageData[imagePtr+dstPixel+2+rgbBytes3] = pixelB;
                }

                imagePtr += 12;
                pixelMask >>= 2;
            }
        }
        imagePtr += rgbBytes3;
        if( imagePtr >= DRAGONDOSVFW_IMAGE_VIEW_SIZE )
        {
            return;
        }
    }
}

void CDragonDOSViewFileWindow::Decode_PMODE2_Image( const std::vector<unsigned char>& _src )
{
    size_t lineBytes = 16; // PMODE2 line size in bytes.
    size_t rgbBytes = 512 * 3;
    size_t linesNum = _src.size() / lineBytes;
    if( 0 == linesNum )
    {
        lineBytes = _src.size();
        linesNum = 1;
    }
    if( linesNum >= 192 )
    {
        linesNum = 192;
    }

    size_t dataPtr = 0;
    size_t imagePtr = 0;
    int paletteIdx = mVideoPalette->value();
    int palettePixelIdx = 0;
    unsigned char pixelR = 0;
    unsigned char pixelG = 0;
    unsigned char pixelB = 0;
    unsigned char pixelMask = 0b11000000;

    for( size_t y = 0; y < linesNum; ++y )
    {
        for( size_t x = 0; x < lineBytes; ++x, ++dataPtr )
        {
            for( unsigned char bit = 0; bit < 8; ++bit )
            {
                palettePixelIdx = (_src[dataPtr] & (1 << (7 - bit)) ) ? 1 : 0;
                pixelR = TwoColorPalettes[paletteIdx][palettePixelIdx][0];
                pixelG = TwoColorPalettes[paletteIdx][palettePixelIdx][1];
                pixelB = TwoColorPalettes[paletteIdx][palettePixelIdx][2];

                for( size_t dstPixel = 0; dstPixel < 12; dstPixel += 3 )
                {
                    mImageData[imagePtr+dstPixel  ] = pixelR;
                    mImageData[imagePtr+dstPixel+1] = pixelG;
                    mImageData[imagePtr+dstPixel+2] = pixelB;

                    mImageData[imagePtr+dstPixel  +rgbBytes] = pixelR;
                    mImageData[imagePtr+dstPixel+1+rgbBytes] = pixelG;
                    mImageData[imagePtr+dstPixel+2+rgbBytes] = pixelB;
                }

                imagePtr += 12;
            }
        }
        imagePtr += rgbBytes;
        if( imagePtr >= DRAGONDOSVFW_IMAGE_VIEW_SIZE )
        {
            return;
        }
    }
}

void CDragonDOSViewFileWindow::Decode_PMODE3_Image( const std::vector<unsigned char>& _src )
{
    size_t lineBytes = 32; // PMODE3 line size in bytes.
    size_t rgbBytes = 512 * 3;
    size_t linesNum = _src.size() / lineBytes;
    if( 0 == linesNum )
    {
        lineBytes = _src.size();
        linesNum = 1;
    }
    if( linesNum >= 192 )
    {
        linesNum = 192;
    }

    size_t dataPtr = 0;
    size_t imagePtr = 0;
    int paletteIdx = mVideoPalette->value();
    int palettePixelIdx = 0;
    unsigned char pixelR = 0;
    unsigned char pixelG = 0;
    unsigned char pixelB = 0;
    unsigned char pixelMask = 0b11000000;

    for( size_t y = 0; y < linesNum; ++y )
    {
        for( size_t x = 0; x < lineBytes; ++x, ++dataPtr )
        {
            pixelMask = 0b11000000;
            for( int pixel = 3; pixel >= 0; --pixel )
            {
                palettePixelIdx = ((_src[dataPtr] & pixelMask) >> (pixel * 2));
                pixelR = FourColorPalettes[paletteIdx][palettePixelIdx][0];
                pixelG = FourColorPalettes[paletteIdx][palettePixelIdx][1];
                pixelB = FourColorPalettes[paletteIdx][palettePixelIdx][2];

                for( size_t dstPixel = 0; dstPixel < 12; dstPixel += 3 )
                {
                    mImageData[imagePtr+dstPixel  ] = pixelR;
                    mImageData[imagePtr+dstPixel+1] = pixelG;
                    mImageData[imagePtr+dstPixel+2] = pixelB;

                    mImageData[imagePtr+dstPixel  +rgbBytes] = pixelR;
                    mImageData[imagePtr+dstPixel+1+rgbBytes] = pixelG;
                    mImageData[imagePtr+dstPixel+2+rgbBytes] = pixelB;
                }

                imagePtr += 12;
                pixelMask >>= 2;
            }
        }
        imagePtr += rgbBytes;
        if( imagePtr >= DRAGONDOSVFW_IMAGE_VIEW_SIZE )
        {
            return;
        }
    }
}

void CDragonDOSViewFileWindow::Decode_PMODE4_Image( const std::vector<unsigned char>& _src )
{
    size_t lineBytes = 32; // PMODE4 line size in bytes.
    size_t rgbBytes = 512 * 3;
    size_t linesNum = _src.size() / lineBytes;
    if( 0 == linesNum )
    {
        lineBytes = _src.size();
        linesNum = 1;
    }
    if( linesNum >= 192 )
    {
        linesNum = 192;
    }

    size_t dataPtr = 0;
    size_t imagePtr = 0;
    int paletteIdx = mVideoPalette->value();
    int palettePixelIdx = 0;
    unsigned char pixelR = 0;
    unsigned char pixelG = 0;
    unsigned char pixelB = 0;

    for( size_t y = 0; y < linesNum; ++y )
    {
        for( size_t x = 0; x < lineBytes; ++x, ++dataPtr )
        {
            for( unsigned char bit = 0; bit < 8; ++bit )
            {
                palettePixelIdx = (_src[dataPtr] & (1 << (7 - bit)) ) ? 1 : 0;
                pixelR = TwoColorPalettes[paletteIdx][palettePixelIdx][0];
                pixelG = TwoColorPalettes[paletteIdx][palettePixelIdx][1];
                pixelB = TwoColorPalettes[paletteIdx][palettePixelIdx][2];

                for( size_t dstPixel = 0; dstPixel < 6; dstPixel += 3 )
                {
                    mImageData[imagePtr+dstPixel  ] = pixelR;
                    mImageData[imagePtr+dstPixel+1] = pixelG;
                    mImageData[imagePtr+dstPixel+2] = pixelB;

                    mImageData[imagePtr+dstPixel  +rgbBytes] = pixelR;
                    mImageData[imagePtr+dstPixel+1+rgbBytes] = pixelG;
                    mImageData[imagePtr+dstPixel+2+rgbBytes] = pixelB;
                }

                imagePtr += 6;
            }
        }
        imagePtr += rgbBytes;
        if( imagePtr >= DRAGONDOSVFW_IMAGE_VIEW_SIZE )
        {
            return;
        }
    }
}

void CDragonDOSViewFileWindow::ClearImage()
{
    memset( mImageData, 0, DRAGONDOSVFW_IMAGE_VIEW_SIZE );
}

void CDragonDOSViewFileWindow::DecodeImage()
{
    ClearImage();
    if( !mImageFileData.empty())
    {
        switch( mVideoMode->value() )
        {
            case DRAGONDOSVFW_PMODE0: Decode_PMODE0_Image(mImageFileData); break;
            case DRAGONDOSVFW_PMODE1: Decode_PMODE1_Image(mImageFileData); break;
            case DRAGONDOSVFW_PMODE2: Decode_PMODE2_Image(mImageFileData); break;
            case DRAGONDOSVFW_PMODE3: Decode_PMODE3_Image(mImageFileData); break;
            default:                  Decode_PMODE4_Image(mImageFileData); break;
        }
    }

    mImage->image( new Fl_RGB_Image( mImageData, DRAGONDOSVFW_IMAGE_VIEW_WIDTH, DRAGONDOSVFW_IMAGE_VIEW_HEIGHT ) );
    mImage->redraw();
}

void CDragonDOSViewFileWindow::ExportImage()
{
    std::string fileName;

    if( !ChooseFilename( fileName, true, false ) )
    {
        return;
    }

    stbi_write_png( fileName.c_str(), 
                    DRAGONDOSVFW_IMAGE_VIEW_WIDTH, 
                    DRAGONDOSVFW_IMAGE_VIEW_HEIGHT, 
                    DRAGONDOSVFW_IMAGE_VIEW_DEPTH, 
                    (const void *)mImageData, 
                    DRAGONDOSVFW_IMAGE_VIEW_STRIDE );
}