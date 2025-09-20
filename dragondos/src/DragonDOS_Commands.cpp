#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdlib.h>

#include "DragonDOS_BASIC.h"
#include "DragonDOS_Commands.h"
#include "DragonDOS_Common.h"
#include "../../common/FileSystems/DragonDOS_FS.h"
#include "../../common/DiskImages/DiskImageInterface.h"
#include "../../common/FileSystems/FileSystemInterface.h"
#include "../../common/DiskImages/VDKDiskImage.h"
#include "../../common/DiskImages/RawDiskImage.h"

std::string PadFilename( const std::string& _name )
{
	std::string retVal = _name;
	retVal.append( DRAGONDOS_MAX_FILE_FULL_NAME_LEN - retVal.length(), ' ' );

	return retVal;
}

bool LoadImageAndFilesystem( std::string _filename, IDiskImageInterface* &_img, IFileSystemInterface* _fs, DiskImageFactory& _diskFactory )
{
	// Load disk image and set disk geometry
	_img = _diskFactory.LoadDiskImage( _filename );
	if( _img == nullptr )
	{
		std::cout << "Failed to load " << _filename << std::endl;
		return false;
	}

	// Initialize filesystem
	if( !_fs->Load( _img ) )
	{
		std::cout << "Unable to initialize " << _fs->GetFSName() << " file system from " << _filename << std::endl;
		delete _img;
		_img = nullptr;
		return false;
	}

	return true;
}

bool HelpCommand()
{
	std::cout << "usage: dragondos <command> [<args> ...]" << std::endl << std::endl;
	std::cout << "A command-line tool for managing DragonDOS disk images." << std::endl << std::endl;
	std::cout << "Commands:" << std::endl;
	std::cout << "\thelp" << std::endl << "\t  Displays this help text." << std::endl << std::endl;
	std::cout << "\tinfo <filename>" << std::endl << "\t  Displays disk image information like type, free space and number of files." << std::endl << std::endl;
	std::cout << "\tlist <filename>" << std::endl;
	std::cout << "\t  Displays a list of the files on a disk image and the following data:" << std::endl;
	std::cout << "\t  Index, file name, size in sectors and size in bytes." << std::endl << std::endl;
	std::cout << "\textract <filename> [<index>] [-strip_binary_header]" << std::endl;
	std::cout << "\t  Extracts all files from within the disk image to the current directory." << std::endl;
	std::cout << "\t  If an optional file index is specified, only that file will be extracted to" << std::endl;
	std::cout << "\t  current directory." << std::endl;
	std::cout << "\t  If the optional flag -strip_binary_header is specified, binary type files" << std::endl;
	std::cout << "\t  will be extracted without the DragonDOS header." << std::endl << std::endl;
	std::cout << "\tnew <filename> <size> [<image_type>] [<sides_num>]" << std::endl;
	std::cout << "\t  Creates a new, formatted disk image." << std::endl;
	std::cout << "\t  Size is the size of the new disk image in kilobytes." << std::endl;
	std::cout << "\t  Valid values are 180, 360 and 720." << std::endl << std::endl;
	std::cout << "\t  Optionally, you can specify a disk image format using the desired" << std::endl;
	std::cout << "\t  format index from the 'listimages' command." << std::endl;
	std::cout << "\t  The default image type is .VDK." << std::endl;
	std::cout << "\t  If you are creating a 360KB disk, you can specify the number of sides" << std::endl;
	std::cout << "\t  (1 or 2) after the disk image format." << std::endl;
	std::cout << "\t  The default number of sides is 1." << std::endl << std::endl;
	std::cout << "\tdelete <filename> <index>" << std::endl;
	std::cout << "\t  Deletes a file from the disk image." << std::endl;
	std::cout << "\t  A backup copy of the disk image will be created." << std::endl << std::endl;
	std::cout << "\tinsertBasic <image filename> <filename to insert>" << std::endl;
	std::cout << "\t  Inserts and tokenizes a BASIC file into the requested disk image." << std::endl << std::endl;
	std::cout << "\tinsertBinary <image filename> <filename to insert> [load_address] [exec_address]" << std::endl;
	std::cout << "\t  Inserts a binary file into the requested disk image." << std::endl << std::endl;
	std::cout << "\tinsertData <image filename> <filename to insert>" << std::endl;
	std::cout << "\t  Inserts a data file into the requested disk image." << std::endl << std::endl;
	std::cout << "\tlistimages" << std::endl;
	std::cout << "\t  Displays a list of the available disk image formats and their" << std::endl;
	std::cout << "\t  indices, to be used with the commands that require them." << std::endl << std::endl;

	return true;
}

