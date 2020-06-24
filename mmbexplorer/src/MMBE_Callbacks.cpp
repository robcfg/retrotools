#include <string>
#include <FL/Fl_Native_File_Chooser.H>
#include "MMBE_Callbacks.h"
#include "MMBE_Gui.h"

// Debug includes
#include <iostream>

using namespace std;

bool ChooseFilename( std::string& fileName, const std::string& filter, const std::string& preset, bool bSaveAsDialog )
{
	// Create native chooser
	Fl_Native_File_Chooser native;
	if( bSaveAsDialog )
	{
		native.title("Save as");
		native.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
		native.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | Fl_Native_File_Chooser::USE_FILTER_EXT);
	}
	else
	{
		native.title("Select file to open");
		native.type(Fl_Native_File_Chooser::BROWSE_FILE);
	}
	native.filter( filter.c_str() );
	native.preset_file( preset.c_str() );
	
	// Show native chooser
	switch ( native.show() )
	{
	case -1: return false; break;	// ERROR
	case  1: return false; break;	// CANCEL
	default: 						// PICKED FILE
		if ( native.filename() )
		{
			fileName = native.filename();
		}
	}

	return true;
}

void openFile_cb( Fl_Widget* pWidget, void* _gui )
{
	string filename;
    string errorString;

	if( !ChooseFilename( filename, "MMB files\t*.mmb\n", ".", false) )
		return;

    ((CMMBEGui*)_gui)->OpenMMB( filename );
}

void closeFile_cb( Fl_Widget* pWidget, void* _gui )
{
    ((CMMBEGui*)_gui)->CloseMMB();
}

void createFile_cb( Fl_Widget* pWidget, void* _gui )
{
	string filename;
    string errorString;

	if( !ChooseFilename( filename, "MMB files\t*.mmb\n", ".", true) )
		return;

    ((CMMBEGui*)_gui)->CreateMMB( filename );
}

void menuQuit_cb( Fl_Widget* pWidget, void* _gui )
{
    ((CMMBEGui*)_gui)->Stop();
}
