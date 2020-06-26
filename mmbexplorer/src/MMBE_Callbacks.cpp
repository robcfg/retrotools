#include <string>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
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

size_t ChooseSlot()
{
	const char* result = fl_input( "Enter slot number (1 - 511)", nullptr );
	if( nullptr == result )
	{
		return (size_t)-1;
	}

	return (size_t)strtoul( result, nullptr, 0);
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

void menuQuit_cb( Fl_Widget* pWidget, void* )
{
    pWidget->window()->hide();
}

void insertDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	if( 0 == pGui->GetNumberOfSlots() )
	{
		return;
	}

	size_t slot = pGui->GetSelectedSlot() != (size_t)-1 ? pGui->GetSelectedSlot() : ChooseSlot();
	if( slot == (size_t)-1 )
	{
		return;
	}

	std::string filename;
	if( !ChooseFilename( filename, "SSD files\t*.ssd\n", ".", false) )
		return;

	pGui->InsertDisk( filename, slot );
}

void extractDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	if( 0 == pGui->GetNumberOfSlots() )
	{
		return;
	}

	size_t slot = pGui->GetSelectedSlot() != (size_t)-1 ? pGui->GetSelectedSlot() : ChooseSlot();
	if( slot == (size_t)-1 )
	{
		return;
	}

	std::string filename;
	if( !ChooseFilename( filename, "SSD files\t*.ssd\n", ".", true) )
		return;

	pGui->ExtractDisk( filename, slot );
}

void removeDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	if( 0 == pGui->GetNumberOfSlots() )
	{
		return;
	}

	size_t slot = pGui->GetSelectedSlot() != (size_t)-1 ? pGui->GetSelectedSlot() : ChooseSlot();
	if( slot == (size_t)-1 )
	{
		return;
	}

	pGui->RemoveDisk( slot );
}

void lockDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	if( 0 == pGui->GetNumberOfSlots() )
	{
		return;
	}

	size_t slot = pGui->GetSelectedSlot() != (size_t)-1 ? pGui->GetSelectedSlot() : ChooseSlot();
	if( slot == (size_t)-1 )
	{
		return;
	}

	pGui->LockDisk( slot );
}

void unlock_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	if( 0 == pGui->GetNumberOfSlots() )
	{
		return;
	}

	size_t slot = pGui->GetSelectedSlot() != (size_t)-1 ? pGui->GetSelectedSlot() : ChooseSlot();
	if( slot == (size_t)-1 )
	{
		return;
	}

	pGui->UnlockDisk( slot );
}
