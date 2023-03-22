#pragma once

#include <string>
#include <FL/Fl_Box.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Window.H>

#include "MMBFile.h"

//******************************************
//* CMMBE_BootOptionsWindow class
//******************************************
class CMMBE_BootOptionsWindow : public Fl_Window
{
public:
	CMMBE_BootOptionsWindow( int _w, int _h, const char* _label = nullptr );
	CMMBE_BootOptionsWindow( int _x, int _y, int _w, int _h, const char* _label = nullptr );
	virtual ~CMMBE_BootOptionsWindow();

    virtual void show( CMMBFile* _mmb );

    size_t GetSliderValue( size_t _slider );
    
private:
    void CreateControls  ();

    Fl_Box* mAboutLabel = nullptr;
    Fl_Return_Button* mOkButton = nullptr;
    Fl_Return_Button* mCancelButton = nullptr;
    Fl_Value_Slider* mBootDiskSliders[4] = { nullptr, nullptr, nullptr, nullptr };

    CMMBFile* mMMB = nullptr;
};
