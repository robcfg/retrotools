#define _CRT_SECURE_NO_WARNINGS

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <string>
#include <filesystem>

#include "../../common/FileSystems/FileSystemInterface.h"
#include "DragonDOS_BASIC.h"
#include "DragonDOS_Common.h"
#include "../../common/FileSystems/DragonDOS_FS.h"
#include "DragonDOS_UI_Callbacks.h"
#include "DragonDOS_ViewFileWindow.h"
#include "../../common/DiskImages/VDKDiskImage.h"

#define UI_MAX_FILE_NAME_LEN 8
#define UI_MAX_FILE_EXT_LEN  4            // Dot plus three characters
#define DRAGONDOSUI_BROWSER_LINE_OFFSET 3 // Skip two lines, 1-based
#ifdef WIN32
#define DRAGONDOSUI_PATH_SEPARATOR '\\';
#else
#define DRAGONDOSUI_PATH_SEPARATOR '/';
#endif

const size_t DRAGONDOS_DISK_SIZE_180KB = (1 * 40 * DRAGONDOS_SECTORSPERTRACK * DRAGONDOS_SECTOR_SIZE);
const size_t DRAGONDOS_DISK_SIZE_360KB = (2 * 40 * DRAGONDOS_SECTORSPERTRACK * DRAGONDOS_SECTOR_SIZE);
const size_t DRAGONDOS_DISK_SIZE_720KB = (2 * 80 * DRAGONDOS_SECTORSPERTRACK * DRAGONDOS_SECTOR_SIZE);

const size_t tmpBufSize = 256;

void UpdateUI( const SDRAGONDOS_Context* _context )
{
	CDragonDOS_FS* pFS = (CDragonDOS_FS*)_context->fs;
	IDiskImageInterface* pDisk = (IDiskImageInterface*)_context->disk;
	char tmpBuf[256] = {0};

	_context->fileLabel->copy_label( _context->diskFilename.c_str() );

	_context->browser->clear();

	_context->browser->add("@f@.File|Name    |Ext|Type|Sec|Bytes |Load|Exec\n");
	_context->browser->add("@f@.----+--------+---+----+---+------+----+----\n");

	if( pDisk == nullptr )
	{
		return;
	}

	for( size_t fileIdx = 0; fileIdx < _context->fs->GetFilesNum(); ++fileIdx )
	{
		SFileInfo fileInfo = _context->fs->GetFileInfo( fileIdx );

		std::filesystem::path filePath( pFS->GetFileName( fileIdx ) );
		
		std::string tmpName = filePath.stem().string();
		if( tmpName.length() < UI_MAX_FILE_NAME_LEN )
		{
			tmpName.insert( tmpName.end(), UI_MAX_FILE_NAME_LEN - tmpName.length(), ' ' );
		}
		std::string tmpExt = filePath.extension().string();
		if( tmpExt.length() < UI_MAX_FILE_EXT_LEN )
		{
			tmpExt.insert( tmpExt.end(), UI_MAX_FILE_EXT_LEN - tmpExt.length(), ' ' );
		}

		uint16_t fileSectors = (uint16_t)(pFS->GetFileSize(fileIdx)/pDisk->GetSectorSize());
		fileSectors += (pFS->GetFileSize(fileIdx)%pDisk->GetSectorSize() != 0) ? 1 : 0;

		CDGNDosFile ddosFile = pFS->GetFile((unsigned short int)fileIdx);
		std::string fileType = pFS->GetFileTypeString((unsigned short int)fileIdx);
		fileType += ' '; // padding

		snprintf(   tmpBuf, tmpBufSize, "@f@.%03zu  %s%s %s %3d %6zu %04X %04X\n", 
					fileIdx, 
					tmpName.c_str(), 
					tmpExt.c_str(),
					fileType.c_str(),
					fileSectors,
					pFS->GetFileSize(fileIdx),
					ddosFile.GetLoadAddress(),
					ddosFile.GetExecAddress() );
		_context->browser->add(tmpBuf);
	}

	snprintf( tmpBuf, tmpBufSize, "Disk info:\n%s side(s)\n%s tracks\n%zu total bytes\n%zu free bytes\n%zu free sectors",
		std::to_string(pDisk->GetSidesNum()).c_str(),
		std::to_string(pDisk->GetTracksNum()).c_str(),
		pDisk->GetSidesNum()*pDisk->GetTracksNum()*pDisk->GetSectorsNum()*pDisk->GetSectorSize(),
		pFS->GetFreeSize(),
		pFS->GetFreeSize()/pDisk->GetSectorSize()
	);
	_context->diskInfoLabel->copy_label( tmpBuf );
}