bool ListCommand( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory )
{
	// Check arguments
	if( _args.size() < 3 )
	{
		std::cout << "The List command requires a filename." << std::endl << std::endl;
		std::cout << "Example:" << std::endl;
		std::cout << "\tdragondos list mydisk.vdk" << std::endl << std::endl;

		HelpCommand();

		return false;
	}

	// Load disk image and initialize file system
	IDiskImageInterface* img;
	CDragonDOS_FS fs;

	if( !LoadImageAndFilesystem( _args[2], img, &fs, _diskFactory ) )
	{
		return false;
	} 

	// Display file list
	for( size_t fileIdx = 0; fileIdx < fs.GetFilesNum(); ++fileIdx )
	{
		CDGNDosFile ddosFile = fs.GetFile((unsigned short int)fileIdx);
		SFileInfo fi = fs.GetFileInfo( fileIdx );
		std::cout << fileIdx << "\t" << PadFilename(fi.name) << "\t" <<  fs.GetFileSize(fileIdx) << std::hex;
		std::cout << "\tLoad: 0x" << ddosFile.GetLoadAddress() << "\tExec: 0x" << ddosFile.GetExecAddress() << std::dec << std::endl;
	}
	
	return true;
}

bool ExtractCommand( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory )
{
	// Check arguments
	if( _args.size() < 3 )
	{
		std::cout << "The Extract command requires a disk image file name." << std::endl;
		std::cout << "Optionally, a file index (as shown by the list command) and/or" << std::endl;
		std::cout << "the -strip_binary_header flag (to extract the raw binary file data" << std::endl;
		std::cout << "without the DragonDOS header) can be specified." << std::endl << std::endl;
		std::cout << "Examples:" << std::endl;
		std::cout << "\tdragondos extract mydisk.vdk                        (extracts all files)" << std::endl;
		std::cout << "\tdragondos extract mydisk.vdk 7                      (extracts file 7 from the file list)" << std::endl << std::endl;
		std::cout << "\tdragondos extract mydisk.vdk 7 -strip_binary_header (extracts file 7 from the file list" << std::endl;
		std::cout << "\t                                                     without the DragonDOS header if" << std::endl;
		std::cout << "\t                                                     it's a binary file)" << std::endl << std::endl;

		HelpCommand();

		return false;
	}

	// Load disk image and initialize file system
	IDiskImageInterface* img;
	CDragonDOS_FS fs;

	if( !LoadImageAndFilesystem( _args[2], img, &fs, _diskFactory ) )
	{
		return false;
	} 

	size_t start = 0;
	size_t end = fs.GetFilesNum();
	bool withBinaryHeader = true;

	// Parse extra arguments
	if( _args.size() > 3 )
	{
		for( size_t extraArg = 3; extraArg < _args.size(); ++extraArg )
		{
			std::string arg = _args[extraArg];
			std::transform( arg.begin(), arg.end(), arg.begin(), ::tolower );
			if( 0 == arg.compare("-strip_binary_header") )
			{
				withBinaryHeader = false;
			}
			else
			{
				start = atoi(arg.c_str());
				end = start + 1;
			}
		}
	}

	if( start >= fs.GetFilesNum() || end > fs.GetFilesNum() )
	{
		std::cout << "Specified index is out of range." << std::endl << std::endl;
		std::cout << " The valid range for this image is 0 to " << fs.GetFilesNum() - 1 << "." << std::endl;
		delete img;
		return false;
	}

	for( size_t fileIdx = start; fileIdx < end; ++fileIdx )
	{
		std::vector<unsigned char> fileData;
		SFileInfo fi = fs.GetFileInfo( fileIdx );

		if( !fs.ExtractFile( fi.name, fileData, withBinaryHeader ) )
		{
			std::cout << "The requested file couldn't be found. The disk image may be damaged or corrupted." << std::endl;
			delete img;
			return false;
		}

		FILE* pOut = fopen( fi.name.c_str(), "wb" );
		if( nullptr == pOut )
		{
			std::cout << "Error creating file " << fi.name << std::endl;
			delete img;
			return false;
		}

		// Detokenize BASIC
		size_t bytesWritten = 0;
		SDGNDosDirectoryEntry entry = fs.GetDirectory()[fileIdx];
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
				std::cout << "Error writing file " << fi.name.c_str() << " " << bytesWritten;
				std::cout << " of " << fileData.size() << " bytes written." << std::endl;
				continue;
			}
		}

		fclose( pOut );

		if( bytesWritten != fileData.size() )
		{
			std::cout << "Error writing file " << fi.name << " " << bytesWritten << " of " << fileData.size() << " bytes written." << std::endl;
			delete img;
			return false;
		}

		std::cout << fileIdx << "\t" << PadFilename(fi.name) << "\t" << bytesWritten << " of " << fileData.size() << " bytes written." << std::endl;
	}

	delete img;
	return true;
}

