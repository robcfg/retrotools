#define _CRT_SECURE_NO_WARNINGS

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <string>
#include <filesystem>

#include "FileSystemInterface.h"
#include "DragonDOS_BASIC.h"
#include "DragonDOS_UI_Callbacks.h"
#include "DragonDOS_ViewFileWindow.h"
#include "VDKDiskImage.h"

using namespace std;

#define UI_MAX_FILE_NAME_LEN 8
#define UI_MAX_FILE_EXT_LEN  3
#define DRAGONDOSUI_BROWSER_LINE_OFFSET 3 // Skip two lines, 1-based
#ifdef WIN32
#define DOS68UI_PATH_SEPARATOR '\\';
#else
#define DOS68UI_PATH_SEPARATOR '/';
#endif

const size_t tmpBufSize = 256;

void UpdateUI( SDRAGONDOS_Context* _context )
{
    IFilesystemInterface* pFS = (IFilesystemInterface*)_context->fs;
    IDiskImageInterface* pDisk = (IDiskImageInterface*)_context->disk;
    char tmpBuf[256] = {0};

    _context->fileLabel->copy_label( _context->diskFilename.c_str() );

	_context->browser->clear();

	_context->browser->add("@f@.File|Name    |Ext|Type|Sec|Bytes \n");
    _context->browser->add("@f@.----+--------+---+----+---+------\n");

    for( size_t fileIdx = 0; fileIdx < _context->fs->GetFilesNum(); ++fileIdx )
    {
        SFileInfo fileInfo = _context->fs->GetFileInfo( fileIdx );

        filesystem::path filePath( pFS->GetFileName( fileIdx ) );
        
        string tmpName = filePath.stem().string();
        if( tmpName.length() < UI_MAX_FILE_NAME_LEN )
        {
            tmpName.insert( tmpName.end(), UI_MAX_FILE_NAME_LEN - tmpName.length(), ' ' );
        }
        string tmpExt = filePath.extension().string();
        if( tmpExt.length() < UI_MAX_FILE_EXT_LEN )
        {
            tmpExt.insert( tmpExt.end(), UI_MAX_FILE_EXT_LEN - tmpExt.length(), ' ' );
        }

        uint16_t fileSectors = pFS->GetFileSize(fileIdx)/pDisk->GetSectorSize(0,0,0); // TODO:Look for a good way to get sector size
        fileSectors += (pFS->GetFileSize(fileIdx)%pDisk->GetSectorSize(0,0,0) != 0) ? 1 : 0;

        snprintf( tmpBuf, tmpBufSize, "@f@.%03zu  %s%s %s %3d %6zu\n", 
                 fileIdx, 
                 tmpName.c_str(), 
                 tmpExt.c_str(),
                 "....",
                 fileSectors,
                 pFS->GetFileSize( fileIdx) );
        _context->browser->add(tmpBuf);
    }

    snprintf( tmpBuf, tmpBufSize, "Disk info:\n%s side(s)\n%s tracks\n%zu total bytes\n%zu free bytes\n%zu free sectors",
        to_string(pDisk->GetSidesNum()).c_str(),
        to_string(pDisk->GetTracksNum()).c_str(),
        pDisk->GetSidesNum()*pDisk->GetTracksNum()*pDisk->GetSectorsNum()*pDisk->GetSectorSize(0,0,0),
        pFS->GetFreeSize(),
        pFS->GetFreeSize()/pDisk->GetSectorSize(0,0,0)
    );
    _context->diskInfoLabel->copy_label( tmpBuf );
}

bool ChooseFilename( string& fileName, bool bSaveAsDialog, bool bDirectory )
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
			native.title("Select file to open");
			native.type(Fl_Native_File_Chooser::BROWSE_FILE);
		}
		//native.filter("Disk image files\t*.*\n");
		//native.preset_file("");
	}

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

void newDisk_cb(Fl_Widget* pWidget,void* _context)
{
    SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
 
    int diskSize = fl_choice_n( "Please select disk size:", "180KB", "360KB", "720KB");
    if( diskSize < 0 )
    {
        return; // Dialog was cancelled.
    }

    string fileName;
    if( !ChooseFilename( fileName, true, false ) )
    {
        return;
    }

    pContext->diskFilename = "(No disk)";
    UpdateUI( pContext );

    // Create new disk
    switch( diskSize )
    {
        case 0 :pContext->disk->New( 40, 1, VDK_SECTORSPERTRACK);break;
        case 1 :pContext->disk->New( 40, 2, VDK_SECTORSPERTRACK);break;
        default:pContext->disk->New( 80, 2, VDK_SECTORSPERTRACK);break;
    }

    // Format it
    if( !pContext->fs->InitDisk(pContext->disk) )
    {
        fl_alert( "Could not initialize filesystem" );
        return;
    }

    // Save it
    if( !pContext->disk->Save(fileName) )
    {
        fl_alert( "Could not save new disk to %s", fileName.c_str() );
        return;
    }

    if( !pContext->fs->Load( pContext->disk ) )
    {
        fl_alert( "Could not open new disk from %s", fileName.c_str() );
        return;
    }

    pContext->diskFilename = fileName;
    UpdateUI( pContext );
}