bool ChooseFilename( std::string& fileName, bool bSaveAsDialog, bool bDirectory, const SDRAGONDOS_Context* _context  )
{
	// Create native chooser
	Fl_Native_File_Chooser native;
	std::string filters;

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
			native.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
		}
		else
		{
			native.title("Select file to open");
			native.type(Fl_Native_File_Chooser::BROWSE_FILE);
		}

		native.filter("Disk image files\t*.*\n");
		native.preset_file("");
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

void AskLoadAndExecAddresses( unsigned short int& loadAddress, unsigned short int& execAddress )
{
	loadAddress = 0;
	execAddress = 0;

	// Ask load address
	const char* strLoadAddress = fl_input( "Enter load address as dec (3072) or hex (0xC00), for example.","0" );
	if( nullptr != strLoadAddress )
	{
		unsigned long int parsedLoadAddress = strtoul( strLoadAddress, nullptr, 0 );
		if( parsedLoadAddress > 0xFFFF )
		{
			fl_alert( "Out of range address %zu\nValid range is 0 to 65535 (0xFFFF)", parsedLoadAddress );
			return;
		}
		loadAddress = (unsigned short int)parsedLoadAddress;
	}

	// Ask exec address
	const char* strExecAddress = fl_input( "Enter exec address as dec (3072) or hex (0xC00), for example.","0" );
	if( nullptr != strExecAddress )
	{
		unsigned long int parsedExecAddress = strtoul( strExecAddress, nullptr, 0 );
		if( parsedExecAddress > 0xFFFF )
		{
			fl_alert( "Out of range address %zu\nValid range is 0 to 65535 (0xFFFF)", parsedExecAddress );
			return;
		} 
		execAddress = (unsigned short int)parsedExecAddress;
	}
}

#ifndef __APPLE__
void menuQuit_cb(Fl_Widget* pWidget,void* _context)
{
	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;

	pContext->mainWindow->hide();
}

void menuAbout_cb(Fl_Widget* pWidget,void* _context)
{
	if( nullptr != Fl::modal() )
	{
		return;
	}

	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;

	pContext->aboutWindow->show();	
}
#endif

void newDisk_cb(Fl_Widget* pWidget,void* _context)
{
    if( nullptr != Fl::modal() )
    {
        return;
    }

	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;

	if( pContext->newDiskWindow->IsDataSet() )
	{
		int posX = pContext->mainWindow->x() + ((pContext->mainWindow->w() - pContext->newDiskWindow->w())/2);
		int posY = pContext->mainWindow->y() + ((pContext->mainWindow->h() - pContext->newDiskWindow->h())/2);
		pContext->newDiskWindow->position(posX,posY);
		pContext->newDiskWindow->show();
	}
}