bool InfoCommand( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory )
{
	// Check arguments
	if( _args.size() < 3 )
	{
		std::cout << "The Info command requires a filename." << std::endl << std::endl;
		std::cout << "Examples:" << std::endl;
		std::cout << "\tdragondos info mydisk.vdk" << std::endl << std::endl;

		HelpCommand();

		return false;
	}

	// Load disk image and initialize file system
	IDiskImageInterface* img;
	CDragonDOS_FS fs;

	if( !LoadImageAndFilesystem( _args[2], img, &fs, _diskFactory ) )
	{
		return false;
	} 

	size_t sides = img->GetSidesNum();
	size_t tracks = img->GetTracksNum();
	size_t sectors = img->GetSectorsNum();
	size_t sectorSize = img->GetSectorSize();
	size_t totalBytes = sides * tracks * sectors * sectorSize;

	std::cout << sides            << " sides"             << std::endl;
	std::cout << tracks           << " tracks"            << std::endl;
	std::cout << sectors          << " sectors per track" << std::endl;
	std::cout << sectorSize       << " bytes per sector"  << std::endl;
	std::cout << totalBytes       << " total bytes"       << std::endl;
	std::cout << fs.GetFreeSize() << " free bytes"        << std::endl;
	std::cout << fs.GetFSName()   << " file system"       << std::endl;
	std::cout << fs.GetFilesNum() << " files"             << std::endl;

	delete img;
	return true;
}

