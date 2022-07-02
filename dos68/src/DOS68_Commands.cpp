#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <iostream>

#include "DOS68_BlankDiskImage.h"
#include "DOS68_Commands.h"
#include "DOS68_FS.h"
#include "RawDiskImage.h"

string PadFilename( const string& _name )
{
    string retVal = _name;
    retVal.append( DOS68_MAX_FILE_FULL_NAME_LEN - retVal.length(), ' ' );

    return retVal;
}

bool LoadImageAndFilesystem( string _filename, CRAWDiskImage& _img, CDOS68_FS& _fs )
{
    // Load disk image and set disk geometry
    if( !_img.Load( _filename ) )
    {
        cout << "Failed to load " << _filename << endl;
        return false;
    }

    _img.SetSectorSize( DOS68_SECTOR_SIZE        );
    _img.SetSectorsNum( DOS68_SECTORS_PER_TRACK  );
    _img.SetSidesNum  ( 1   );
    _img.SetTracksNum ( 35  );

    // Initialize filesystem
    _fs.Load( &_img );

    return true;
}

bool HelpCommand()
{
    cout << "usage: dos68 <command> [<args> ...]" << endl << endl;
    cout << "A command-line tool for managing DOS68 disk images." << endl << endl;
    cout << "Commands:" << endl;
    cout << "\thelp" << endl << "\t  Displays this help text." << endl << endl;
    cout << "\tinfo <filename>" << endl << "\t  Displays disk image information like type, free space and number of files." << endl << endl;
    cout << "\tlist <filename>" << endl;
    cout << "\t  Displays a list of the files on a disk image and the following data:" << endl;
    cout << "\t  Index, file name, size in sectors and size in bytes." << endl << endl;
    cout << "\textract <filename> [<index>]" << endl;
    cout << "\t  Extracts all files from within the disk image to the current directory." << endl;
    cout << "\t  If an optional file index is specified, only that file will be extracted to" << endl;
    cout << "\t  current directory." << endl << endl;
    cout << "\tnew <filename>" << endl;
    cout << "\t  Creates a new, formatted disk image." << endl << endl;
    cout << "\tdelete <filename> <index>" << endl;
    cout << "\t  Deletes a file from the disk image." << endl;
    cout << "\t  A backup copy of the disk image will be created." << endl << endl;
    cout << "\tinsert <image filename> <filename to insert> <ascii|binary>" << endl;
    cout << "\t  Inserts a file into the requested disk image." << endl << endl;

    return true;
}

bool ListCommand( const vector<string>& _args )
{
    // Check arguments
    if( _args.size() < 3 )
    {
        cout << "The List command requires a filename." << endl << endl;
        cout << "Example:" << endl;
        cout << "\tdos68 list fortran.img" << endl << endl;

        HelpCommand();

        return false;
    }

    // Load disk image and initialize file system
    CRAWDiskImage img;
    CDOS68_FS fs;

    if( !LoadImageAndFilesystem( _args[2], img, fs ) )
    {
        return false;
    } 

    // Display file list
    for( size_t fileIdx = 0; fileIdx < fs.GetFilesNum(); ++fileIdx )
    {
        SFileInfo fi = fs.GetFileInfo( fileIdx );
        cout << fileIdx << "\t" << PadFilename(fi.name) << "\t" << fi.size / DOS68_SECTOR_DATA_SIZE << "\t" << fi.size << endl;
    }
    
    return true;
}