void openDisk_cb(Fl_Widget* pWidget,void* _context)
{
    string fileName;

    if( !ChooseFilename( fileName, false, false ) )
    {
        return;
    }

    SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;

    if( !pContext->disk->Load( fileName ) )
    {
        fl_alert( "Error opening file %s", pContext->diskFilename.c_str() );
        return;
    }

    if( !pContext->fs->Load( pContext->disk ) )
    {
        fl_alert( "Error processing file %s\nImage may be damaged or corrupt.", fileName.c_str() );
    }

    pContext->diskFilename = fileName;
    UpdateUI( pContext );
}

void saveDisk_cb(Fl_Widget* pWidget,void* _context)
{
    SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
    IFilesystemInterface* fs = (IFilesystemInterface*)pContext->fs;

    if( !fs->Save(pContext->diskFilename) )
    {
        string error = "Couldn't save changes to ";
        error += pContext->diskFilename;
    }
}

void InsertFile( SDRAGONDOS_Context* _context, const string& _filename, bool _isBinary, string& _errors )
{
    IFilesystemInterface* fs = (IFilesystemInterface*)_context->fs;

    string errorStr;

    FILE* pIn = fopen( _filename.c_str(), "rb" );
    if( nullptr == pIn )
    {
        errorStr = "Could not open requested file ";
        errorStr += _filename;
        errorStr += "\n";
        _errors += errorStr;
        return;
    }

    fseek( pIn, 0, SEEK_END );
    size_t insertFileSize = ftell( pIn );
    fseek( pIn, 0, SEEK_SET );

    // Check if there's enough room on the disk for the file to be inserted.
    size_t freeBytes = fs->GetFreeSize();
    if( insertFileSize > freeBytes )
    {
        fclose( pIn );

        errorStr = "Not enough room to insert ";
        errorStr += _filename;
        errorStr += ". Another ";
        errorStr += to_string(insertFileSize - freeBytes);
        errorStr += " are needed.\n";
        _errors += errorStr;
        return;
    }

    vector<unsigned char> fileData;
    fileData.resize( insertFileSize );
    size_t bytesRead = fread( fileData.data(), 1, insertFileSize, pIn );
    fclose( pIn );

    if( bytesRead < insertFileSize )
    {
        errorStr = "Error loading file ";
        errorStr += _filename;
        errorStr += " (";
        errorStr += to_string(bytesRead);
        errorStr += " of ";
        errorStr += to_string(insertFileSize);
        errorStr += " bytes read).\n";
        _errors += errorStr;
        return;
    }

    /*if( !fs->InsertFile( _filename, fileData, _isBinary ) )
    {
        errorStr = "File ";
        errorStr += _filename;
        errorStr += " could not be inserted.\n";
        _errors += errorStr;
        return;
    }*/
}

void insertAscii_cb(Fl_Widget* pWidget,void* _context)
{
    vector<string> fileNames;

    if( !ChooseFilename( fileNames, "All files\t*.*\n", "*.*", false, false ) )
    {
        return;
    }

    string errorStr;

    SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
    for( auto file : fileNames )
    {
        InsertFile( pContext, file, false, errorStr );
    }

    if( !errorStr.empty() )
    {
        fl_alert( "%s", errorStr.c_str() );
    }

    pContext->fs->Load( pContext->disk );
    UpdateUI( pContext );
}

void insertBinary_cb(Fl_Widget* pWidget,void* _context)
{
    vector<string> fileNames;

    if( !ChooseFilename( fileNames, "All files\t*.*\n", "*.*", false, false ) )
    {
        return;
    }

    string errorStr;

    SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
    for( auto file : fileNames )
    {
        InsertFile( pContext, file, true, errorStr );
    }

    if( !errorStr.empty() )
    {
        fl_alert( "%s", errorStr.c_str() );
    }

    pContext->fs->Load( pContext->disk );
    UpdateUI( pContext );
}