bool NewCommand( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory )
{
	// Check arguments
	if( _args.size() < 4 )
	{
		std::cout << "The New command requires a filename and a disk size (180,360 or 720)." << std::endl;
		std::cout << "Optionally, you can specify a disk image format using the desired" << std::endl;
		std::cout << "format index from the 'listimages' command." << std::endl;
		std::cout << "The default image type is .VDK." << std::endl;
		std::cout << "If you are creating a 360KB disk, you can specify the number of sides" << std::endl;
		std::cout << "(1 or 2) after the disk image format." << std::endl;
		std::cout << "The default number of sides is 1." << std::endl;
		std::cout << "Examples:" << std::endl;
		std::cout << "\tdragondos new blank.vdk 180     (1 side,  40 tracks, 180KB .VDK file)" << std::endl;
		std::cout << "\tdragondos new blank.imd 720 2   (2 sides, 80 tracks, 720KB .IMD file)" << std::endl;
		std::cout << "\tdragondos new blank.jvc 360 1 2 (2 sides, 40 tracks, 360KB .JVC file)" << std::endl;

		HelpCommand();

		return false;
	}

	// Read optional disk image format to create.
	size_t imageType = 0;
	if( _args.size() >= 5 )
	{
		imageType = stoi(_args[4]);

		if( imageType >= _diskFactory.Size() )
		{
			std::cout << "Invalid disk format index (" << imageType << ")" << std::endl;
			std::cout << "Valid values are from 0 to " << _diskFactory.Size() - 1 << std::endl << std::endl;

			HelpCommand();

			return false;
		}
	}

	// Read optional number of sides.
	size_t sidesNum = 1;
	if( _args.size() >= 6 )
	{
		sidesNum = stoi(_args[5]);
		if( sidesNum < 1 || sidesNum > 2 )
		{
			std::cout << "Invalid number of sides (" << sidesNum << ")." << std::endl;
			std::cout << "Valid values are 1 or 2." << std::endl << std::endl;

			HelpCommand();

			return false;
		}
	}
	size_t tracksNum = (sidesNum == 1 ? 80 : 40);

	IDiskImageInterface* img = _diskFactory.GetDiskImage( imageType )->NewImage(); 
	CDragonDOS_FS fs;


	// Create new disk image
	int disk_size = stoi(_args[3]);
	switch( disk_size )
	{
		case 180:img->New( 40			, 1 		, VDK_SECTORSPERTRACK, VDK_SECTORSIZE );break;
		case 360:img->New( tracksNum	, sidesNum	, VDK_SECTORSPERTRACK, VDK_SECTORSIZE );break;
		case 720:img->New( 80			, 2 		, VDK_SECTORSPERTRACK, VDK_SECTORSIZE );break;
		default:
		{
			std::cout << "Invalid disk size (" << disk_size << ")." << std::endl;
			std::cout << "Please use either 180, 360 or 720 as size values." << std::endl << std::endl;

			HelpCommand();

			delete img;
			return false;
		}
	}

	// Initialize filesystem
	if( !fs.InitDisk( img ) )
	{
		std::cout << "Couldn't initialize the file system on the new disk image." << std::endl << std::endl;
		delete img;
		return false;
	}

	// Save image
	if( !img->Save(_args[2]) )
	{
		std::cout << "Couldn't save new image file " << _args[2] << std::endl << std::endl;
		delete img;
		return false;
	}

	delete img;
	return true;
}

bool DeleteCommand( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory )
{
	// Check arguments
	if( _args.size() < 4 )
	{
		std::cout << "The Delete command requires a filename and a file index." << std::endl << std::endl;
		std::cout << "Examples:" << std::endl;
		std::cout << "\tdragondos delete mydisk.vdk 15" << std::endl << std::endl;

		HelpCommand();

		return false;
	}

	// Load disk image and initialize file system
	IDiskImageInterface* img;
	CDragonDOS_FS fs;

	if( !LoadImageAndFilesystem( _args[2], img, &fs, _diskFactory ) )
	{
		return false;
	}
	
	size_t fileIdx = atoi( _args[3].c_str() );
	if( fileIdx >= fs.GetFilesNum() )
	{
		std::cout << "Specified index is out of range." << std::endl << std::endl;
		std::cout << " The valid range for this image is 0 to " << fs.GetFilesNum() - 1 << "." << std::endl;
		delete img;
		return false;
	}

	// Create backup file
	std::string backupFilename = _args[2];
	backupFilename += ".bak";
	if( !fs.GetDisk()->Save( backupFilename ) )
	{
		std::cout << "Could not create backup file " << backupFilename << std::endl;
		delete img;
		return false;
	}

	SFileInfo fi = fs.GetFileInfo( fileIdx );

	if( !fs.DeleteFile( fi.name ) )
	{
		std::cout << "The requested file couldn't be deleted. Please check that the file name is correct." << std::endl;
		std::cout << "The disk image may be damaged or corrupted." << std::endl;
		delete img;
		return false;
	}

	// Save modified disk image
	if( !fs.GetDisk()->Save( _args[2] ) )
	{
		std::cout << "Could not overwrite file " << _args[2] << std::endl;
		delete img;
		return false;
	}

	delete img;
	return true;
}

