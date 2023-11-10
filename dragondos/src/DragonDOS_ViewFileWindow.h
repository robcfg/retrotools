////////////////////////////////////////////////////////////////////
//
// DragonDOS_ViewFileWindow.h - Header file for 
//                              CDragonDOSViewFileWindow, a class
//                              that manages a window to visualize
//                              the content of different files.
//
////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Radio_Light_Button.H>
#include <FL/Fl_Window.H>

#include "FileSystemInterface.h"

enum EDRAGONDOSVFW_ViewMode
{
    VM_HEX = 0,
    VM_TEXT,
    VM_BASIC,
    VM_IMAGE,
    VM_NUM
};

//******************************************
//* CDragonDOSViewFileWindow class
//******************************************
class CDragonDOSViewFileWindow : public Fl_Window
{
public:
	CDragonDOSViewFileWindow( int _w, int _h, const char* _label = nullptr );
	CDragonDOSViewFileWindow( int _x, int _y, int _w, int _h, const char* _label = nullptr );
	virtual ~CDragonDOSViewFileWindow();

	virtual int handle(int _event);
    virtual void show();

    void SetData( const IFilesystemInterface* _fs, const std::vector<int>& _selectedFiles );

    void ViewAsHex();
    void ViewAsText();
    void ViewAsBasic();
    void ViewAsImage();

    void DecodeImage();
    void ExportImage();

private:
    void CreateControls  ();
    void AddHexViewData  ( const std::string _fileHeader, const std::vector<unsigned char>& _fileData );
    void AddTextViewData ( const std::string _fileHeader, const std::vector<unsigned char>& _fileData );
    void AddBasicViewData( const std::string _fileHeader, const std::vector<unsigned char>& _fileData );

    void Decode_PMODE0_Image( const std::vector<unsigned char>& _src );
    void Decode_PMODE1_Image( const std::vector<unsigned char>& _src );
    void Decode_PMODE2_Image( const std::vector<unsigned char>& _src );
    void Decode_PMODE3_Image( const std::vector<unsigned char>& _src );
    void Decode_PMODE4_Image( const std::vector<unsigned char>& _src );
    void ClearImage();

    void ShowImageControls( bool _status );

    std::string mHexView;
    std::string mTextView;
    std::string mBasicView;
    std::string mHexViewColors;
    std::string mTextViewColors;
    std::string mBasicViewColors;

    Fl_Text_Display* mTextDisplay = nullptr;
    Fl_Text_Buffer*  mTextBuffer = nullptr;
    Fl_Text_Buffer*  mTextColor = nullptr;

    Fl_Radio_Light_Button* mViewAsHexButton = nullptr;
    Fl_Radio_Light_Button* mViewAsTextButton = nullptr;
    Fl_Radio_Light_Button* mViewAsBasicButton = nullptr;
    Fl_Radio_Light_Button* mViewAsImageButton = nullptr;

    Fl_Box* mViewAsLabel = nullptr;

    Fl_Box* mImage;
    unsigned char * mImageData = nullptr;
    vector<unsigned char> mImageFileData;
    Fl_Choice* mVideoMode = nullptr;
    Fl_Choice* mVideoPalette = nullptr;

    Fl_Button* mExportAsPNG = nullptr;

    EDRAGONDOSVFW_ViewMode mViewMode = VM_HEX;
};
