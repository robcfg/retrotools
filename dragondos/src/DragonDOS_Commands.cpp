#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdlib.h>

#include "DragonDOS_Commands.h"
#include "DragonDOS_BASIC.h"
#include "DragonDOS_FS.h"
#include "DiskImageInterface.h"
#include "FileSystemInterface.h"
#include "VDKDiskImage.h"
#include "RawDiskImage.h"

std::string PadFilename( const std::string& _name )
{
    std::string retVal = _name;
    retVal.append( DRAGONDOS_MAX_FILE_FULL_NAME_LEN - retVal.length(), ' ' );

    return retVal;
}

bool LoadImageAndFilesystem( std::string _filename, IDiskImageInterface* _img, IFilesystemInterface* _fs )
{
    // Load disk image and set disk geometry
    if( !_img->Load( _filename ) )
    {
        std::cout << "Failed to load " << _filename << std::endl;
        return false;
    }

    // Initialize filesystem
    if( !_fs->Load( _img ) )
    {
        std::cout << "Unable to initialize " << _fs->GetFSName() << " file system from " << _filename << std::endl;
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
    std::cout << "\textract <filename> [<index>]" << std::endl;
    std::cout << "\t  Extracts all files from within the disk image to the current directory." << std::endl;
    std::cout << "\t  If an optional file index is specified, only that file will be extracted to" << std::endl;
    std::cout << "\t  current directory." << std::endl << std::endl;
    std::cout << "\tnew <filename> <size>" << std::endl;
    std::cout << "\t  Creates a new, formatted disk image." << std::endl;
    std::cout << "\t  Size is the size of the new disk image in kilobytes." << std::endl;
    std::cout << "\t  Valid values are 180, 360 and 720." << std::endl << std::endl;
    std::cout << "\tdelete <filename> <index>" << std::endl;
    std::cout << "\t  Deletes a file from the disk image." << std::endl;
    std::cout << "\t  A backup copy of the disk image will be created." << std::endl << std::endl;
    std::cout << "\tinsertBasic <image filename> <filename to insert>" << std::endl;
    std::cout << "\t  Inserts and tokenizes a BASIC file into the requested disk image." << std::endl << std::endl;
    std::cout << "\tinsertBinary <image filename> <filename to insert> <load_address> <exec_address>" << std::endl;
    std::cout << "\t  Inserts a binary file into the requested disk image." << std::endl << std::endl;
    std::cout << "\tinsertData <image filename> <filename to insert>" << std::endl;
    std::cout << "\t  Inserts a data file into the requested disk image." << std::endl << std::endl;

    return true;
}

bool ListCommand( const std::vector<std::string>& _args )
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
    CVDKDiskImage img;
    CDragonDOS_FS fs;

    if( !LoadImageAndFilesystem( _args[2], &img, &fs ) )
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

bool ExtractCommand( const std::vector<std::string>& _args )
{
    // Check arguments
    if( _args.size() < 3 )
    {
        std::cout << "The Extract command requires a filename and an optional index." << std::endl << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "\tdragondos extract mydisk.vdk   (extracts all files)" << std::endl;
        std::cout << "\tdragondos extract mydisk.vdk 7 (extracts file 7 from the file list)" << std::endl << std::endl;

        HelpCommand();

        return false;
    }

    // Load disk image and initialize file system
    CVDKDiskImage img;
    CDragonDOS_FS fs;

    if( !LoadImageAndFilesystem( _args[2], &img, &fs ) )
    {
        return false;
    } 

    size_t start = ((_args.size() > 3) ? atoi(_args[3].c_str()) : 0);
    size_t end   = ((_args.size() > 3) ? (start + 1)   : fs.GetFilesNum());

    if( start >= fs.GetFilesNum() || end > fs.GetFilesNum() )
    {
        std::cout << "Specified index is out of range." << std::endl << std::endl;
        std::cout << " The valid range for this image is 0 to " << fs.GetFilesNum() - 1 << "." << std::endl;
        return false;
    }

    for( size_t fileIdx = start; fileIdx < end; ++fileIdx )
    {
        std::vector<unsigned char> fileData;
        SFileInfo fi = fs.GetFileInfo( fileIdx );

        if( !fs.ExtractFile( fi.name, fileData ) )
        {
            std::cout << "The requested file couldn't be found. The disk image may be damaged or corrupted." << std::endl;
            return false;
        }

        FILE* pOut = fopen( fi.name.c_str(), "wb" );
        if( nullptr == pOut )
        {
            std::cout << "Error creating file " << fi.name << std::endl;
            return false;
        }

        size_t bytesWritten = fwrite( fileData.data(), 1, fileData.size(), pOut );
        fclose( pOut );

        if( bytesWritten != fileData.size() )
        {
            std::cout << "Error writing file " << fi.name << " " << bytesWritten << " of " << fileData.size() << " bytes written." << std::endl;
            return false;
        }

        std::cout << fileIdx << "\t" << PadFilename(fi.name) << "\t" << bytesWritten << " of " << fileData.size() << " bytes written." << std::endl;
    }

    return true;
}

bool InfoCommand( const std::vector<std::string>& _args )
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
    CVDKDiskImage img;
    CDragonDOS_FS fs;

    if( !LoadImageAndFilesystem( _args[2], &img, &fs ) )
    {
        return false;
    } 

    size_t sides = img.GetSidesNum();
    size_t tracks = img.GetTracksNum();
    size_t sectors = img.GetSectorsNum();
    size_t sectorSize = img.GetSectorSize();
    size_t totalBytes = sides * tracks * sectors * sectorSize;

    std::cout << sides            << " sides"             << std::endl;
    std::cout << tracks           << " tracks"            << std::endl;
    std::cout << sectors          << " sectors per track" << std::endl;
    std::cout << sectorSize       << " bytes per sector"  << std::endl;
    std::cout << totalBytes       << " total bytes"       << std::endl;
    std::cout << fs.GetFreeSize() << " free bytes"        << std::endl;
    std::cout << fs.GetFSName()   << " file system"       << std::endl;
    std::cout << fs.GetFilesNum() << " files"             << std::endl;

    return true;
}

