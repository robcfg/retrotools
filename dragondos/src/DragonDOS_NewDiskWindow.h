////////////////////////////////////////////////////////////////////
//
// DragonDOS_NewDiskWindow.h - Header file for 
//                             CDragonDOSNewDiskWindow, a class
//                             that manages a window for asking
//                             the user the disk image type and
//                             disk parameters for creating a new
//                             disk image.
//
////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Window.H>

#define NDW_GEOMETRY_1S40T		0
#define NDW_GEOMETRY_1S80T		1
#define NDW_GEOMETRY_2S40T		2
#define NDW_GEOMETRY_2S80T		3
#define NDW_GEOMETRIES_NUM		4
#define NDW_GEOMETRY_INVALID	-1
#define NDW_IMAGE_TYPE_INVALID	-1

//******************************************
//* CDragonDOSNewDiskWindow class
//******************************************
struct SDRAGONDOS_Context;

class CDragonDOSNewDiskWindow : public Fl_Window
{
public:
	CDragonDOSNewDiskWindow( int _w, int _h, const char* _label = nullptr );
	CDragonDOSNewDiskWindow( int _x, int _y, int _w, int _h, const char* _label = nullptr );
	virtual ~CDragonDOSNewDiskWindow();

//	virtual int handle(int _event);
//	virtual void show();

	void SetData( SDRAGONDOS_Context* _context );
	
	bool IsDataSet() { return mContext != nullptr; }

	int GetSelectedDiskGeometry() 	{ return mDiskGeometry->value(); }
	int GetSelectedImageType()		{ return mImageType->value(); 	 }

private:
	void CreateControls  ();

	Fl_Choice* mDiskGeometry	= nullptr;
	Fl_Choice* mImageType   	= nullptr;
	Fl_Button* mOkButton		= nullptr;
	Fl_Button* mCancelButton	= nullptr;

	SDRAGONDOS_Context* mContext = nullptr;
};
