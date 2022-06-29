#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <string>

#include "DOS68_BlankDiskImage.h"
#include "DOS68_FS.h"
#include "DOS68_UI_Callbacks.h"

using namespace std;

#define DOS68UI_BROWSER_LINE_OFFSET 3 // Skip two lines, 1-based
#ifdef WIN32
#define DOS68UI_PATH_SEPARATOR '\\';
#else
#define DOS68UI_PATH_SEPARATOR '/';
#endif


void UpdateUI( SDOS68UI_Context* _context )
{
    CDOS68_FS* pFS = (CDOS68_FS*)_context->fs;
    char tmpBuf[256] = {0};

    _context->fileLabel->copy_label( _context->diskFilename.c_str() );

	_context->browser->clear();

	_context->browser->add("@f@.File|Name  |Ext |FTFS|BTBS|ETES|Sec|Bytes\n");
    _context->browser->add("@f@.----+------+----+----+----+----+---+-----\n");

    for( size_t fileIdx = 0; fileIdx < _context->fs->GetFilesNum(); ++fileIdx )
    {
        SFileInfo fileInfo = _context->fs->GetFileInfo( fileIdx );
        SDOS68_FileInfoBlock fib = pFS->GetFIB( fileIdx );

        string tmpName = fib.name;
        if( tmpName.length() < DOS68_MAX_FILE_NAME_LEN )
        {
            tmpName.insert( tmpName.end(), DOS68_MAX_FILE_NAME_LEN - tmpName.length(), ' ' );
        }
        string tmpExt = fib.ext;
        if( tmpExt.length() < DOS68_MAX_FILE_EXT_LEN )
        {
            tmpExt.insert( tmpExt.end(), DOS68_MAX_FILE_EXT_LEN - tmpExt.length(), ' ' );
        }

        uint16_t fileSectors = (fib.sectorsNumHigh << 8) | fib.sectorsNumLow;

        sprintf( tmpBuf, "@f@.%03zu  %s.%s  %02X%02X %02X%02X %02X%02X %3d %5d\n", 
                 fileIdx, 
                 tmpName.c_str(), 
                 tmpExt.c_str(),
                 fib.type,
                 fib.status,
                 fib.firstTrack  | DOS68_TRACK_ID_MARK,
                 fib.firstSector | DOS68_SECTOR_ID_MARK,
                 fib.lastTrack   | DOS68_TRACK_ID_MARK,
                 fib.lastSector  | DOS68_SECTOR_ID_MARK,
                 fileSectors,
                 fileSectors * DOS68_SECTOR_DATA_SIZE );
        _context->browser->add(tmpBuf);
    }

    unsigned char diskType = pFS->GetDiskType();
    sprintf( tmpBuf, "Disk info:\n%s side(s)\n%s density\n%s\"\n%s TPI\n%s\n%u free sectors\n%u free bytes",
        (((diskType & DOS68_DISK_TYPE_SIDES_MASK    ) == DOS68_DISK_TYPE_SINGLE_SIDE   ) ? "1"           : "2"        ),
        (((diskType & DOS68_DISK_TYPE_DENSITY_MASK  ) == DOS68_DISK_TYPE_SINGLE_DENSITY) ? "Single"      : "Double"   ),
        (((diskType & DOS68_DISK_TYPE_INCHES_MASK   ) == DOS68_DISK_TYPE_5_INCHES      ) ? "5.25"        : "8"        ),
        (((diskType & DOS68_DISK_TYPE_TPI_MASK      ) == DOS68_DISK_TYPE_48_TPI        ) ? "48"          : "96"       ),
        (((diskType & DOS68_DISK_TYPE_PROTECTED_MASK) == DOS68_DISK_TYPE_UNPROTECTED   ) ? "Unprotected" : "Protected (Master)"),
        pFS->GetAvailableSectorsNum(),
        pFS->GetAvailableSectorsNum() * DOS68_SECTOR_DATA_SIZE
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
		native.filter("Disk image files\t*.img\n");
		native.preset_file(".img");
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
    string fileName;

    if( !ChooseFilename( fileName, true, false ) )
    {
        return;
    }

    FILE* pOut = fopen( fileName.c_str(), "wb" );
    if( nullptr == pOut )
    {
        fl_alert( "Error creating file %s", fileName.c_str() );
        return;
    }

    size_t bytesWritten = fwrite( blankImage, 1, BLANK_DISK_IMAGE_SIZE, pOut );
    fclose( pOut );

    if( bytesWritten != BLANK_DISK_IMAGE_SIZE )
    {
        fl_alert( "Error writing file %s %zu of %d bytes written.", fileName.c_str(), bytesWritten, BLANK_DISK_IMAGE_SIZE );
        return;
    }

    SDOS68UI_Context* pContext = (SDOS68UI_Context*)_context;
    pContext->diskFilename = fileName;
    if( !pContext->disk->Load( pContext->diskFilename ) )
    {
        fl_alert( "Error opening file %s", pContext->diskFilename.c_str() );
    }
    if( !pContext->fs->Load( pContext->disk ) )
    {
        fl_alert( "Error processing file %s\nImage may be damaged or corrupt.", pContext->diskFilename.c_str() );
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

    SDOS68UI_Context* pContext = (SDOS68UI_Context*)_context;
    if( !pContext->disk->Load( fileName ) )
    {
        fl_alert( "Error opening file %s", pContext->diskFilename.c_str() );
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
    SDOS68UI_Context* pContext = (SDOS68UI_Context*)_context;
    CDOS68_FS* fs = (CDOS68_FS*)pContext->fs;

    if( !fs->Save(pContext->diskFilename) )
    {
        string error = "Couldn't save changes to ";
        error += pContext->diskFilename;
    }
}

void InsertFile( SDOS68UI_Context* _context, const string& _filename, bool _isBinary, string& _errors )
{
    CDOS68_FS* fs = (CDOS68_FS*)_context->fs;

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
    size_t freeBytes = fs->GetAvailableSectorsNum() * DOS68_SECTOR_DATA_SIZE;
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

    if( !fs->InsertFile( _filename, fileData, _isBinary ) )
    {
        errorStr = "File ";
        errorStr += _filename;
        errorStr += " could not be inserted.\n";
        _errors += errorStr;
        return;
    }
}

void insertAscii_cb(Fl_Widget* pWidget,void* _context)
{
    vector<string> fileNames;

    if( !ChooseFilename( fileNames, "All files\t*.*\n", "*.*", false, false ) )
    {
        return;
    }

    string errorStr;

    SDOS68UI_Context* pContext = (SDOS68UI_Context*)_context;
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

    SDOS68UI_Context* pContext = (SDOS68UI_Context*)_context;
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
    SDOS68UI_Context* pContext = (SDOS68UI_Context*)_context;
    CDOS68_FS* fs = (CDOS68_FS*)pContext->fs;
    string path;

    if( pContext->browser->size() < DOS68UI_BROWSER_LINE_OFFSET )
    {
        return;
    }

    if( !ChooseFilename( path, true, true ) )
    {
        return;
    }

    string errors;

    for( int line = DOS68UI_BROWSER_LINE_OFFSET; line <= pContext->browser->size(); ++line  )
    {
        if( pContext->browser->selected(line) )
        {
            SFileInfo fi = fs->GetFileInfo( line - DOS68UI_BROWSER_LINE_OFFSET );
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
    SDOS68UI_Context* pContext = (SDOS68UI_Context*)_context;
    CDOS68_FS* fs = (CDOS68_FS*)pContext->fs;

    vector<string> fileNames;

    for( int line = DOS68UI_BROWSER_LINE_OFFSET; line <= pContext->browser->size(); ++line  )
    {
        if( pContext->browser->selected(line) )
        {
            fileNames.push_back( fs->GetFileName(line - DOS68UI_BROWSER_LINE_OFFSET) );
        }
    }

    for( auto fileName : fileNames )
    {
        if( !fs->DeleteFile( fileName ) )
        {
            string error = "Couldn't remove file ";
            error += fileName;
            error += " , stopping operation.";
            fl_alert( "%s", error.c_str() );
            UpdateUI( pContext );
            return;
        }
    }

    UpdateUI( pContext );
}
