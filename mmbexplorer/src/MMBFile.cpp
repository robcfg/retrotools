#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <string.h> // for memset
#include "MMBFile.h"

const unsigned char firstDirectoryEntry  [MMB_DIRECTORYENTRYSIZE] = { 0,1,2,3,0,0,0,0,0,0,0,0,0,0,0,0    };
const unsigned char emptyDirectoryEntry  [MMB_DIRECTORYENTRYSIZE] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,MMB_DISKATTRIBUTE_UNFORMATTED };
const unsigned char invalidDirectoryEntry[MMB_DIRECTORYENTRYSIZE] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,MMB_DISKATTRIBUTE_INVALID     };
const size_t bytesInKilobyte = 1024;

CMMBFile::CMMBFile()
{

}

CMMBFile::~CMMBFile()
{
    // Make sure we close the file properly.
    //if( nullptr != mFile )
    //{
    //    fclose( mFile );
    //}    
}

bool CMMBFile::Open( const std::string& _filename, std::string& _errorString )
{
    // Close file handle if opened previously
    Close();

    // Open file in binary read/write mode
    mFile = fopen( _filename.c_str(), "rb+" );
    if( nullptr == mFile )
    {
        _errorString = "Could not open file ";
        _errorString += _filename;
        return false;
    }
    mFilename = _filename;

    // Get file size and compute maximum number of disks that can be stored on the MMB file.
    fseek( mFile, 0, SEEK_END );
    mFileSize = ftell( mFile );
    fseek( mFile, 0, SEEK_SET );

    if( mFileSize < MMB_DIRECTORYSIZE )
    {
        _errorString = "File is too short (";
        _errorString += std::to_string( mFileSize );
        _errorString += " bytes). It needs to be 8KB + 200KB per disk.";

        Close();

        return false;
    }

    mNumberOfDisks = (mFileSize - MMB_DIRECTORYSIZE) / MMB_DISKSIZE;

    // Check size.
    // TODO: Add a cmd line switch to be able to work on damaged images
    size_t remainder = (mFileSize - MMB_DIRECTORYSIZE) % MMB_DISKSIZE;
    if( 0 != remainder )
    {
        _errorString = "File size inconsistency, possible corrupt or broken file.";
        
        Close();

        return false;
    }

    ReadDirectory();

    return true;
}

bool CMMBFile::Create( const std::string& _filename, size_t _numberOfDisks, std::string& _errorString ) const
{
    _numberOfDisks = std::min( _numberOfDisks, MMB_MAXNUMBEROFDISKS );

    // Open file
    FILE* pFile = fopen( _filename.c_str(), "wb" );
    if( nullptr == pFile )
    {
        _errorString = "Could not create file ";
        _errorString += _filename;
        return false;
    }

    // Write header. Always 8192 bytes. One header entry and 511 slots.
    fwrite( firstDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, pFile );

    // Write usable entries  
    for( size_t usable = 0; usable < _numberOfDisks; ++usable )
    {
        fwrite( emptyDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, pFile );
    }

    // Write non-existant entries
    for( size_t entry = 0; entry < MMB_MAXNUMBEROFDISKS - _numberOfDisks; ++entry )
    {
        fwrite( invalidDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, pFile );
    }

    // Write empty data area
    unsigned char kilobyte[bytesInKilobyte];
    memset( kilobyte, 0, bytesInKilobyte );
    size_t bytesToWrite = MMB_DISKSIZE * _numberOfDisks;

    for( size_t written = 0; written < bytesToWrite; written += bytesInKilobyte )
    {
        fwrite( kilobyte, 1, bytesInKilobyte, pFile );
    }

    // Close file
    fclose( pFile );
    
    return true;
}

void CMMBFile::Close()
{
    if( nullptr != mFile )
    {
        fclose( mFile );
        mFile = nullptr;
    }

    mFileSize = 0;
    mNumberOfDisks = 0;

    ClearDirectory();
}

const SMMBDirectoryEntry* CMMBFile::GetDirectory()
{
    return mDirectory;
}

const char* CMMBFile::GetEntryName( size_t _entry )
{ 
    return mDirectory[_entry].name.c_str(); 
}

