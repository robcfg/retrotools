//////////////////////////////////////////////////////////////
// Credits:
//      This view file window class is a refactored and
//      upgraded version of the one originally submitted by 
//      Dmcoles (https://github.com/dmcoles)
//
//      Thank you for your contributions!
//////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Radio_Light_Button.H>
#include <FL/Fl_Window.H>

#include "MMBFile.h"
#include "AcornDFS.h"

enum EMMBEVFW_ViewMode
{
    VM_HEX = 0,
    VM_TEXT,
    VM_BASIC,
    VM_NUM
};

//******************************************
//* CMMBEViewFileWindow class
//******************************************
class CMMBEViewFileWindow : public Fl_Window
{
public:
	CMMBEViewFileWindow( int _w, int _h, const char* _label = nullptr );
	CMMBEViewFileWindow( int _x, int _y, int _w, int _h, const char* _label = nullptr );
	virtual ~CMMBEViewFileWindow();

	virtual int handle(int _event);
    virtual void show();

    void SetData( const DFSDisk& _disk, const std::vector<int>& _selectedFiles );

    void ViewAsHex();
    void ViewAsText();
    void ViewAsBasic();

private:
    void CreateControls  ();
    void AddHexViewData  ( const std::string _fileHeader, const std::vector<unsigned char>& _fileData );
    void AddTextViewData ( const std::string _fileHeader, const std::vector<unsigned char>& _fileData );
    void AddBasicViewData( const std::string _fileHeader, const std::vector<unsigned char>& _fileData );

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

    Fl_Box* mViewAsLabel = nullptr;

    EMMBEVFW_ViewMode mViewMode = VM_HEX;
};