bool InsertBasicCommand( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory )
{
	// Check arguments
	if( _args.size() < 4 )
	{
		std::cout << "The InsertBasic command requires a disk image filename and the filename of the file to insert." << std::endl;
		std::cout << "The file to be inserted has to be in plain ASCII text format." << std::endl << std::endl;
		std::cout << "Examples:" << std::endl;
		std::cout << "\tdragondos insertbasic mydisk.vdk program.bas" << std::endl << std::endl;

		HelpCommand();

		return false;
	}

	// Load disk image and initialize file system
	IDiskImageInterface* img;
	CDragonDOS_FS fs;

	if( !LoadImageAndFilesystem( _args[2], img, &fs, _diskFactory ) )
	{
		return false;
	}

	// Create backup file
	std::string backupFilename = _args[2];
	backupFilename += ".bak";
	if( !fs.GetDisk()->Save( backupFilename ) )
	{
		std::cout << "Could not create backup file " << backupFilename << std::endl;
		delete img;
		return false;
	}

	// Load file to be inserted
	std::string insertFilename = _args[3];
	std::vector<char> fileData;

	FILE* pIn = fopen( insertFilename.c_str(), "rb" );
	if( nullptr == pIn )
	{
		std::cout << "Could not open requested file " << insertFilename.c_str() << std::endl;
		delete img;
		return false;
	}

	fseek( pIn, 0, SEEK_END );
	size_t insertFileSize = ftell( pIn );
	fseek( pIn, 0, SEEK_SET );

	fileData.resize( insertFileSize );
	size_t bytesRead = fread( fileData.data(), 1, insertFileSize, pIn );
	fclose( pIn );

	if( bytesRead < insertFileSize )
	{
		std::cout << "Error loading file " << insertFilename.c_str() << " (" << bytesRead;
		std::cout << insertFileSize << " bytes read)." << std::endl;
		delete img;
		return false;
	}

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

	// Check if there's enough room on the disk for the file to be inserted.
	size_t freeBytes = fs.GetFreeSize();
	if( encodedData.size() > freeBytes )
	{
		fclose( pIn );

		std::cout << "Not enough room to insert " << insertFilename << ". Another " << encodedData.size() - freeBytes << " are needed." << std::endl;
		delete img;
		return false;
	}

	std::filesystem::path filePath( _args[3] );

	if( !fs.InsertFile( filePath.filename().string(), encodedData, true ) )
	{
		std::cout << "The requested file couldn't be inserted. Please check that the file name is correct." << std::endl;
		std::cout << "The disk image may not have enough free space or be damaged or corrupted." << std::endl;
		delete img;
		return false;
	}

	// Save modified disk image
	if( !fs.GetDisk()->Save( _args[2] ) )
	{
		std::cout << "Could not overwrite file " << _args[2] << std::endl;
		delete img;
		return false;
	}

	delete img;
	return true;
}