unsigned char CMMBFile::GetEntryAttribute( size_t _entry )
{ 
    return mDirectory[_entry].diskAttributes; 
}

void CMMBFile::ReadDirectory()
{
    if( nullptr == mFile )
    {
        return;
    }

    ClearDirectory();

    // Skip first entry
    fseek( mFile, MMB_DIRECTORYENTRYSIZE, SEEK_SET );

    // Read entries
    char tmpChar = 0;
    size_t bytesRead = 0;

    for( size_t entry = 0; entry < MMB_MAXNUMBEROFDISKS; ++entry )
    {
        for( size_t nameChar = 0; nameChar < MMB_MAXDISKNAMELENGTH; ++nameChar )
        {
            bytesRead = fread( &tmpChar, 1, 1, mFile );
            mDirectory[entry].name += tmpChar;
        }

        // Skip 3 bytes
        fseek( mFile, 3, SEEK_CUR );

        // Read disk atributes byte
        bytesRead = fread( &mDirectory[entry].diskAttributes, 1, 1, mFile );
    }
}

void CMMBFile::ClearDirectory()
{
    for( size_t entry = 0; entry < MMB_MAXNUMBEROFDISKS; ++entry )
    {
        mDirectory[entry].name = "";
        mDirectory[entry].diskAttributes = MMB_DISKATTRIBUTE_INVALID;
    }
}

size_t CMMBFile::GetNumberOfDisks() const
{
    return mNumberOfDisks;
}

bool CMMBFile::InsertImageInSlot( const std::string& _filename, size_t _slot, std::string& _errorString )
{
    if( nullptr == mFile )
    {
        _errorString = "No MMB file opened.";
        return false;
    }

    // Check slot number
    if( _slot >= GetNumberOfDisks() )
    {
        _errorString = "Slot number out of range or invalid: ";
        _errorString += std::to_string(_slot);
        _errorString += " ( max slot number is ";
        _errorString += std::to_string( GetNumberOfDisks() - 1);
        _errorString += ").";
        return false;
    }

    // Open disk image to insert
    FILE* pFile = fopen( _filename.c_str(), "rb" );
    if( nullptr == pFile )
    {
        _errorString = "Could not open disk image file ";
        _errorString += _filename;
        return false;
    }

    // Allocate memory for storing the SSD image
    unsigned char* pImage = new unsigned char[MMB_DISKSIZE];
    if( nullptr == pImage )
    {
        fclose( pFile );
        _errorString = "Could not allocate memory for temporal disk image storage.";
        return false;
    }
    memset( pImage, 0, MMB_DISKSIZE );

    // Get file size
    fseek( pFile, 0, SEEK_END );
    size_t fileSize = ftell( pFile );
    fseek( pFile, 0, SEEK_SET );

    if( fileSize > MMB_DISKSIZE )
    {
        delete[] pImage;
        fclose( pFile );
        _errorString = "Disk image size is greater than 200KB (";
        _errorString += std::to_string( fileSize );
        _errorString += ").";
        return false;
    }

    // Read file
    size_t bytesRead = 0;
    bytesRead = fread( pImage, 1, fileSize, pFile );
    fclose( pFile );

    // Write directory entry
    unsigned char directoryEntry[MMB_DIRECTORYENTRYSIZE];
    memset( directoryEntry, 0, MMB_DIRECTORYENTRYSIZE );

    directoryEntry[MMB_DIRECTORYENTRYSIZE - 1] = MMB_DISKATTRIBUTE_UNLOCKED;
    memcpy( &directoryEntry[0], pImage, 8 );
    memcpy( &directoryEntry[8], &pImage[256], 4 );

    fseek( mFile, (_slot + 1) * MMB_DIRECTORYENTRYSIZE, SEEK_SET );
    fwrite( directoryEntry, 1, MMB_DIRECTORYENTRYSIZE, mFile );

    // Write disk image
    fseek( mFile, MMB_DIRECTORYSIZE + (_slot * MMB_DISKSIZE), SEEK_SET );
    fwrite( pImage, 1, MMB_DISKSIZE, mFile );

    // Cleanup
    delete[] pImage;

    ReadDirectory();

    return true;
}