bool NewCommand( const std::vector<std::string>& _args )
{
    // Check arguments
    if( _args.size() < 4 )
    {
        std::cout << "The New command requires a filename and a disk size (180,360 or 720)." << std::endl << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "\tdragondos new blank.vdk 180" << std::endl << std::endl;

        HelpCommand();

        return false;
    }

    CVDKDiskImage img;
    CDragonDOS_FS fs;

    // Create new disk image
    int disk_size = stoi(_args[3]);
    switch( disk_size )
    {
        case 180:img.New( 40, 1, VDK_SECTORSPERTRACK);break;
        case 360:img.New( 40, 2, VDK_SECTORSPERTRACK);break;
        case 720:img.New( 80, 2, VDK_SECTORSPERTRACK);break;
        default:
        {
            std::cout << "Invalid disk size (" << disk_size << ")." << std::endl;
            std::cout << "Please use either 180, 360 or 720 as size values." << std::endl << std::endl;

            HelpCommand();

            return false;
        }
    }

    // Initialize filesystem
    if( !fs.InitDisk( &img ) )
    {
        std::cout << "Couldn't initialize the file system on the new disk image." << std::endl << std::endl;
        return false;
    }

    // Save image
    if( !img.Save(_args[2]) )
    {
        std::cout << "Couldn't save new image file " << _args[2] << std::endl << std::endl;
        return false;
    }

    return true;
}

bool DeleteCommand( const std::vector<std::string>& _args )
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
    CVDKDiskImage img;
    CDragonDOS_FS fs;

    if( !LoadImageAndFilesystem( _args[2], &img, &fs ) )
    {
        return false;
    }
    
    size_t fileIdx = atoi( _args[3].c_str() );
    if( fileIdx >= fs.GetFilesNum() )
    {
        std::cout << "Specified index is out of range." << std::endl << std::endl;
        std::cout << " The valid range for this image is 0 to " << fs.GetFilesNum() - 1 << "." << std::endl;
        return false;
    }

    // Create backup file
    std::string backupFilename = _args[2];
    backupFilename += ".bak";
    if( !fs.GetDisk()->Save( backupFilename ) )
    {
        std::cout << "Could not create backup file " << backupFilename << std::endl;
        return false;
    }

    SFileInfo fi = fs.GetFileInfo( fileIdx );

    if( !fs.DeleteFile( fi.name ) )
    {
        std::cout << "The requested file couldn't be deleted. Please check that the file name is correct." << std::endl;
        std::cout << "The disk image may be damaged or corrupted." << std::endl;
        return false;
    }

    // Save modified disk image
    if( !fs.GetDisk()->Save( _args[2] ) )
    {
        std::cout << "Could not overwrite file " << _args[2] << std::endl;
        return false;
    }

    return true;
}