bool InsertBinaryCommand( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory )
{
	// Check arguments
	if( _args.size() < 4 )
	{
		std::cout << "The InsertBinary command requires a disk image filename, the filename of the file to insert." << std::endl;
		std::cout << "If the file has the DragonDOS header, it will automatically get the Load and Exec addresses from it."  << std::endl;
		std::cout << "In case the file doesn't contain the DragonDOS header, or you want to override them, the Load and"  << std::endl;
		std::cout << "Exec addresses can be added at the end of the arguments list in decimal or hex." << std::endl << std::endl;
		std::cout << "Examples:" << std::endl;
		std::cout << "\tdragondos insertbinary mydisk.vdk file_with_header.bin" << std::endl;
		std::cout << "\tdragondos insertbinary mydisk.vdk file.bin 3072 0xC00" << std::endl << std::endl;

		HelpCommand();

		return false;
	}

	// Load disk image and initialize file system
	IDiskImageInterface* img;
	CDragonDOS_FS fs;

	if( !LoadImageAndFilesystem( _args[2], img, &fs, _diskFactory ) )
	{
		return false;
	}

	// Create backup file
	std::string backupFilename = _args[2];
	backupFilename += ".bak";
	if( !fs.GetDisk()->Save( backupFilename ) )
	{
		std::cout << "Could not create backup file " << backupFilename << std::endl;
		delete img;
		return false;
	}

	// Load file to be inserted
	std::string insertFilename = _args[3];
	std::vector<unsigned char> fileData;

	FILE* pIn = fopen( insertFilename.c_str(), "rb" );
	if( nullptr == pIn )
	{
		std::cout << "Could not open requested file " << insertFilename.c_str() << std::endl;
		delete img;
		return false;
	}

	fseek( pIn, 0, SEEK_END );
	size_t insertFileSize = ftell( pIn );
	fseek( pIn, 0, SEEK_SET );

	unsigned short int loadAddress = 0;
	unsigned short int execAddress = 0;
	bool hasHeader = GetBinaryFileHeaderParams( pIn, insertFileSize, loadAddress, execAddress );

	size_t dataSize = hasHeader ? insertFileSize : insertFileSize+DRAGONDOS_FILEHEADER_SIZE;
	size_t dataStart = hasHeader ? 0 : DRAGONDOS_FILEHEADER_SIZE;
	fileData.resize( dataSize );
	fseek( pIn, 0, SEEK_SET );
	size_t bytesRead = fread( fileData.data()+dataStart, 1, insertFileSize, pIn );
	fclose( pIn );

	if( bytesRead < insertFileSize )
	{
		std::cout << "Error loading file " << insertFilename.c_str() << " (" << bytesRead;
		std::cout << insertFileSize << " bytes read)." << std::endl;
		delete img;
		return false;
	}

	// Check if there's enough room on the disk for the file to be inserted.
	size_t freeBytes = fs.GetFreeSize();
	if( dataSize > freeBytes )
	{
		fclose( pIn );

		std::cout << "Not enough room to insert " << insertFilename << ". Another " << dataSize - freeBytes << " are needed." << std::endl;
		delete img;
		return false;
	}

	// Get or override Load address
	if( _args.size() > 4 )
	{
		unsigned long int parsedLoadAddress = strtoul( _args[4].c_str(), nullptr, 0 );
		if( parsedLoadAddress > 0xFFFF )
		{
			std::cout << "Out of range load address " << parsedLoadAddress << std::endl << "Valid range is 0 to 65535 (0xFFFF)" << std::endl;
			delete img;
			return false;
		}
		loadAddress = (unsigned short int)parsedLoadAddress;
	}

	// Get or override Load address
	if( _args.size() > 5 )
	{
		unsigned long int parsedExecAddress = strtoul( _args[5].c_str(), nullptr, 0 );
		if( parsedExecAddress > 0xFFFF )
		{
			std::cout << "Out of range exec address " << parsedExecAddress << std::endl << "Valid range is 0 to 65535 (0xFFFF)" << std::endl;
			delete img;
			return false;
		} 
		execAddress = (unsigned short int)parsedExecAddress;
	}

	// Add or update file header ///////////////////////
	// For BASIC files, load address is usually always 
	// 0x2401 and the exec address 0x8B8D
	////////////////////////////////////////////////////
	fileData[0] = DRAGONDOS_FILE_HEADER_BEGIN;      // Constant
	fileData[1] = DRAGONDOS_FILETYPE_BINARY;        // File type
	fileData[2] = (loadAddress / 256) & 0xFF;       // Load address high byte
	fileData[3] = loadAddress & 0xFF;               // Load address low byte
	fileData[4] = (insertFileSize / 256) & 0xFF;    // File size high byte
	fileData[5] = insertFileSize & 0xFF;            // File size low byte
	fileData[6] = (execAddress / 256) & 0xFF;       // Exec address high byte
	fileData[7] = execAddress & 0xFF;               // Exec address low byte
	fileData[8] = DRAGONDOS_FILE_HEADER_END;        // Constant
	////////////////////////////////////////////////////

	std::filesystem::path filePath( _args[3] );

	if( !fs.InsertFile( filePath.filename().string(), fileData, true ) )
	{
		std::cout << "The requested file couldn't be inserted. Please check that the file name is correct." << std::endl;
		std::cout << "The disk image may not have enough free space or be damaged or corrupted." << std::endl;
		delete img;
		return false;
	}

	// Save modified disk image
	if( !fs.GetDisk()->Save( _args[2] ) )
	{
		std::cout << "Could not overwrite file " << _args[2] << std::endl;
		delete img;
		return false;
	}

	delete img;
	return true;
}