bool CMMBFile::InsertImageInSlot( const unsigned char* _data, size_t _dataSize, size_t _slot, std::string& _errorString )
{
    if( nullptr == mFile )
    {
        _errorString = "No MMB file opened.";
        return false;
    }

    // Check slot number
    if( _slot >= GetNumberOfDisks() )
    {
        _errorString = "Slot number out of range or invalid: ";
        _errorString += std::to_string(_slot);
        _errorString += " ( max slot number is ";
        _errorString += std::to_string( GetNumberOfDisks() - 1);
        _errorString += ").";
        return false;
    }

    // Write directory entry
    unsigned char directoryEntry[MMB_DIRECTORYENTRYSIZE];
    memset( directoryEntry, 0, MMB_DIRECTORYENTRYSIZE );

    directoryEntry[MMB_DIRECTORYENTRYSIZE - 1] = MMB_DISKATTRIBUTE_UNLOCKED;
    memcpy( &directoryEntry[0], _data, 8 );
    memcpy( &directoryEntry[8], &_data[256], 4 );

    fseek( mFile, (_slot + 1) * MMB_DIRECTORYENTRYSIZE, SEEK_SET );
    fwrite( directoryEntry, 1, MMB_DIRECTORYENTRYSIZE, mFile );

    // Write disk image
    fseek( mFile, MMB_DIRECTORYSIZE + (_slot * MMB_DISKSIZE), SEEK_SET );
    fwrite( _data, 1, MMB_DISKSIZE, mFile );

    ReadDirectory();

    return true;
}

bool CMMBFile::ExtractImageInSlot( const std::string& _filename, size_t _slot, std::string& _errorString )
{
    if( nullptr == mFile )
    {
        _errorString = "No MMB file opened.";
        return false;
    }

    // Check slot number
    if( _slot >= GetNumberOfDisks() )
    {
        _errorString = "Slot number out of range or invalid: ";
        _errorString += std::to_string(_slot);
        _errorString += " ( max slot number is ";
        _errorString += std::to_string( GetNumberOfDisks() - 1);
        _errorString += ").";
        return false;
    }

    // Open destination file
    FILE* pDestinationFile = fopen( _filename.c_str(), "wb" );
    if( nullptr == pDestinationFile )
    {
        _errorString = "Could not create/overwrite destination file ";
        _errorString = _filename;
        return false;
    }

    // Allocate memory for storing the disk image
    unsigned char* pImage = new unsigned char[MMB_DISKSIZE];
    if( nullptr == pImage )
    {
        _errorString = "Could not allocate memory for temporal storage.";
        return false;
    }

    size_t bytesRead = 0;
    fseek( mFile, MMB_DIRECTORYSIZE + (_slot * MMB_DISKSIZE), SEEK_SET );
    bytesRead = fread( pImage, 1, MMB_DISKSIZE, mFile );
    fwrite( pImage, 1, MMB_DISKSIZE, pDestinationFile );

    fclose( pDestinationFile );
    delete[] pImage;

    return true;
}

bool CMMBFile::ExtractImageInSlot( unsigned char* _data, size_t _slot, std::string& _errorString )
{
    size_t bytesRead = 0;
    fseek( mFile, MMB_DIRECTORYSIZE + (_slot * MMB_DISKSIZE), SEEK_SET );
    bytesRead = fread( _data, 1, MMB_DISKSIZE, mFile );
    
    return true;
}

bool CMMBFile::LockImageInSlot( size_t _slot, std::string& _errorString )
{
    if( nullptr == mFile )
    {
        _errorString = "No MMB file opened.";
        return false;
    }

    // Check slot number
    if( _slot >= GetNumberOfDisks() )
    {
        _errorString = "Slot number out of range or invalid: ";
        _errorString += std::to_string( _slot );
        _errorString += " ( max slot number is ";
        _errorString += std::to_string( GetNumberOfDisks() - 1);
        _errorString += ").";
        return false;
    }

    fseek( mFile, ((_slot + 2) * MMB_DIRECTORYENTRYSIZE) - 1, SEEK_SET );
    fwrite( &MMB_DISKATTRIBUTE_LOCKED, 1, 1, mFile );

    ReadDirectory();

    return true;
}