bool InsertBasicCommand( const std::vector<std::string>& _args )
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
    CVDKDiskImage img;
    CDragonDOS_FS fs;

    if( !LoadImageAndFilesystem( _args[2], &img, &fs ) )
    {
        return false;
    }

    // Create backup file
    std::string backupFilename = _args[2];
    backupFilename += ".bak";
    if( !fs.GetDisk()->Save( backupFilename ) )
    {
        std::cout << "Could not create backup file " << backupFilename << std::endl;
        return false;
    }

    // Load file to be inserted
    std::string insertFilename = _args[3];
    std::vector<char> fileData;

    FILE* pIn = fopen( insertFilename.c_str(), "rb" );
    if( nullptr == pIn )
    {
        std::cout << "Could not open requested file " << insertFilename.c_str() << std::endl;
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
        return false;
    }

    fileData.resize( insertFileSize );
    size_t bytesRead = fread( fileData.data(), 1, insertFileSize, pIn );
    fclose( pIn );

    if( bytesRead < insertFileSize )
    {
        std::cout << "Error loading file " << insertFilename.c_str() << " (" << bytesRead;
        std::cout << insertFileSize << " bytes read)." << std::endl;
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

    std::filesystem::path filePath( _args[3] );

    if( !fs.InsertFile( filePath.filename().string(), encodedData ) )
    {
        std::cout << "The requested file couldn't be inserted. Please check that the file name is correct." << std::endl;
        std::cout << "The disk image may not have enough free space or be damaged or corrupted." << std::endl;
        return false;
    }

    // Save modified disk image
    if( !fs.GetDisk()->Save( _args[2] ) )
    {
        std::cout << "Could not overwrite file " << _args[2] << std::endl;
        return false;
    }

    return true;
}