bool ExtractCommand( const vector<string>& _args )
{
    // Check arguments
    if( _args.size() < 3 )
    {
        cout << "The Extract command requires a filename and an optional index." << endl << endl;
        cout << "Examples:" << endl;
        cout << "\tdos68 list fortran.img   (extracts all files)" << endl;
        cout << "\tdos68 list fortran.img 7 (extracts file 7 from the file list)" << endl << endl;

        HelpCommand();

        return false;
    }

    // Load disk image and initialize file system
    CRAWDiskImage img;
    CDOS68_FS fs;

    if( !LoadImageAndFilesystem( _args[2], img, fs ) )
    {
        return false;
    } 

    size_t start = ((_args.size() > 3) ? atoi(_args[3].c_str()) : 0);
    size_t end   = ((_args.size() > 3) ? (start + 1)   : fs.GetFilesNum());

    if( start >= fs.GetFilesNum() || end > fs.GetFilesNum() )
    {
        cout << "Specified index is out of range." << endl << endl;
        cout << " The valid range for this image is 0 to " << fs.GetFilesNum() - 1 << "." << endl;
        return false;
    }

    for( size_t fileIdx = start; fileIdx < end; ++fileIdx )
    {
        vector<unsigned char> fileData;
        SFileInfo fi = fs.GetFileInfo( fileIdx );

        if( !fs.ExtractFile( fi.name, fileData ) )
        {
            cout << "The requested file couldn't be found. The disk image may be damaged or corrupted." << endl;
            return false;
        }

        FILE* pOut = fopen( fi.name.c_str(), "wb" );
        if( nullptr == pOut )
        {
            cout << "Error creating file " << fi.name << endl;
            return false;
        }

        size_t bytesWritten = fwrite( fileData.data(), 1, fileData.size(), pOut );
        fclose( pOut );

        if( bytesWritten != fileData.size() )
        {
            cout << "Error writing file " << fi.name << " " << bytesWritten << " of " << fileData.size() << " bytes written." << endl;
            return false;
        }

        cout << fileIdx << "\t" << PadFilename(fi.name) << "\t" << bytesWritten << " of " << fileData.size() << " bytes written." << endl;
    }

    return true;
}

bool InfoCommand( const vector<string>& _args )
{
    // Check arguments
    if( _args.size() < 3 )
    {
        cout << "The Info command requires a filename." << endl << endl;
        cout << "Examples:" << endl;
        cout << "\tdos68 info fortran.img" << endl << endl;

        HelpCommand();

        return false;
    }

    // Load disk image and initialize file system
    CRAWDiskImage img;
    CDOS68_FS fs;

    if( !LoadImageAndFilesystem( _args[2], img, fs ) )
    {
        return false;
    }

    unsigned char diskType = fs.GetDiskType();
    cout << "Disk type: ";
    cout << (((diskType & DOS68_DISK_TYPE_SIDES_MASK    ) == DOS68_DISK_TYPE_SINGLE_SIDE   ) ? "Single-sided, "   : "Double-sided, "     );
    cout << (((diskType & DOS68_DISK_TYPE_DENSITY_MASK  ) == DOS68_DISK_TYPE_SINGLE_DENSITY) ? "Single density, " : "Double density, "   );
    cout << (((diskType & DOS68_DISK_TYPE_INCHES_MASK   ) == DOS68_DISK_TYPE_5_INCHES      ) ? "5.25\", "         : "8\", "              );
    cout << (((diskType & DOS68_DISK_TYPE_TPI_MASK      ) == DOS68_DISK_TYPE_48_TPI        ) ? "48 TPI, "         : "96 TPI, "           );
    cout << (((diskType & DOS68_DISK_TYPE_PROTECTED_MASK) == DOS68_DISK_TYPE_UNPROTECTED   ) ? "Unprotected."     : "Protected (Master).");
    cout << endl;

    cout << "Next available block track: "  << (unsigned int)fs.GetNextAvailableTrack()  << endl;
    cout << "Next available block sector: " << (unsigned int)fs.GetNextAvailableSector() << endl;
    cout << "Last available block track: "  << (unsigned int)fs.GetLastAvailableTrack()  << endl;
    cout << "Last available block sector: " << (unsigned int)fs.GetLastAvailableSector() << endl;
    cout << "Available sectors: "           << (unsigned int)fs.GetAvailableSectorsNum();
    
    cout << " (" << fs.GetAvailableSectorsNum() * DOS68_SECTOR_DATA_SIZE << " bytes)" << endl;

    cout << "Number of files: "             << fs.GetFilesNum() << endl;

    return true;
}

bool NewCommand( const vector<string>& _args )
{
    // Check arguments
    if( _args.size() < 3 )
    {
        cout << "The New command requires a filename." << endl << endl;
        cout << "Examples:" << endl;
        cout << "\tdos68 new blank.img" << endl << endl;

        HelpCommand();

        return false;
    }

    FILE* pOut = fopen( _args[2].c_str(), "wb" );
    if( nullptr == pOut )
    {
        cout << "Error creating file " << _args[2] << endl;
        return false;
    }

    size_t bytesWritten = fwrite( blankImage, 1, BLANK_DISK_IMAGE_SIZE, pOut );
    fclose( pOut );

    if( bytesWritten != BLANK_DISK_IMAGE_SIZE )
    {
        cout << "Error writing file " << _args[2] << " " << bytesWritten << " of " << BLANK_DISK_IMAGE_SIZE << " bytes written." << endl;
        return false;
    }

    return true;
}