void openDisk_cb(Fl_Widget* pWidget,void* _context)
{
	std::string fileName;

	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;

	if( !ChooseFilename( fileName, false, false, pContext ) )
	{
		return;
	}

	if( pContext->disk )
	{
		delete pContext->disk;
	}
	pContext->disk = pContext->diskImageFactory->LoadDiskImage( fileName );

	if( pContext->disk == nullptr )
	{
		fl_alert( "Error opening file %s", pContext->diskFilename.c_str() );
		return;
	}
	
	// If a raw disk is loaded, try to guess geometry based on known disk sizes.
	bool fsLoaded = false;
	if( pContext->disk->NeedManualSetup() )
	{
		switch( pContext->disk->GetDataSize() )
		{
		case DRAGONDOS_DISK_SIZE_180KB:
		{
			pContext->disk->SetSidesNum(1);
			pContext->disk->SetTracksNum(40);
			pContext->disk->SetSectorsNum(DRAGONDOS_SECTORSPERTRACK);
			pContext->disk->SetSectorSize(DRAGONDOS_SECTOR_SIZE);
			fsLoaded = pContext->fs->Load( pContext->disk );
		}
		break;
		// This case is ambiguous as it can be either 1 side and 80 tracks, or
		// 2 sides and 40 tracks. As only one of these configurations would be
		// valid, let the filesystem try to load them to determine which one
		// works.
		case DRAGONDOS_DISK_SIZE_360KB:
		{
			// Try 1 side and 80 tracks.
			pContext->disk->SetSidesNum(1);
			pContext->disk->SetTracksNum(80);
			pContext->disk->SetSectorsNum(DRAGONDOS_SECTORSPERTRACK);
			pContext->disk->SetSectorSize(DRAGONDOS_SECTOR_SIZE);
			fsLoaded = pContext->fs->Load( pContext->disk );

			// If the filesystem didn't load, try 2 sides and 40 tracks.
			if( !fsLoaded )
			{
				pContext->disk->SetSidesNum(2);
				pContext->disk->SetTracksNum(40);
				fsLoaded = pContext->fs->Load( pContext->disk );
			}
		}
		break;
		case DRAGONDOS_DISK_SIZE_720KB:
		{
			pContext->disk->SetSidesNum(2);
			pContext->disk->SetTracksNum(80);
			pContext->disk->SetSectorsNum(DRAGONDOS_SECTORSPERTRACK);
			pContext->disk->SetSectorSize(DRAGONDOS_SECTOR_SIZE);
			fsLoaded = pContext->fs->Load( pContext->disk );
		}
		break;
		
		default:
			break;
		}
	}
	else
	{
		fsLoaded = pContext->fs->Load( pContext->disk );
	}

	if( !fsLoaded )
	{
		fl_alert( "Error processing file %s\nImage may be damaged or corrupt.", fileName.c_str() );
	}

	pContext->diskFilename = fileName;
	UpdateUI( pContext );
}

void saveDisk_cb(Fl_Widget* pWidget,void* _context)
{
	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
	IFileSystemInterface* fs = (IFileSystemInterface*)pContext->fs;

	if( !fs->Save(pContext->diskFilename) )
	{
		std::string error = "Couldn't save changes to ";
		error += pContext->diskFilename;
	}
}