bool CMMBFile::UnlockImageInSlot( size_t _slot, std::string& _errorString )
{
    if( nullptr == mFile )
    {
        _errorString = "No MMB file opened.";
        return false;
    }

    // Check slot number
    if( _slot >= GetNumberOfDisks() )
    {
        _errorString = "Slot number out of range or invalid: ";
        _errorString += std::to_string( _slot );
        _errorString += " ( max slot number is ";
        _errorString += std::to_string( GetNumberOfDisks() - 1);
        _errorString += ").";
        return false;
    }

    fseek( mFile, ((_slot + 2) * MMB_DIRECTORYENTRYSIZE) - 1, SEEK_SET );
    fwrite( &MMB_DISKATTRIBUTE_UNLOCKED, 1, 1, mFile );

    ReadDirectory();

    return true;
}

bool CMMBFile::RemoveImageFromSlot( size_t _slot, std::string& _errorString )
{
    if( nullptr == mFile )
    {
        _errorString = "No MMB file opened.";
        return false;
    }

    // Check slot number
    if( _slot >= GetNumberOfDisks() )
    {
        _errorString = "Slot number out of range or invalid: ";
        _errorString += std::to_string( _slot );
        _errorString += " ( max slot number is ";
        _errorString += std::to_string( GetNumberOfDisks() - 1);
        _errorString += ").";
        return false;
    }

    // Clear directory entry
    fseek( mFile, (_slot + 1) * MMB_DIRECTORYENTRYSIZE, SEEK_SET );
    fwrite( emptyDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, mFile );

    // Clear data area

    // Allocate memory for storing the empty data area
    unsigned char* pImage = new unsigned char[MMB_DISKSIZE];
    if( nullptr == pImage )
    {
        _errorString = "Could not allocate memory for temporal storage.";
        return false;
    }
    memset( pImage, 0, MMB_DISKSIZE );

    fseek( mFile, MMB_DIRECTORYSIZE + (_slot * MMB_DISKSIZE), SEEK_SET );
    fwrite( pImage, 1, MMB_DISKSIZE, mFile );

    delete[] pImage;

    ReadDirectory();

    return true;
}

const std::string& CMMBFile::GetFilename()
{
    return mFilename;
}

bool CMMBFile::LockFile( size_t _slot, size_t _fileIndex, std::string& _errorString )
{
    if( nullptr == mFile )
    {
        _errorString = "No MMB file opened.";
        return false;
    }

    // Check slot number
    if( _slot >= GetNumberOfDisks() )
    {
        _errorString = "Slot number out of range or invalid: ";
        _errorString += std::to_string( _slot );
        _errorString += " ( max slot number is ";
        _errorString += std::to_string( GetNumberOfDisks() - 1);
        _errorString += ").";
        return false;
    }

    unsigned char statusByte = 0;
    fseek( mFile, MMB_DIRECTORYSIZE + (_slot * MMB_DISKSIZE) + ((_fileIndex + 1) * 8) + 7, SEEK_SET );
    fread( &statusByte, 1, 1, mFile );
    statusByte |= 0x80;
    fseek( mFile, -1, SEEK_CUR );
    fwrite( &statusByte, 1, 1, mFile );
    
    return true;
}

bool CMMBFile::UnlockFile( size_t _slot, size_t _fileIndex, std::string& _errorString )
{
    if( nullptr == mFile )
    {
        _errorString = "No MMB file opened.";
        return false;
    }

    // Check slot number
    if( _slot >= GetNumberOfDisks() )
    {
        _errorString = "Slot number out of range or invalid: ";
        _errorString += std::to_string( _slot );
        _errorString += " ( max slot number is ";
        _errorString += std::to_string( GetNumberOfDisks() - 1);
        _errorString += ").";
        return false;
    }

    unsigned char statusByte = 0;
    fseek( mFile, MMB_DIRECTORYSIZE + (_slot * MMB_DISKSIZE) + ((_fileIndex + 1) * 8) + 7, SEEK_SET );
    fread( &statusByte, 1, 1, mFile );
    statusByte &= 0x7F;
    fseek( mFile, -1, SEEK_CUR );
    fwrite( &statusByte, 1, 1, mFile );

    return true;
}
