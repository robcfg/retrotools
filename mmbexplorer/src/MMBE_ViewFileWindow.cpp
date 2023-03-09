//////////////////////////////////////////////////////////////
// Credits:
//      This view file window class is a refactored and
//      upgraded version of the one originally submitted by 
//      Dmcoles (https://github.com/dmcoles)
//
//      Thank you for your contributions!
//////////////////////////////////////////////////////////////

#include <iomanip>
#include <sstream>

#include "MMBE_Callbacks.h"
#include "MMBE_ViewFileWindow.h"

using namespace std;

const char BBCBasicTokens[255][20] = {  "AND","DIV","EOR","MOD","OR","ERROR","LINE","OFF","STEP","SPC","TAB(","ELSE",
                                        "THEN","line","OPENIN","PTR","PAGE","TIME","LOMEM","HIMEM","ABS","ACS","ADVAL",
                                        "ASC","ASN","ATN","BGET","COS","COUNT","DEG","ERL","ERR","EVAL","EXP","EXT",
                                        "FALSE","FN","GET","INKEY","INSTR(","INT","LEN","LN","LOG","NOT","OPENUP",
                                        "OPENOUT","PI","POINT(","POS","RAD","RND","SGN","SIN","SQR","TAN","TO","TRUE",
                                        "USR","VAL","VPOS","CHR$","GET$","INKEY$","LEFT$(","MID$(","RIGHT$(","STR$",
                                        "STRING$(","EOF","SUM","WHILE","CASE","WHEN","OF","ENDCASE","OTHERWISE","ENDIF",
                                        "ENDWHILE","PTR","PAGE","TIME","LOMEM","HIMEM","SOUND","BPUT","CALL","CHAIN",
                                        "CLEAR","CLOSE","CLG","CLS","DATA","DEF","DIM","DRAW","END","ENDPROC","ENVELOPE",
                                        "FOR","GOSUB","GOTO","GCOL","IF","INPUT","LET","LOCAL","MODE","MOVE","NEXT","ON",
                                        "VDU","PLOT","PRINT","PROC","READ","REM","REPEAT","REPORT","RESTORE","RETURN",
                                        "RUN","STOP","COLOUR","TRACE","UNTIL","WIDTH","OSCLI" };

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

#define MMBEVFW_STYLE_TABLE_SIZE        sizeof(stable)/sizeof(stable[0])
#define MMBEVFW_MAX_FILENAME_LENGTH     9   // Directory (1), dot separator (1) and maximum DFS_FILENAME_LENGTH (7)
#define MMBEVFW_FILE_HEADER_FILLER_NUM  42  // Number of '-' to append at the end of the file separator header.
#define MMBEVFW_WINDOW_MARGIN           10
#define MMBEVFW_RADIO_BUTTON_WIDTH      80
#define MMBEVFW_RADIO_BUTTON_HEIGHT     25
#define MMBEVFW_RADIO_BUTTON_GAP        5
#define MMBEVFW_COLOR_TEXT              'A'
#define MMBEVFW_COLOR_ERROR             'B'
#define MMBEVFW_COLOR_BASIC_TOKEN       'C'
#define MMBEVFW_COLOR_STRING            'D'
#define MMBEVFW_COLOR_LINE_NUMBER       'E'
#define MMBEVFW_KEY_B                   98
#define MMBEVFW_KEY_H                   104
#define MMBEVFW_KEY_T                   116

CMMBEViewFileWindow::CMMBEViewFileWindow( int _w, int _h, const char* _label ) : 
    Fl_Window( _w, _h, _label ) 
{
    CreateControls();
}

CMMBEViewFileWindow::CMMBEViewFileWindow( int _x, int _y, int _w, int _h, const char* _label ) : 
    Fl_Window( _x, _y, _w, _h, _label )
{
    CreateControls();
}

CMMBEViewFileWindow::~CMMBEViewFileWindow()
{

}

int CMMBEViewFileWindow::handle(int _event)
{
	if( _event == FL_KEYUP )
	{
        int key = Fl::event_key();
        if( key == MMBEVFW_KEY_H )
        {
            ViewAsHex();
            mViewAsHexButton->value(1);
            mViewAsTextButton->value(0);
            mViewAsBasicButton->value(0);
            return 1;
        }
        else if( key == MMBEVFW_KEY_T )
        {
            ViewAsText();
            mViewAsHexButton->value(0);
            mViewAsTextButton->value(1);
            mViewAsBasicButton->value(0);
            return 1;
        }
        else if( key == MMBEVFW_KEY_B )
        {
            ViewAsBasic();
            mViewAsHexButton->value(0);
            mViewAsTextButton->value(0);
            mViewAsBasicButton->value(1);
            return 1;
        }
    }

    int ret = Fl_Window::handle( _event );

    return( ret );
}

