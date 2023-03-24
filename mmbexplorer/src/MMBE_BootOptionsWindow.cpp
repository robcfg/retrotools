#include "MMBE_BootOptionsWindow.h"
#include "MMBE_Callbacks.h"
#include "MMBE_Gui.h"

#define MMBEBOW_VERTICAL_SEPARATION 8
#define MMBEBOW_TEXT_SIZE           14
#define MMBEBOW_TEXT_HEIGHT         15
#define MMBEBOW_LABEL_WIDTH         100

CMMBE_BootOptionsWindow::CMMBE_BootOptionsWindow( int _w, int _h, const char* _label ) : 
    Fl_Window( _w, _h, _label ) 
{
    CreateControls();
}

CMMBE_BootOptionsWindow::CMMBE_BootOptionsWindow( int _x, int _y, int _w, int _h, const char* _label ) : 
    Fl_Window( _x, _y, _w, _h, _label )
{
    CreateControls();
}

CMMBE_BootOptionsWindow::~CMMBE_BootOptionsWindow()
{

}

void CMMBE_BootOptionsWindow::CreateControls()
{
    box(FL_FLAT_BOX);
    color(FL_DARK2);
    set_modal();
    
    begin();

    int y = MMBEBOW_VERTICAL_SEPARATION;

    mAboutLabel = new Fl_Box( 115, y, MMBEBOW_LABEL_WIDTH, MMBEBOW_TEXT_HEIGHT, "OnBoot Options" );
    mAboutLabel->align( FL_ALIGN_LEFT) ;
    mAboutLabel->labelcolor( FL_BLACK );
    mAboutLabel->labelsize( MMBEBOW_TEXT_SIZE );
    mAboutLabel->labelfont( FL_HELVETICA );
    y += MMBEBOW_TEXT_HEIGHT + MMBEBOW_VERTICAL_SEPARATION;
    
    for( size_t drive = 0; drive < 4; ++drive )
    {
        std::string driveStr = "Boot Drive ";
        driveStr += std::to_string(drive);
        
        mBootDiskSliders[drive] = new Fl_Value_Slider(100, y, 150, MMBEBOW_TEXT_HEIGHT, nullptr);
        mBootDiskSliders[drive]->copy_label( driveStr.c_str() );
        y += MMBEBOW_TEXT_HEIGHT + MMBEBOW_VERTICAL_SEPARATION;
        
        mBootDiskSliders[drive]->align(FL_ALIGN_LEFT);
        mBootDiskSliders[drive]->step(1);
        mBootDiskSliders[drive]->minimum(0);
        mBootDiskSliders[drive]->type(FL_HOR_NICE_SLIDER);
    }

    mOkButton = new Fl_Return_Button( 100, y, 80, MMBEBOW_TEXT_HEIGHT+4, "Ok" );
    mOkButton->align( FL_ALIGN_CENTER );

    mCancelButton = new Fl_Return_Button( 190, y, 80, MMBEBOW_TEXT_HEIGHT+4, "Cancel" );
    mCancelButton->align( FL_ALIGN_CENTER );
    y += MMBEBOW_TEXT_HEIGHT + MMBEBOW_VERTICAL_SEPARATION;

    end();
}

void CMMBE_BootOptionsWindow::show( CMMBFile* _mmb )
{
    if( nullptr == _mmb )
    {
        return;
    }

    mMMB = _mmb;

    for( size_t drive = 0; drive < 4; ++drive )
    {
        mBootDiskSliders[drive]->maximum(mMMB->GetNumberOfDisks()-1);
        mBootDiskSliders[drive]->value(mMMB->GetDriveBootDisk(drive));
    }

    mOkButton->callback( editBoot_ok_cb, (void*)mMMB );
    mCancelButton->callback( editBoot_cancel_cb, (void*)mMMB );

    Fl_Window::show();
}

size_t CMMBE_BootOptionsWindow::GetSliderValue( size_t _slider )
{
    if( _slider < 4 )
    {
        return mBootDiskSliders[_slider]->value();
    }

    return 0;
}