bool DeleteCommand( const vector<string>& _args )
{
    // Check arguments
    if( _args.size() < 4 )
    {
        cout << "The Delete command requires a filename and a file index." << endl << endl;
        cout << "Examples:" << endl;
        cout << "\tdos68 delete fortran.img 15" << endl << endl;

        HelpCommand();

        return false;
    }

    // Load disk image and initialize file system
    CRAWDiskImage img;
    CDOS68_FS fs;

    if( !LoadImageAndFilesystem( _args[2], img, fs ) )
    {
        return false;
    }
    
    size_t fileIdx = atoi( _args[3].c_str() );
    if( fileIdx >= fs.GetFilesNum() )
    {
        cout << "Specified index is out of range." << endl << endl;
        cout << " The valid range for this image is 0 to " << fs.GetFilesNum() - 1 << "." << endl;
        return false;
    }

    // Create backup file
    string backupFilename = _args[2];
    backupFilename += ".bak";
    if( !fs.Save( backupFilename ) )
    {
        cout << "Could not create backup file " << backupFilename << endl;
        return false;
    }

    SFileInfo fi = fs.GetFileInfo( fileIdx );

    if( !fs.RemoveFile( fi.name ) )
    {
        cout << "The requested file couldn't be deleted. Please check that the file name is correct." << endl;
        cout << "The disk image may be damaged or corrupted." << endl;
        return false;
    }

    // Save modified disk image
    if( !fs.Save( _args[2] ) )
    {
        cout << "Could not overwrite file " << _args[2] << endl;
        return false;
    }

    return true;
}

bool InsertCommand( const vector<string>& _args )
{
    // Check arguments
    if( _args.size() < 5 )
    {
        cout << "The Insert command requires a disk image filename, the filename of the file to insert and its type." << endl << endl;
        cout << "Examples:" << endl;
        cout << "\tdos68 insert fortran.img test.txt ascii" << endl << endl;

        HelpCommand();

        return false;
    }

    // Load disk image and initialize file system
    CRAWDiskImage img;
    CDOS68_FS fs;

    if( !LoadImageAndFilesystem( _args[2], img, fs ) )
    {
        return false;
    }

    // Create backup file
    string backupFilename = _args[2];
    backupFilename += ".bak";
    if( !fs.Save( backupFilename ) )
    {
        cout << "Could not create backup file " << backupFilename << endl;
        return false;
    }

    // Load file to be inserted
    string insertFilename = _args[3];
    vector<unsigned char> fileData;

    FILE* pIn = fopen( insertFilename.c_str(), "rb" );
    if( nullptr == pIn )
    {
        cout << "Could not open requested file " << insertFilename.c_str() << endl;
        return false;
    }

    fseek( pIn, 0, SEEK_END );
    size_t insertFileSize = ftell( pIn );
    fseek( pIn, 0, SEEK_SET );

    // Check if there's enough room on the disk for the file to be inserted.
    size_t freeBytes = fs.GetAvailableSectorsNum() * DOS68_SECTOR_DATA_SIZE;
    if( insertFileSize > freeBytes )
    {
        fclose( pIn );

        cout << "Not enough room to insert " << insertFilename << ". Another " << insertFileSize - freeBytes << " are needed." << endl;
        return false;
    }

    fileData.resize( insertFileSize );
    size_t bytesRead = fread( fileData.data(), 1, insertFileSize, pIn );
    fclose( pIn );

    if( bytesRead < insertFileSize )
    {
        cout << "Error loading file " << insertFilename.c_str() << " (" << bytesRead;
        cout << insertFileSize << " bytes read)." << endl;
        return false;
    }

    string typeStr;
    transform( _args[4].begin(), _args[4].end(), typeStr.begin(), ::tolower );
    bool isBinary = (0 == typeStr.compare("binary"));

    if( !fs.InsertFile( insertFilename, fileData, isBinary ) )
    {
        cout << "The requested file couldn't be inserted. Please check that the file name is correct." << endl;
        cout << "The disk image may not have enough free space or be damaged or corrupted." << endl;
        return false;
    }

    // Save modified disk image
    if( !fs.Save( _args[2] ) )
    {
        cout << "Could not overwrite file " << _args[2] << endl;
        return false;
    }

    return true;
}