bool InsertDataCommand( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory )
{
	// Check arguments
	if( _args.size() < 4 )
	{
		std::cout << "The InsertData command requires a disk image filename and the filename of the file to insert" << std::endl << std::endl;
		std::cout << "Examples:" << std::endl;
		std::cout << "\tdragondos insertdata mydisk.vdk file.dat" << std::endl << std::endl;

		HelpCommand();

		return false;
	}

	// Load disk image and initialize file system
	IDiskImageInterface* img;
	CDragonDOS_FS fs;

	if( !LoadImageAndFilesystem( _args[2], img, &fs, _diskFactory ) )
	{
		return false;
	}

	// Create backup file
	std::string backupFilename = _args[2];
	backupFilename += ".bak";
	if( !fs.GetDisk()->Save( backupFilename ) )
	{
		std::cout << "Could not create backup file " << backupFilename << std::endl;
		delete img;
		return false;
	}

	// Load file to be inserted
	std::string insertFilename = _args[3];
	std::vector<unsigned char> fileData;

	FILE* pIn = fopen( insertFilename.c_str(), "rb" );
	if( nullptr == pIn )
	{
		std::cout << "Could not open requested file " << insertFilename.c_str() << std::endl;
		delete img;
		return false;
	}

	fseek( pIn, 0, SEEK_END );
	size_t insertFileSize = ftell( pIn );
	fseek( pIn, 0, SEEK_SET );

	// Check if there's enough room on the disk for the file to be inserted.
	size_t freeBytes = fs.GetFreeSize();
	if( insertFileSize > freeBytes )
	{
		fclose( pIn );

		std::cout << "Not enough room to insert " << insertFilename << ". Another " << insertFileSize - freeBytes << " are needed." << std::endl;
		delete img;
		return false;
	}

	fileData.resize( insertFileSize );
	size_t bytesRead = fread( fileData.data(), 1, insertFileSize, pIn );
	fclose( pIn );

	if( bytesRead < insertFileSize )
	{
		std::cout << "Error loading file " << insertFilename.c_str() << " (" << bytesRead;
		std::cout << insertFileSize << " bytes read)." << std::endl;
		delete img;
		return false;
	}

	std::filesystem::path filePath( _args[3] );

	if( !fs.InsertFile( filePath.filename().string(), fileData, true ) )
	{
		std::cout << "The requested file couldn't be inserted. Please check that the file name is correct." << std::endl;
		std::cout << "The disk image may not have enough free space or be damaged or corrupted." << std::endl;
		delete img;
		return false;
	}

	// Save modified disk image
	if( !fs.GetDisk()->Save( _args[2] ) )
	{
		std::cout << "Could not overwrite file " << _args[2] << std::endl;
		delete img;
		return false;
	}

	delete img;
	return true;
}

bool ListImagesCommand( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory )
{
	for( size_t imageIdx = 0; imageIdx < _diskFactory.Size(); ++imageIdx )
	{
		std::cout << imageIdx << " : " << _diskFactory.GetDiskImage(imageIdx)->GetFileSpec();
	}

	return true;
}