void CMMBEViewFileWindow::show()
{
    switch( mViewMode )
    {
        case VM_HEX  :ViewAsHex();   break;
        case VM_TEXT :ViewAsText();  break;
        case VM_BASIC:ViewAsBasic(); break;
        default:      ViewAsHex();   break;
    }

    Fl_Window::show();
}

void CMMBEViewFileWindow::CreateControls()
{
    mTextBuffer = new Fl_Text_Buffer();
    mTextColor  = new Fl_Text_Buffer();

    box(FL_FLAT_BOX);
    color(FL_DARK2);
    begin();

    int x = MMBEVFW_WINDOW_MARGIN;
    int y = MMBEVFW_WINDOW_MARGIN;

    mViewAsLabel = new Fl_Box( FL_NO_BOX, x, y, 90, MMBEVFW_RADIO_BUTTON_HEIGHT, "View as:" );
    mViewAsLabel->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
    x += 90;
    
    mViewAsHexButton = new Fl_Radio_Light_Button( x, y, MMBEVFW_RADIO_BUTTON_WIDTH, MMBEVFW_RADIO_BUTTON_HEIGHT, "Hex (H)");
    mViewAsHexButton->callback( viewFileAsHex_cb, (void*)this );
    mViewAsHexButton->value(1);
    x += MMBEVFW_RADIO_BUTTON_WIDTH + MMBEVFW_RADIO_BUTTON_GAP;
    
    mViewAsTextButton = new Fl_Radio_Light_Button( x, y, MMBEVFW_RADIO_BUTTON_WIDTH, MMBEVFW_RADIO_BUTTON_HEIGHT, "Text (T)");
    mViewAsTextButton->callback( viewFileAsText_cb, (void*)this );
    x += MMBEVFW_RADIO_BUTTON_WIDTH + MMBEVFW_RADIO_BUTTON_GAP;
    
    mViewAsBasicButton = new Fl_Radio_Light_Button( x, y, MMBEVFW_RADIO_BUTTON_WIDTH, MMBEVFW_RADIO_BUTTON_HEIGHT, "Basic (B)");
    mViewAsBasicButton->callback( viewFileAsBasic_cb, (void*)this );
    x = MMBEVFW_WINDOW_MARGIN;
    y += 35;

    mTextDisplay = new Fl_Text_Display( x, y, w() - (MMBEVFW_WINDOW_MARGIN * 2), h() - y - MMBEVFW_WINDOW_MARGIN, nullptr );
    mTextDisplay->textfont(FL_COURIER);
    mTextDisplay->buffer( mTextBuffer );

    mTextDisplay->highlight_data( mTextColor, stable, MMBEVFW_STYLE_TABLE_SIZE, 'A', 0, 0 );

    end();
}

void CMMBEViewFileWindow::SetData( const DFSDisk& _disk, const std::vector<int>& _selectedFiles )
{
    mHexView.clear();
    mHexViewColors.clear();
    mTextView.clear();
    mTextViewColors.clear();
    mBasicView.clear();
    mBasicViewColors.clear();

    for( auto file : _selectedFiles )
    {
        std::string filename;
        filename += (char)_disk.files[file].directory;
        filename += ".";
        filename += _disk.files[file].name;
        if( filename.length() > MMBEVFW_MAX_FILENAME_LENGTH )
        {
            filename =filename.substr( 0, MMBEVFW_MAX_FILENAME_LENGTH );
        }

        std::string fileHeader;
        if( _selectedFiles.size() > 1 )
        {
            fileHeader = "-[";
            fileHeader += filename;
            fileHeader.append( MMBEVFW_MAX_FILENAME_LENGTH - filename.length(), ' ' );
            fileHeader += "]";
            fileHeader.append( MMBEVFW_FILE_HEADER_FILLER_NUM, '-' );
        }

        AddHexViewData  ( fileHeader, _disk.files[file].data );
        AddTextViewData ( fileHeader, _disk.files[file].data );
        AddBasicViewData( fileHeader, _disk.files[file].data );
    }

    mHexViewColors.append ( mHexView.length() , MMBEVFW_COLOR_TEXT );
    mTextViewColors.append( mTextView.length(), MMBEVFW_COLOR_TEXT );
}

void CMMBEViewFileWindow::ViewAsHex()
{
    mViewMode = VM_HEX;
    mTextBuffer->text( mHexView.c_str() );
    mTextColor->text( mHexViewColors.c_str() );
}

void CMMBEViewFileWindow::ViewAsText()
{
    mViewMode = VM_TEXT;
    mTextBuffer->text( mTextView.c_str() );
    mTextColor->text( mTextViewColors.c_str() );
}

