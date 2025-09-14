
#include "DragonDOS_NewDiskWindow.h"
#include "DragonDOS_UI_Callbacks.h"

CDragonDOSNewDiskWindow::CDragonDOSNewDiskWindow( int _w, int _h, const char* _label ) : 
	Fl_Window( _w, _h, _label ) 
{
	CreateControls();
}

CDragonDOSNewDiskWindow::CDragonDOSNewDiskWindow( int _x, int _y, int _w, int _h, const char* _label ) : 
	Fl_Window( _x, _y, _w, _h, _label )
{
	CreateControls();
}

CDragonDOSNewDiskWindow::~CDragonDOSNewDiskWindow()
{
}

void CDragonDOSNewDiskWindow::SetData( SDRAGONDOS_Context* _context )
{
	mContext = _context;

	if( mContext != nullptr )
	{
		// Get disk image formats
		for( size_t format = 0; format < mContext->diskImageFactory->Size(); ++format )
		{
			mImageType->add( mContext->diskImageFactory->GetDiskImage(format)->GetFileSpec().c_str() );
		}

		mImageType->value(0);
		mOkButton->callback( newDiskWindowOK_cb, (void*)mContext );
		mCancelButton->callback( newDiskWindowCancel_cb, (void*)mContext );
	}
}

void CDragonDOSNewDiskWindow::CreateControls()
{
	mDiskGeometry 	= new Fl_Choice( 100, 10 , 200, 25, "Disk geometry:" );
	mImageType 		= new Fl_Choice( 100, 45 , 200, 25, "Image type   :" );
	mOkButton		= new Fl_Button( 100, 90 , 95 , 25, "Ok" 			 );
	mCancelButton	= new Fl_Button( 205, 90 , 95 , 25, "Cancel" 		 );

	mDiskGeometry->align( FL_ALIGN_LEFT );
	mImageType->align( FL_ALIGN_LEFT );

	// Disk geometries
	mDiskGeometry->add( "1 side , 40 tracks, 180KB" );
	mDiskGeometry->add( "1 side , 80 tracks, 360KB" );
	mDiskGeometry->add( "2 sides, 40 tracks, 360KB" );
	mDiskGeometry->add( "2 sides, 80 tracks, 720KB" );
	mDiskGeometry->value(0);
}