void extractFiles_cb(Fl_Widget* pWidget,void* _context)
{
    SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
    IFilesystemInterface* fs = (IFilesystemInterface*)pContext->fs;
    string path;

    if( pContext->browser->size() < DRAGONDOSUI_BROWSER_LINE_OFFSET )
    {
        return;
    }

    if( !ChooseFilename( path, true, true ) )
    {
        return;
    }

    string errors;

    for( int line = DRAGONDOSUI_BROWSER_LINE_OFFSET; line <= pContext->browser->size(); ++line  )
    {
        if( pContext->browser->selected(line) )
        {
            SFileInfo fi = fs->GetFileInfo( line - DRAGONDOSUI_BROWSER_LINE_OFFSET );
            vector<unsigned char> fileData;
            string fileName = path;
            fileName += DOS68UI_PATH_SEPARATOR;
            fileName += fi.name;

            if( !fs->ExtractFile(fi.name, fileData) )
            {
                errors += "Couldn't extract file ";
                errors += fi.name;
                errors += "\n";
                continue;
            }
            
            FILE* pOut = fopen( fileName.c_str(), "wb" );
            if( nullptr == pOut )
            {
                errors += "Error writing file ";
                errors += fileName;
                errors += "\n";
                continue;
            }

            size_t bytesWritten = fwrite( fileData.data(), 1, fileData.size(), pOut );
            fclose( pOut );

            if( bytesWritten != fileData.size() )
            {
                errors += "Error writing file ";
                errors += fileName;
                errors += " ";
                errors += to_string(bytesWritten);
                errors += " of ";
                errors += to_string(fileData.size());
                errors += " bytes written.\n";
                continue;
            }
        }
    }

    if( !errors.empty() )
    {
        fl_alert( "%s", errors.c_str() );
    }
}

void removeFiles_cb(Fl_Widget* pWidget,void* _context)
{
    SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
    IFilesystemInterface* fs = (IFilesystemInterface*)pContext->fs;

    vector<string> fileNames;

    for( int line = DRAGONDOSUI_BROWSER_LINE_OFFSET; line <= pContext->browser->size(); ++line  )
    {
        if( pContext->browser->selected(line) )
        {
            fileNames.push_back( fs->GetFileName(line - DRAGONDOSUI_BROWSER_LINE_OFFSET) );
        }
    }

    for( auto fileName : fileNames )
    {
        /*if( !fs->RemoveFile( fileName) )
        {
            string error = "Couldn't remove file ";
            error += fileName;
            error += " , stopping operation.";
            fl_alert( "%s", error.c_str() );
            UpdateUI( pContext );
            return;
        }*/
    }

    UpdateUI( pContext );
}

void viewFiles_cb(Fl_Widget* pWidget,void* _context)
{
    SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
    IFilesystemInterface* fs = (IFilesystemInterface*)pContext->fs;

    stringstream decodedFiles;
    string fltkTextColors;
    vector<int> selectedFiles;

    // Line numbers are 1 based. First 2 lines are the header lines.
    for( int line = DRAGONDOSUI_BROWSER_LINE_OFFSET; line <= pContext->browser->size() ; ++line )
    {
        if( pContext->browser->selected( line ) )
        {
            selectedFiles.push_back( line - DRAGONDOSUI_BROWSER_LINE_OFFSET );
        }
    }

    pContext->viewFileWindow->SetData( fs, selectedFiles );
    pContext->viewFileWindow->show();

    for( int line = DRAGONDOSUI_BROWSER_LINE_OFFSET; line <= pContext->browser->size(); ++line  )
    {
        if( pContext->browser->selected(line) )
        {
            vector<unsigned char> file;
            fs->ExtractFile( fs->GetFileName(line - DRAGONDOSUI_BROWSER_LINE_OFFSET), file );

            unsigned short int programStart = 0x1E01;
            DragonDOS_BASIC::Decode( file, decodedFiles, fltkTextColors, programStart, false, false );
        }
    }
}

void viewFileAsHex_cb( Fl_Widget* pWidget, void* _viewFileWindow )
{
    if( _viewFileWindow )
    {
        ((CDragonDOSViewFileWindow*)_viewFileWindow)->ViewAsHex();
    }
}

void viewFileAsText_cb( Fl_Widget* pWidget, void* _viewFileWindow )
{
    if( _viewFileWindow )
    {
        ((CDragonDOSViewFileWindow*)_viewFileWindow)->ViewAsText();
    }
}

void viewFileAsBasic_cb( Fl_Widget* pWidget, void* _viewFileWindow )
{
    if( _viewFileWindow )
    {
        ((CDragonDOSViewFileWindow*)_viewFileWindow)->ViewAsBasic();
    }
}

void viewFileAsImage_cb( Fl_Widget* pWidget, void* _viewFileWindow )
{
    if( _viewFileWindow )
    {
        ((CDragonDOSViewFileWindow*)_viewFileWindow)->ViewAsImage();
    }
}

void viewFileModeChanged_cb( Fl_Widget* pWidget, void* _viewFileWindow )
{
    if( _viewFileWindow )
    {
        ((CDragonDOSViewFileWindow*)_viewFileWindow)->DecodeImage();
    }
}

void viewFilePaletteChanged_cb( Fl_Widget* pWidget, void* _viewFileWindow )
{
    if( _viewFileWindow )
    {
        ((CDragonDOSViewFileWindow*)_viewFileWindow)->DecodeImage();
    }
}

void viewFileExportPNG_cb( Fl_Widget* pWidget, void* _viewFileWindow )
{
    if( _viewFileWindow )
    {
        ((CDragonDOSViewFileWindow*)_viewFileWindow)->ExportImage();
    }
}