void CMMBEViewFileWindow::ViewAsBasic()
{
    mViewMode = VM_BASIC;
    mTextBuffer->text( mBasicView.c_str() );
    mTextColor->text( mBasicViewColors.c_str() );
}

void CMMBEViewFileWindow::AddHexViewData( const std::string _fileHeader, const std::vector<unsigned char>& _fileData )
{
    stringstream strStream;
    if( !_fileHeader.empty() )
    {
        strStream << _fileHeader << endl;
    }

    size_t c = 0;
    size_t filesize = _fileData.size();

    while( c < _fileData.size() )
    {
        strStream << hex << std::uppercase << setfill('0') << setw(5);
        strStream << c << ": ";

        for( int x = 0; x < 16; ++x )
        {
            if( c < filesize )
            {
                strStream << setfill('0') << setw(2) << (size_t)_fileData[c] << " ";
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

        strStream << endl;
    }

    strStream << endl;

    mHexView += strStream.str();
}

void CMMBEViewFileWindow::AddTextViewData ( const std::string _fileHeader, const std::vector<unsigned char>& _fileData )
{
    stringstream strStream;
    if( !_fileHeader.empty() )
    {
        strStream << _fileHeader << endl;
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
            strStream << endl;
            charsPerLineCount = 0;
        }
        c++;

        if( charsPerLineCount && 0 == charsPerLineCount % 64 )
        {
            strStream << endl;
        }
    }

    strStream << endl;

    mTextView += strStream.str();
}

void CMMBEViewFileWindow::AddBasicViewData( const std::string _fileHeader, const std::vector<unsigned char>& _fileData )
{
    stringstream strStream;
    stringstream clrStream;
    std::string textColors;
    if( !_fileHeader.empty() )
    {
        strStream << _fileHeader << endl;
        textColors.append( _fileHeader.length(), MMBEVFW_COLOR_TEXT );
        textColors += "\n";
    }

    size_t c = 0;
    size_t filesize = _fileData.size();
    unsigned char work;
    string lineNumber;
    int number = 0;
    bool quote = false;

    if( 0 == filesize )
    {
        string strError = "File is empty.";
        strStream << strError << endl;
        textColors.append( strError.length(), MMBEVFW_COLOR_ERROR );
        textColors += "\n";

        strStream << endl;
        textColors += "\n";

        mBasicView += strStream.str();
        mBasicViewColors += textColors;

        return;
    }

    work = _fileData[c++];
    if (work != 0x0D)
    {
        string strError = "This is not a BBC Basic file.";
        strStream << strError << endl;
        textColors.append( strError.length(), MMBEVFW_COLOR_ERROR );
        textColors += "\n";
    }
    else
    {
        while( c < filesize )
        {
            char* s = new char[255];
            // first get the line number
            work = _fileData[c++];
            if (work == 0xFF)
            {
                // End of program
                break;
            }

            lineNumber = to_string( work * 256 + _fileData[c++] );
            strStream << lineNumber << " ";
            textColors.append( lineNumber.length()+1, MMBEVFW_COLOR_LINE_NUMBER );

            // First skip the length as we don't care!
            ++c;

            quote = 0;
            //Now the rest of the line
            do
            {
                work = _fileData[c++];
                if( work == 0x8D && !quote )
                {
                    // It's a line number
                    int num1 = _fileData[c++], num2 = _fileData[c++], num3 = _fileData[c++];
                    number = num2 - 0x40;
                    switch (num1)
                    {
                    case 0x44: number += 0x40; break;
                    case 0x54: break;
                    case 0x64: number += 0xC0; break;
                    case 0x74: number += 0x80; break;
                    }
                    number += (num3 - 0x40) * 256;
                    strStream << number;
                    textColors.append( to_string(number).length(), MMBEVFW_COLOR_LINE_NUMBER );
                }
                else if (work >= 0x80 && !quote)
                {
                    // It's a token!
                    strStream << BBCBasicTokens[work - 0x80];
                    textColors.append( strlen(BBCBasicTokens[work - 0x80]), MMBEVFW_COLOR_BASIC_TOKEN );
                }
                else if (work != 0x0D)
                {
                    // It's a character
                    strStream << work;
                    if (work == '\"')
                    {
                        textColors += MMBEVFW_COLOR_STRING;
                        quote = !quote;
                    }
                    else
                    {
                        textColors += (quote ? MMBEVFW_COLOR_STRING : MMBEVFW_COLOR_TEXT);
                    }
                }
                else
                {
                    // End of line
                    strStream << endl;
                    textColors += "\n";
                }
            }
            while (work != 0x0D);
        }
    }

    strStream << endl;
    textColors += "\n";

    mBasicView += strStream.str();
    mBasicViewColors += textColors;
}