bool InsertBinaryCommand( const std::vector<std::string>& _args )
{
    // Check arguments
    if( _args.size() < 6 )
    {
        std::cout << "The InsertBinary command requires a disk image filename, the filename of the file to insert" << std::endl;
        std::cout << "plus the Load and Exec addresses (decimal or hex)." << std::endl << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "\tdragondos insertbinary mydisk.vdk file.bin 3072 0xC00" << std::endl << std::endl;

        HelpCommand();

        return false;
    }

    // Load disk image and initialize file system
    CVDKDiskImage img;
    CDragonDOS_FS fs;

    if( !LoadImageAndFilesystem( _args[2], &img, &fs ) )
    {
        return false;
    }

    // Create backup file
    std::string backupFilename = _args[2];
    backupFilename += ".bak";
    if( !fs.GetDisk()->Save( backupFilename ) )
    {
        std::cout << "Could not create backup file " << backupFilename << std::endl;
        return false;
    }

    // Load file to be inserted
    std::string insertFilename = _args[3];
    std::vector<unsigned char> fileData;

    FILE* pIn = fopen( insertFilename.c_str(), "rb" );
    if( nullptr == pIn )
    {
        std::cout << "Could not open requested file " << insertFilename.c_str() << std::endl;
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
        return false;
    }

    fileData.resize( insertFileSize + DRAGONDOS_FILEHEADER_SIZE );
    size_t bytesRead = fread( fileData.data()+DRAGONDOS_FILEHEADER_SIZE, 1, insertFileSize, pIn );
    fclose( pIn );

    if( bytesRead < insertFileSize )
    {
        std::cout << "Error loading file " << insertFilename.c_str() << " (" << bytesRead;
        std::cout << insertFileSize << " bytes read)." << std::endl;
        return false;
    }

    unsigned long int parsedLoadAddress = strtoul( _args[4].c_str(), nullptr, 0 );
    if( parsedLoadAddress > 0xFFFF )
    {
        std::cout << "Out of range load address " << parsedLoadAddress << std::endl << "Valid range is 0 to 65535 (0xFFFF)" << std::endl;
        return false;
    }
    unsigned short int loadAddress = (unsigned short int)parsedLoadAddress;

    unsigned long int parsedExecAddress = strtoul( _args[5].c_str(), nullptr, 0 );
    if( parsedExecAddress > 0xFFFF )
    {
        std::cout << "Out of range exec address " << parsedExecAddress << std::endl << "Valid range is 0 to 65535 (0xFFFF)" << std::endl;
        return false;
    } 
    unsigned short int execAddress = (unsigned short int)parsedExecAddress;

    // Add file header /////////////////////////////////
    // For BASIC files, load address is usually always 
    // 0x2401 and the exec address 0x8B8D
    ////////////////////////////////////////////////////
    fileData[0] = DRAGONDOS_FILE_HEADER_BEGIN;      // Constant
    fileData[1] = DRAGONDOS_FILETYPE_BINARY;        // File type
    fileData[2] = (loadAddress / 256) & 0xFF;       // Load address high byte
    fileData[3] = loadAddress & 0xFF;               // Load address low byte
    fileData[4] = (insertFileSize / 256) & 0xFF;   // File size high byte
    fileData[5] = insertFileSize & 0xFF;           // File size low byte
    fileData[6] = (execAddress / 256) & 0xFF;       // Exec address high byte
    fileData[7] = execAddress & 0xFF;               // Exec address low byte
    fileData[8] = DRAGONDOS_FILE_HEADER_END;        // Constant
    ////////////////////////////////////////////////////

    std::filesystem::path filePath( _args[3] );

    if( !fs.InsertFile( filePath.filename().string(), fileData ) )
    {
        std::cout << "The requested file couldn't be inserted. Please check that the file name is correct." << std::endl;
        std::cout << "The disk image may not have enough free space or be damaged or corrupted." << std::endl;
        return false;
    }

    // Save modified disk image
    if( !fs.GetDisk()->Save( _args[2] ) )
    {
        std::cout << "Could not overwrite file " << _args[2] << std::endl;
        return false;
    }

    return true;
}

bool InsertDataCommand( const std::vector<std::string>& _args )
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
    CVDKDiskImage img;
    CDragonDOS_FS fs;

    if( !LoadImageAndFilesystem( _args[2], &img, &fs ) )
    {
        return false;
    }

    // Create backup file
    std::string backupFilename = _args[2];
    backupFilename += ".bak";
    if( !fs.GetDisk()->Save( backupFilename ) )
    {
        std::cout << "Could not create backup file " << backupFilename << std::endl;
        return false;
    }

    // Load file to be inserted
    std::string insertFilename = _args[3];
    std::vector<unsigned char> fileData;

    FILE* pIn = fopen( insertFilename.c_str(), "rb" );
    if( nullptr == pIn )
    {
        std::cout << "Could not open requested file " << insertFilename.c_str() << std::endl;
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
        return false;
    }

    fileData.resize( insertFileSize );
    size_t bytesRead = fread( fileData.data(), 1, insertFileSize, pIn );
    fclose( pIn );

    if( bytesRead < insertFileSize )
    {
        std::cout << "Error loading file " << insertFilename.c_str() << " (" << bytesRead;
        std::cout << insertFileSize << " bytes read)." << std::endl;
        return false;
    }

    std::filesystem::path filePath( _args[3] );

    if( !fs.InsertFile( filePath.filename().string(), fileData ) )
    {
        std::cout << "The requested file couldn't be inserted. Please check that the file name is correct." << std::endl;
        std::cout << "The disk image may not have enough free space or be damaged or corrupted." << std::endl;
        return false;
    }

    // Save modified disk image
    if( !fs.GetDisk()->Save( _args[2] ) )
    {
        std::cout << "Could not overwrite file " << _args[2] << std::endl;
        return false;
    }

    return true;
}