void insertBasic_cb(Fl_Widget* pWidget,void* _context)
{
	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
	CDragonDOS_FS* fs = (CDragonDOS_FS*)pContext->fs;
	std::vector<std::string> fileNames;

	if( !ChooseFilename( fileNames, "All files\t*.*\n", "*.*", false, false ) )
	{
		return;
	}

	std::string errorStr;

	for( auto file : fileNames )
	{
		FILE* pIn = fopen( file.c_str(), "rb" );
		if( nullptr == pIn )
		{
			errorStr += "Could not open requested file ";
			errorStr += file;
			errorStr += "\n";
			continue;
		}

		fseek( pIn, 0, SEEK_END );
		size_t insertFileSize = ftell( pIn );
		fseek( pIn, 0, SEEK_SET );

		// Check if there's enough room on the disk for the file to be inserted.
		size_t freeBytes = fs->GetFreeSize();
		if( insertFileSize > DRAGONDOS_MAX_FILE_SIZE - DRAGONDOS_FILEHEADER_SIZE )
		{
			fclose( pIn );

			errorStr += "Cannot insert file ";
			errorStr += file;
			errorStr += " ,size is greater than 65535 bytes.";
			continue;
		}
		if( insertFileSize > freeBytes )
		{
			fclose( pIn );

			errorStr += "Not enough room to insert ";
			errorStr += file;
			errorStr += ". Another ";
			errorStr += std::to_string(insertFileSize - freeBytes);
			errorStr += " are needed.\n";
			continue;
		}

		std::vector<char> fileData;
		fileData.resize( insertFileSize );
		size_t bytesRead = fread( fileData.data(), 1, insertFileSize, pIn );
		fclose( pIn );

		std::vector<unsigned char> encodedData; 
		
		// Add file header /////////////////////////////////
		// For BASIC files, load address is usually always 
		// 0x2401 and the exec address 0x8B8D
		////////////////////////////////////////////////////
		encodedData.push_back( DRAGONDOS_FILE_HEADER_BEGIN ); // Constant
		encodedData.push_back( DRAGONDOS_FILETYPE_BASIC    ); // File type
		encodedData.push_back( 0x24                        ); // Load address high byte
		encodedData.push_back( 0x01                        ); // Load address low byte
		encodedData.push_back( 0x00                        ); // File size high byte (updated below)
		encodedData.push_back( 0x00                        ); // File size low byte (updated below)
		encodedData.push_back( 0x8B                        ); // Exec address high byte
		encodedData.push_back( 0x8D                        ); // Exec address low byte
		encodedData.push_back( DRAGONDOS_FILE_HEADER_END   ); // Constant
		////////////////////////////////////////////////////

		DragonDOS_BASIC::Encode( fileData, encodedData );

		encodedData[4] = (encodedData.size() / 256) & 0xFF;
		encodedData[5] = encodedData.size() & 0xFF;

		std::filesystem::path filePath( file );
		fs->InsertFile( filePath.filename().string(), encodedData, true );
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
	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
	CDragonDOS_FS* fs = (CDragonDOS_FS*)pContext->fs;
	std::vector<std::string> fileNames;

	if( !ChooseFilename( fileNames, "All files\t*.*\n", "*.*", false, false ) )
	{
		return;
	}

	std::string errorStr;

	for( auto file : fileNames )
	{
		FILE* pIn = fopen( file.c_str(), "rb" );
		if( nullptr == pIn )
		{
			errorStr += "Could not open requested file ";
			errorStr += file;
			errorStr += "\n";
			continue;
		}

		fseek( pIn, 0, SEEK_END );
		size_t insertFileSize = ftell( pIn );
		fseek( pIn, 0, SEEK_SET );

		// Check if there's enough room on the disk for the file to be inserted.
		size_t freeBytes = fs->GetFreeSize();
		if( insertFileSize > DRAGONDOS_MAX_FILE_SIZE - DRAGONDOS_FILEHEADER_SIZE )
		{
			fclose( pIn );

			errorStr += "Cannot insert file ";
			errorStr += file;
			errorStr += " ,size is greater than 65535 bytes.";
			continue;
		}
		if( insertFileSize > freeBytes )
		{
			fclose( pIn );

			errorStr += "Not enough room to insert ";
			errorStr += file;
			errorStr += ". Another ";
			errorStr += std::to_string(insertFileSize - freeBytes);
			errorStr += " are needed.\n";
			continue;
		}

		bool hasHeader = GetBinaryFileHeaderParams( pIn, insertFileSize, pContext->loadAddress, pContext->execAddress );

		size_t dataSize = hasHeader ? insertFileSize : insertFileSize+DRAGONDOS_FILEHEADER_SIZE;
		size_t dataStart = hasHeader ? 0 : DRAGONDOS_FILEHEADER_SIZE;
		std::vector<unsigned char> fileData;
		fileData.resize( dataSize );
		fseek( pIn, 0, SEEK_SET );
		size_t bytesRead = fread( fileData.data()+dataStart, 1, insertFileSize, pIn );
		fclose( pIn );

		if( !hasHeader )
		{
			AskLoadAndExecAddresses( pContext->loadAddress, pContext->execAddress );

			// Add file header /////////////////////////////////
			// For BASIC files, load address is usually always 
			// 0x2401 and the exec address 0x8B8D
			////////////////////////////////////////////////////
			fileData[0] = DRAGONDOS_FILE_HEADER_BEGIN;          // Constant
			fileData[1] = DRAGONDOS_FILETYPE_BINARY;            // File type
			fileData[2] = (pContext->loadAddress / 256) & 0xFF; // Load address high byte
			fileData[3] = pContext->loadAddress & 0xFF;         // Load address low byte
			fileData[4] = (insertFileSize / 256) & 0xFF;        // File size high byte
			fileData[5] = insertFileSize & 0xFF;                // File size low byte
			fileData[6] = (pContext->execAddress / 256) & 0xFF; // Exec address high byte
			fileData[7] = pContext->execAddress & 0xFF;         // Exec address low byte
			fileData[8] = DRAGONDOS_FILE_HEADER_END;            // Constant
			////////////////////////////////////////////////////
		}

		std::filesystem::path filePath( file );
		fs->InsertFile( filePath.filename().string(), fileData, true );
	}

	if( !errorStr.empty() )
	{
		fl_alert( "%s", errorStr.c_str() );
	}

	pContext->fs->Load( pContext->disk );
	UpdateUI( pContext );
}

void insertData_cb(Fl_Widget* pWidget,void* _context)
{
	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
	CDragonDOS_FS* fs = (CDragonDOS_FS*)pContext->fs;
	std::vector<std::string> fileNames;

	if( !ChooseFilename( fileNames, "All files\t*.*\n", "*.*", false, false ) )
	{
		return;
	}

	std::string errorStr;

	for( auto file : fileNames )
	{
		FILE* pIn = fopen( file.c_str(), "rb" );
		if( nullptr == pIn )
		{
			errorStr += "Could not open requested file ";
			errorStr += file;
			errorStr += "\n";
			continue;
		}

		fseek( pIn, 0, SEEK_END );
		size_t insertFileSize = ftell( pIn );
		fseek( pIn, 0, SEEK_SET );

		// Check if there's enough room on the disk for the file to be inserted.
		size_t freeBytes = fs->GetFreeSize();
		if( insertFileSize > DRAGONDOS_MAX_FILE_SIZE - DRAGONDOS_FILEHEADER_SIZE )
		{
			fclose( pIn );

			errorStr += "Cannot insert file ";
			errorStr += file;
			errorStr += " ,size is greater than 65535 bytes.";
			continue;
		}
		if( insertFileSize > freeBytes )
		{
			fclose( pIn );

			errorStr += "Not enough room to insert ";
			errorStr += file;
			errorStr += ". Another ";
			errorStr += std::to_string(insertFileSize - freeBytes);
			errorStr += " are needed.\n";
			continue;
		}

		std::vector<unsigned char> fileData;
		fileData.resize( insertFileSize );
		size_t bytesRead = fread( fileData.data(), 1, insertFileSize, pIn );
		fclose( pIn );

		std::filesystem::path filePath( file );
		fs->InsertFile( filePath.filename().string(), fileData, true );
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
	CDragonDOS_FS* fs = (CDragonDOS_FS*)pContext->fs;
	std::string path;

	if( pContext->browser->size() < DRAGONDOSUI_BROWSER_LINE_OFFSET )
	{
		return;
	}

	if( !ChooseFilename( path, true, true, pContext ) )
	{
		return;
	}

	std::string errors;

	for( int line = DRAGONDOSUI_BROWSER_LINE_OFFSET; line <= pContext->browser->size(); ++line  )
	{
		if( pContext->browser->selected(line) )
		{
			SFileInfo fi = fs->GetFileInfo( line - DRAGONDOSUI_BROWSER_LINE_OFFSET );
			std::vector<unsigned char> fileData;
			std::string fileName = path;
			fileName += DRAGONDOSUI_PATH_SEPARATOR;
			fileName += fi.name;

			bool extractBinaryHeaders = (pContext->extractBinaryHeadersButton != NULL) && (0 != pContext->extractBinaryHeadersButton->value());
			if( !fs->ExtractFile(fi.name, fileData, extractBinaryHeaders ) )
			{
				errors += "Couldn't extract file ";
				errors += fi.name;
				errors += "\n";
				continue;
			}
			
			unsigned short int fileIdx = fs->GetFileEntry( fi.name );
			if( fileIdx == DRAGONDOS_INVALID )
			{
				continue;
			}

			size_t bytesWritten = 0;
			FILE* pOut = fopen( fileName.c_str(), "wb" );
			if( nullptr == pOut )
			{
				errors += "Error writing file ";
				errors += fileName;
				errors += "\n";
				continue;
			}

			// Detokenize BASIC
			SDGNDosDirectoryEntry entry = fs->GetDirectory()[fileIdx];
			if( entry.fileType == DRAGONDOS_FILETYPE_BASIC  )
			{
				std::stringstream strStream;
				std::stringstream clrStream;
				std::string textColors;
				unsigned short int programStart = DRAGONDOS_BASIC_PROGRAM_START;

				DragonDOS_BASIC::Decode( fileData, strStream, textColors, programStart, false, false );

				bytesWritten = fwrite( strStream.str().c_str(), 1, strStream.str().length(), pOut );
				fclose( pOut );
			}
			else
			{
				size_t bytesWritten = fwrite( fileData.data(), 1, fileData.size(), pOut );
				fclose( pOut );

				if( bytesWritten != fileData.size() )
				{
					errors += "Error writing file ";
					errors += fileName;
					errors += " ";
					errors += std::to_string(bytesWritten);
					errors += " of ";
					errors += std::to_string(fileData.size());
					errors += " bytes written.\n";
					continue;
				}
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
	IFileSystemInterface* fs = (IFileSystemInterface*)pContext->fs;

	std::vector<std::string> fileNames;

	for( int line = DRAGONDOSUI_BROWSER_LINE_OFFSET; line <= pContext->browser->size(); ++line  )
	{
		if( pContext->browser->selected(line) )
		{
			fileNames.push_back( fs->GetFileName(line - DRAGONDOSUI_BROWSER_LINE_OFFSET) );
		}
	}

	for( auto fileName : fileNames )
	{
		if( !fs->DeleteFile( fileName) )
		{
			std::string error = "Couldn't remove file ";
			error += fileName;
			error += " , stopping operation.";
			fl_alert( "%s", error.c_str() );
			UpdateUI( pContext );
			return;
		}
	}

	fs->Load( pContext->disk );
	UpdateUI( pContext );
}

void viewFiles_cb(Fl_Widget* pWidget,void* _context)
{
	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;
	CDragonDOS_FS* fs = (CDragonDOS_FS*)pContext->fs;

	std::stringstream decodedFiles;
	std::string fltkTextColors;
	std::vector<int> selectedFiles;

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
			std::vector<unsigned char> file;
			fs->ExtractFile( fs->GetFileName(line - DRAGONDOSUI_BROWSER_LINE_OFFSET), file, false );

			unsigned short int programStart = DRAGONDOS_BASIC_PROGRAM_START;
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

void viewFileAsDisassembly_cb( Fl_Widget* pWidget, void* _viewFileWindow )
{
    if( _viewFileWindow )
    {
        ((CDragonDOSViewFileWindow*)_viewFileWindow)->ViewAsDisassembly();
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

void newDiskWindowOK_cb(Fl_Widget* pWidget,void* _context)
{
	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;

	pContext->newDiskWindow->hide();

	int sidesNum = 0;
	int tracksNum = 0;

	switch( pContext->newDiskWindow->GetSelectedDiskGeometry() )
	{
	case NDW_GEOMETRY_1S40T: sidesNum = 1; tracksNum = 40; break;
	case NDW_GEOMETRY_1S80T: sidesNum = 1; tracksNum = 80; break;
	case NDW_GEOMETRY_2S40T: sidesNum = 2; tracksNum = 40; break;
	case NDW_GEOMETRY_2S80T: sidesNum = 2; tracksNum = 80; break;
	default: return; break;
	}

	std::string fileName;
	if( !ChooseFilename( fileName, true, false, pContext ) )
	{
		return;
	}

	pContext->diskFilename = "(No disk)";
	UpdateUI( pContext );

	// Create new disk
	int imageFormat = pContext->newDiskWindow->GetSelectedImageType();
	if( imageFormat <= NDW_IMAGE_TYPE_INVALID || imageFormat >= pContext->diskImageFactory->Size() )
	{
		return;
	}

	IDiskImageInterface* diskImage = pContext->diskImageFactory->GetDiskImage( imageFormat );
	if( diskImage == nullptr )
	{
		return;
	}

	pContext->disk = diskImage->NewImage();
	if( pContext->disk == nullptr )
	{
		return;
	}

	if( 0 == pContext->disk->New( tracksNum, sidesNum, VDK_SECTORSPERTRACK, VDK_SECTORSIZE ) )
	{
		return;
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

void newDiskWindowCancel_cb(Fl_Widget* pWidget,void* _context)
{
	SDRAGONDOS_Context* pContext = (SDRAGONDOS_Context*)_context;

	pContext->newDiskWindow->hide();
}

void processor6809_cb(Fl_Widget* pWidget,void* _viewFileWindow)
{
	if( _viewFileWindow )
	{
		((CDragonDOSViewFileWindow*)_viewFileWindow)->Show6809Disassembly();
	}
}

void processor6309_cb(Fl_Widget* pWidget,void* _viewFileWindow)
{
	if( _viewFileWindow )
	{
		((CDragonDOSViewFileWindow*)_viewFileWindow)->Show6309Disassembly();
	}
}
