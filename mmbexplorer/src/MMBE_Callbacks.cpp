#include <string>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include "MMBE_Callbacks.h"
#include "MMBE_Gui.h"

// Debug includes
#include <iostream>

using namespace std;

bool ChooseFilename( std::string& fileName, const std::string& filter, const std::string& preset, bool bSaveAsDialog, bool bDirectory )
{
	std::vector<std::string> result;
	if( ChooseFilename( result, filter, preset, bSaveAsDialog, bDirectory ) && result.size() > 0 )
	{
		fileName = result[0];
		return true;
	}

	return false;
}

bool ChooseFilename( std::vector<std::string>& fileNames, const std::string& filter, const std::string& preset, bool bSaveAsDialog, bool bDirectory )
{
	// Create native chooser
	Fl_Native_File_Chooser native;
	if( bDirectory )
	{
		native.title( bSaveAsDialog ? "Save to folder" : "Choose folder" );
		native.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
		native.options(Fl_Native_File_Chooser::NEW_FOLDER);
	}
	else
	{
		if( bSaveAsDialog )
		{
			native.title("Save as");
			native.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
			native.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | Fl_Native_File_Chooser::USE_FILTER_EXT);
		}
		else
		{
			native.title("Select file(s) to open");
			native.type(Fl_Native_File_Chooser::BROWSE_MULTI_FILE);
		}
		native.filter( filter.c_str() );
		native.preset_file( preset.c_str() );
	}

	// Show native chooser
	switch ( native.show() )
	{
	case -1: return false; break;	// ERROR
	case  1: return false; break;	// CANCEL
	default: 
		for( int iFilename = 0; iFilename < native.count(); ++iFilename )	// PICKED FILES
		{
			fileNames.push_back( native.filename(iFilename) );
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

	size_t slot = pGui->GetSelectionSize() == 1 ? pGui->GetSelection()[0] : ChooseSlot();
	if( slot == (size_t)-1 )
	{
		return;
	}

	std::vector<std::string> filenames;
	if( !ChooseFilename( filenames, "SSD files\t*.ssd\n", ".", false) )
		return;

	for( auto& filename : filenames )
	{
		if( slot < pGui->GetNumberOfSlots() )
		{
			pGui->InsertDisk( filename, slot++ );
		}
	}
}

void extractDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	if( 0 == pGui->GetNumberOfSlots() )
	{
		return;
	}

	pGui->ExtractSelectedDisks();
}

void removeDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	if( 0 == pGui->GetNumberOfSlots() )
	{
		return;
	}

	if( pGui->GetSelectionSize() > 0 )
	{
		pGui->RemoveSelectedDisks();
	}
	else
	{
		size_t slot = ChooseSlot();
		if( slot != (size_t)-1 )
		{
			pGui->RemoveDisk( slot );
		}
	}
}

void lockDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	if( 0 == pGui->GetNumberOfSlots() )
	{
		return;
	}

	if( pGui->GetSelectionSize() > 0 )
	{
		pGui->LockSelectedDisks();
	}
	else
	{
		size_t slot = ChooseSlot();
		if( slot != (size_t)-1 )
		{
			pGui->LockDisk( slot );
		}
	}
}

void unlockDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	if( 0 == pGui->GetNumberOfSlots() )
	{
		return;
	}

	if( pGui->GetSelectionSize() > 0 )
	{
		pGui->UnlockSelectedDisks();
	}
	else
	{
		size_t slot = ChooseSlot();
		if( slot != (size_t)-1 )
		{
			pGui->UnlockDisk( slot );
		}
	}
}

void menuAbout_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;
	pGui->ShowAboutDialog();	
}

// Disk menu callbacks
void formatDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	for( auto slot : pGui->GetSelection() )
	{
		pGui->FormatDisk( slot );
	}
}

void nameDisk_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	if( pGui->GetSelectionSize() != 1 )
	{
		return;
	}

	const char* result = fl_input( "Enter disk name (12 characters max.)", nullptr );
	if( nullptr == result )
	{
		return;
	}

	size_t slot = pGui->GetSelection()[0];

	pGui->NameDisk( slot, result );
}

void insertFile_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	if( pGui->GetSelectionSize() != 1 )
	{
		return;
	}

	size_t slot = pGui->GetSelection()[0];

	std::vector<std::string> filenames;
	if( !ChooseFilename( filenames, "", "", false, false ) )
		return;

	for( auto& filename : filenames )
	{
		pGui->InsertFile( slot, filename );
	}
}

void extractFile_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	if( pGui->GetSelectionSize() != 1 )
	{
		return;
	}

	pGui->ExtractSelectedFiles();
}

void removeFile_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	std::vector<int> selection;
	pGui->GetSelectedFiles( selection );
	size_t slot = 0;

	if( pGui->GetSelectionSize() != 1 )
	{
		return;
	}
	slot = pGui->GetSelection()[0];

	for( auto idx : selection )
	{
		pGui->RemoveFile( slot, idx );
	}
}

void lockFile_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	std::vector<int> selection;
	pGui->GetSelectedFiles( selection );
	size_t slot = 0;

	if( pGui->GetSelectionSize() != 1 )
	{
		return;
	}
	slot = pGui->GetSelection()[0];

	for( auto idx : selection )
	{
		pGui->LockFile( slot, idx );
	}
}

void unlockFile_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	std::vector<int> selection;
	pGui->GetSelectedFiles( selection );
	size_t slot = 0;

	if( pGui->GetSelectionSize() != 1 )
	{
		return;
	}
	slot = pGui->GetSelection()[0];

	for( auto idx : selection )
	{
		pGui->UnlockFile( slot, idx );
	}
}

void setBootOption0_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	for( auto slot : pGui->GetSelection() )
	{
		pGui->SetBootOption( slot, 0 );
	}
}

void setBootOption1_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	for( auto slot : pGui->GetSelection() )
	{
		pGui->SetBootOption( slot, 1 );
	}
}

void setBootOption2_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	for( auto slot : pGui->GetSelection() )
	{
		pGui->SetBootOption( slot, 2 );
	}
}

void setBootOption3_cb( Fl_Widget* pWidget, void* _gui )
{
	CMMBEGui* pGui = (CMMBEGui*)_gui;

	for( auto slot : pGui->GetSelection() )
	{
		pGui->SetBootOption( slot, 3 );
	}
}

size_t GetFileSize( const std::string& _filename )
{
	size_t retVal = 0;
	FILE* pFile = fopen( _filename.c_str(), "rb" );
	if( nullptr != pFile )
	{
		fseek( pFile, 0, SEEK_END );
		retVal = ftell( pFile );
	}

	return retVal;
}