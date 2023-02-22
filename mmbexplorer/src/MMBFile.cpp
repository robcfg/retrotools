#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <string.h> // for memset
#include "MMBFile.h"
#include <io.h>
#ifndef WIN32
#include <unistd.h>
#endif

const unsigned char firstDirectoryEntry[MMB_DIRECTORYENTRYSIZE] = { 0,1,2,3,0,0,0,0,0,0,0,0,0,0,0,0 };
const unsigned char emptyDirectoryEntry[MMB_DIRECTORYENTRYSIZE] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,MMB_DISKATTRIBUTE_UNFORMATTED };
const unsigned char invalidDirectoryEntry[MMB_DIRECTORYENTRYSIZE] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,MMB_DISKATTRIBUTE_INVALID };
const size_t bytesInKilobyte = 1024;

CMMBFile::CMMBFile()
{
    mDirectory = new SMMBDirectoryEntry[MMB_MAXNUMBEROFDISKS];
}

CMMBFile::~CMMBFile()
{
    if (mDirectory) delete[] mDirectory;
    // Make sure we close the file properly.
    //if( nullptr != mFile )
    //{
    //    fclose( mFile );
    //}    
}

bool CMMBFile::Open(const std::string& _filename, std::string& _errorString)
{
    // Close file handle if opened previously
    Close();

    // Open file in binary read/write mode
    mFilename = _filename;
    if (!OpenMMBFileInternal())
    {
        _errorString = "Could not open file ";
        _errorString += _filename;
        Close();
        return false;
    }

    // Get file size and compute maximum number of disks that can be stored on the MMB file.
    fseek(mFile, 0, SEEK_END);
    mFileSize = ftell(mFile);
    fseek(mFile, 0, SEEK_SET);

    if (mFileSize < MMB_DIRECTORYSIZE)
    {
        _errorString = "File is too short (";
        _errorString += std::to_string(mFileSize);
        _errorString += " bytes). It needs to be 8KB + 200KB per disk.";

        Close();

        return false;
    }

    size_t remainder = 0;

    mNumberOfChunks = (mFileSize + MMB_CHUNKSIZE - 1) / (MMB_DIRECTORYSIZE + (MMB_MAXNUMBEROFDISKS * MMB_DISKSIZE));
    mFileSize -= (mNumberOfChunks-1) * MMB_CHUNKSIZE;

    mNumberOfDisks = (mNumberOfChunks-1) * MMB_MAXNUMBEROFDISKS;
    if (mFileSize > 0) {
        mNumberOfDisks += (mFileSize - MMB_DIRECTORYSIZE) / MMB_DISKSIZE;
        remainder = (mFileSize - MMB_DIRECTORYSIZE) % MMB_DISKSIZE;
    }


    // Check size.
    // TODO: Add a cmd line switch to be able to work on damaged images

    if (0 != remainder)
    {
        _errorString = "File size inconsistency, possible corrupt or broken file.";

        Close();

        return false;
    }
    CloseMMBFileInternal();

ReadDirectory();

return true;
}

bool CMMBFile::Create(const std::string& _filename, size_t _numberOfDisks, std::string& _errorString) const
{
    size_t ndisks;

    _numberOfDisks = std::min(_numberOfDisks, MMB_MAXNUMBEROFDISKS2);

    // Open file
    FILE* pFile = fopen(_filename.c_str(), "wb");
    if (nullptr == pFile)
    {
        _errorString = "Could not create file ";
        _errorString += _filename;
        return false;
    }
    size_t chunks = (_numberOfDisks + MMB_MAXNUMBEROFDISKS-1) / MMB_MAXNUMBEROFDISKS;

    while (_numberOfDisks > 0) {

        if (_numberOfDisks > MMB_MAXNUMBEROFDISKS) {
            ndisks = MMB_MAXNUMBEROFDISKS;
        }
        else {
            ndisks = _numberOfDisks;
        }
        // Write header. Always 8192 bytes. One header entry and 511 slots.
        fwrite(firstDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, pFile);

        // Write usable entries  
        for (size_t usable = 0; usable < ndisks; ++usable)
        {
            fwrite(emptyDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, pFile);
        }

        // Write non-existant entries
        for (size_t entry = 0; entry < MMB_MAXNUMBEROFDISKS - ndisks; ++entry)
        {
            fwrite(invalidDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, pFile);
        }

        // Write empty data area
        unsigned char kilobyte[bytesInKilobyte];
        memset(kilobyte, 0, bytesInKilobyte);
        size_t bytesToWrite = MMB_DISKSIZE * ndisks;

        for (size_t written = 0; written < bytesToWrite; written += bytesInKilobyte)
        {
            fwrite(kilobyte, 1, bytesInKilobyte, pFile);
        }
        _numberOfDisks -= ndisks;
    }

    if (chunks > 1) {
        fseek(pFile, 8, SEEK_SET);
        chunks = 0xa0 + chunks - 1;
        fwrite(&chunks, 1, 1, pFile);
    }

    // Close file
    fclose(pFile);

    return true;
}

void CMMBFile::Close()
    {
    if (nullptr != mFile)
    {
        fclose(mFile);
        mFile = nullptr;
    }

    mFileSize = 0;
    mFilename = "";
    mNumberOfDisks = 0;
    mBoot0 = 0;
    mBoot1 = 1;
    mBoot2 = 2;
    mBoot3 = 3;

    ClearDirectory();
}

bool CMMBFile::Resize(size_t _numberOfDisks, std::string& _errorString)
{

    if (!OpenMMBFileInternal())
    {
        _errorString = "No MMB file opened.";
        return false;
    }

    //Close();
    size_t chunks = _numberOfDisks / MMB_MAXNUMBEROFDISKS;
    int ndisks = _numberOfDisks % MMB_MAXNUMBEROFDISKS;

    size_t newsize = chunks * MMB_CHUNKSIZE;
    if (ndisks > 0) {
        newsize += MMB_DIRECTORYSIZE + (ndisks * MMB_DISKSIZE);
    }

    chunks = (_numberOfDisks + MMB_MAXNUMBEROFDISKS - 1) / MMB_MAXNUMBEROFDISKS;

    #ifdef WIN32
    if (_chsize(fileno(mFile), newsize)!=0) {
    #else
    if (ftruncate(fileno(mFile), newsize) != 0) {
    #endif
        _errorString = "Error while resizeing ";
        _errorString += mFilename;
        return false;
    }

    size_t diskCount = 1;
    if (_numberOfDisks > mNumberOfDisks)
    {
        while (_numberOfDisks > 0) {

            if (_numberOfDisks > MMB_MAXNUMBEROFDISKS) {
                ndisks = MMB_MAXNUMBEROFDISKS;
            }
            else {
                ndisks = _numberOfDisks;
            }

            if (diskCount > mNumberOfDisks) {
                fwrite(firstDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, mFile);
            }
            else {
                fseek(mFile, MMB_DIRECTORYENTRYSIZE, SEEK_CUR);
            }


            // Write usable entries  
            for (size_t usable = 0; usable < ndisks; ++usable)
            {
                if (diskCount>mNumberOfDisks) {
                    fwrite(emptyDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, mFile);
                }
                else {
                    fseek(mFile, MMB_DIRECTORYENTRYSIZE, SEEK_CUR);
                }
                diskCount++;
            }

            // Write non-existant entries
            for (size_t entry = 0; entry < MMB_MAXNUMBEROFDISKS - ndisks; ++entry)
            {
                if (diskCount > mNumberOfDisks) {
                    fwrite(invalidDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, mFile);
                }
                else {
                    fseek(mFile, MMB_DIRECTORYENTRYSIZE, SEEK_CUR);
                }
                diskCount++;
            }

            fseek(mFile, MMB_DISKSIZE * ndisks, SEEK_CUR);
            _numberOfDisks -= ndisks;
        }
    }

    fseek(mFile, 8, SEEK_SET);
    if (chunks>1) chunks = 0xa0 + chunks - 1;
    fwrite(&chunks, 1, 1, mFile);

    CloseMMBFileInternal();

    mFileSize = newsize;
    mNumberOfChunks = (mFileSize + MMB_CHUNKSIZE - 1) / (MMB_DIRECTORYSIZE + (MMB_MAXNUMBEROFDISKS * MMB_DISKSIZE));
    mFileSize -= (mNumberOfChunks - 1) * MMB_CHUNKSIZE;

    mNumberOfDisks = (mNumberOfChunks - 1) * MMB_MAXNUMBEROFDISKS;
    if (mFileSize > 0) {
        mNumberOfDisks += (mFileSize - MMB_DIRECTORYSIZE) / MMB_DISKSIZE;
    }

    ReadDirectory();

    return true;
}

bool CMMBFile::ApplyBootOptionValues(size_t disk0, size_t disk1, size_t disk2, size_t disk3, std::string& _errorString)
{
    if (!OpenMMBFileInternal())
    {
        _errorString = "Could not open file ";
        Close();
        return false;
    }

    fseek(mFile, 0, SEEK_SET);

    char tmpChar = 0;

    tmpChar = disk0 % 256;
    fwrite(&tmpChar, 1, 1, mFile);
    tmpChar = disk1 % 256;
    fwrite(&tmpChar, 1, 1, mFile);
    tmpChar = disk2 % 256;
    fwrite(&tmpChar, 1, 1, mFile);
    tmpChar = disk3 % 256;
    fwrite(&tmpChar, 1, 1, mFile);

    tmpChar = disk0 / 256;
    fwrite(&tmpChar, 1, 1, mFile);
    tmpChar = disk1 / 256;
    fwrite(&tmpChar, 1, 1, mFile);
    tmpChar = disk2 / 256;
    fwrite(&tmpChar, 1, 1, mFile);
    tmpChar = disk3 / 256;
    fwrite(&tmpChar, 1, 1, mFile);

    CloseMMBFileInternal();

    mBoot0 = disk0;
    mBoot1 = disk1;
    mBoot2 = disk2;
    mBoot3 = disk3;
    return true;
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

bool CMMBFile::OpenMMBFileInternal()
{
    if( nullptr != mFile )
    {
        fclose( mFile );
    }

    mFile = fopen( mFilename.c_str(), "rb+" );
    
    return ( nullptr != mFile );
}

void CMMBFile::CloseMMBFileInternal()
{
    if( nullptr != mFile )
    {
        fclose( mFile );
        mFile = nullptr;
    }
}

void CMMBFile::ReadDirectory()
{
    if( !OpenMMBFileInternal() )
    {
        return;
    }

    ClearDirectory();


    
    // Read entries
    char tmpChar = 0;
    size_t bytesRead = 0;

    fseek(mFile, 0, SEEK_SET);

    bytesRead = fread(&tmpChar, 1, 1, mFile);
    mBoot0 = tmpChar;
    bytesRead = fread(&tmpChar, 1, 1, mFile);
    mBoot1 = tmpChar;
    bytesRead = fread(&tmpChar, 1, 1, mFile);
    mBoot2 = tmpChar;
    bytesRead = fread(&tmpChar, 1, 1, mFile);
    mBoot3 = tmpChar;

    bytesRead = fread(&tmpChar, 1, 1, mFile);
    mBoot0 += 256*tmpChar;
    bytesRead = fread(&tmpChar, 1, 1, mFile);
    mBoot1 += 256*tmpChar;
    bytesRead = fread(&tmpChar, 1, 1, mFile);
    mBoot2 += 256*tmpChar;
    bytesRead = fread(&tmpChar, 1, 1, mFile);
    mBoot3 += 256*tmpChar;

    if (mDirectory) delete[] mDirectory;
    mDirectory = new SMMBDirectoryEntry[mNumberOfChunks* MMB_MAXNUMBEROFDISKS];

    for (size_t chunk = 0; chunk < mNumberOfChunks; chunk++) {

        // seek to first entry of correct chunk
        fseek(mFile, MMB_DIRECTORYENTRYSIZE+MMB_CHUNKSIZE*chunk, SEEK_SET);
        for (size_t entry = 0; entry < MMB_MAXNUMBEROFDISKS; ++entry)
        {
            for (size_t nameChar = 0; nameChar < MMB_MAXDISKNAMELENGTH; ++nameChar)
            {
                bytesRead = fread(&tmpChar, 1, 1, mFile);
                mDirectory[entry + (chunk * MMB_MAXNUMBEROFDISKS)].name += tmpChar;
            }

            // Skip 3 bytes
            fseek(mFile, 3, SEEK_CUR);

            // Read disk atributes byte
            bytesRead = fread(&mDirectory[entry+(chunk*MMB_MAXNUMBEROFDISKS)].diskAttributes, 1, 1, mFile);
        }
    }

    CloseMMBFileInternal();
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

size_t CMMBFile::GetBoot0() const
{
    return mBoot0;
}

size_t CMMBFile::GetBoot1() const
{
    return mBoot1;
}

size_t CMMBFile::GetBoot2() const
{
    return mBoot2;
}

size_t CMMBFile::GetBoot3() const
{
    return mBoot3;
}

bool CMMBFile::InsertImageInSlot( const std::string& _filename, size_t _slot, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    // Open disk image to insert
    FILE* pFile = fopen( _filename.c_str(), "rb" );
    if( nullptr == pFile )
    {
        _errorString = "Could not open disk image file ";
        _errorString += _filename;
        CloseMMBFileInternal();
        return false;
    }

    // Allocate memory for storing the SSD image
    unsigned char* pImage = new unsigned char[MMB_DISKSIZE];
    if( nullptr == pImage )
    {
        fclose( pFile );
        CloseMMBFileInternal();
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
        CloseMMBFileInternal();
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


    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;

    fseek(mFile, MMB_CHUNKSIZE * chunk + ((dnum + 1) * MMB_DIRECTORYENTRYSIZE), SEEK_SET);
    fwrite( directoryEntry, 1, MMB_DIRECTORYENTRYSIZE, mFile );

    // Write disk image
    fseek(mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE), SEEK_SET);
    fwrite( pImage, 1, MMB_DISKSIZE, mFile );
    CloseMMBFileInternal();

    // Cleanup
    delete[] pImage;

    ReadDirectory();

    return true;
}

bool CMMBFile::InsertImageInSlot( const unsigned char* _data, size_t _dataSize, size_t _slot, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    // Write directory entry
    unsigned char directoryEntry[MMB_DIRECTORYENTRYSIZE];
    memset( directoryEntry, 0, MMB_DIRECTORYENTRYSIZE );

    directoryEntry[MMB_DIRECTORYENTRYSIZE - 1] = MMB_DISKATTRIBUTE_UNLOCKED;
    memcpy( &directoryEntry[0], _data, 8 );
    memcpy( &directoryEntry[8], &_data[256], 4 );

    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;

    fseek( mFile, MMB_CHUNKSIZE * chunk + ((dnum + 1) * MMB_DIRECTORYENTRYSIZE), SEEK_SET );
    fwrite( directoryEntry, 1, MMB_DIRECTORYENTRYSIZE, mFile );

    // Write disk image
    fseek(mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE), SEEK_SET);
    fwrite( _data, 1, MMB_DISKSIZE, mFile );
    CloseMMBFileInternal();

    ReadDirectory();

    return true;
}

bool CMMBFile::ExtractImageInSlot( const std::string& _filename, size_t _slot, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    // Open destination file
    FILE* pDestinationFile = fopen( _filename.c_str(), "wb" );
    if( nullptr == pDestinationFile )
    {
        _errorString = "Could not create/overwrite destination file ";
        _errorString = _filename;
        CloseMMBFileInternal();
        return false;
    }

    // Allocate memory for storing the disk image
    unsigned char* pImage = new unsigned char[MMB_DISKSIZE];
    if( nullptr == pImage )
    {
        _errorString = "Could not allocate memory for temporal storage.";
        CloseMMBFileInternal();
        return false;
    }

    size_t bytesRead = 0;
    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;

    fseek(mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE), SEEK_SET);
    bytesRead = fread( pImage, 1, MMB_DISKSIZE, mFile );
    fwrite( pImage, 1, MMB_DISKSIZE, pDestinationFile );

    fclose( pDestinationFile );
    CloseMMBFileInternal();
    delete[] pImage;

    return true;
}

bool CMMBFile::ExtractImageInSlot( unsigned char* _data, size_t _slot, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
        return false;
    }

    size_t bytesRead = 0;
    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;
    
    fseek( mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE), SEEK_SET );
    bytesRead = fread( _data, 1, MMB_DISKSIZE, mFile );

    CloseMMBFileInternal();
    
    return true;
}

bool CMMBFile::LockImageInSlot( size_t _slot, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;

    fseek( mFile, MMB_CHUNKSIZE * chunk + ((dnum + 2) * MMB_DIRECTORYENTRYSIZE) - 1, SEEK_SET );
    fwrite( &MMB_DISKATTRIBUTE_LOCKED, 1, 1, mFile );
    CloseMMBFileInternal();

    ReadDirectory();

    return true;
}

bool CMMBFile::UnlockImageInSlot( size_t _slot, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;

    fseek(mFile, MMB_CHUNKSIZE * chunk + ((dnum + 2) * MMB_DIRECTORYENTRYSIZE) - 1, SEEK_SET);
    fwrite( &MMB_DISKATTRIBUTE_UNLOCKED, 1, 1, mFile );
    CloseMMBFileInternal();

    ReadDirectory();

    return true;
}

bool CMMBFile::RemoveImageFromSlot( size_t _slot, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;

    // Clear directory entry
    fseek( mFile, MMB_CHUNKSIZE * chunk + (dnum + 1) * MMB_DIRECTORYENTRYSIZE, SEEK_SET );
    fwrite( emptyDirectoryEntry, 1, MMB_DIRECTORYENTRYSIZE, mFile );

    // Clear data area

    // Allocate memory for storing the empty data area
    unsigned char* pImage = new unsigned char[MMB_DISKSIZE];
    if( nullptr == pImage )
    {
        _errorString = "Could not allocate memory for temporal storage.";
        CloseMMBFileInternal();
        return false;
    }
    memset( pImage, 0, MMB_DISKSIZE );

    fseek( mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE), SEEK_SET );
    fwrite( pImage, 1, MMB_DISKSIZE, mFile );
    CloseMMBFileInternal();
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
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;
    unsigned char statusByte = 0;
    fseek( mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE) + ((_fileIndex + 1) * 8) + 7, SEEK_SET );
    size_t bytesRead = fread( &statusByte, 1, 1, mFile );
    statusByte |= 0x80;
    fseek( mFile, -1, SEEK_CUR );
    fwrite( &statusByte, 1, 1, mFile );
    CloseMMBFileInternal();
    
    return true;
}

bool CMMBFile::UnlockFile( size_t _slot, size_t _fileIndex, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;
    unsigned char statusByte = 0;
    fseek( mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE) + ((_fileIndex + 1) * 8) + 7, SEEK_SET );
    size_t bytesRead = fread( &statusByte, 1, 1, mFile );
    statusByte &= 0x7F;
    fseek( mFile, -1, SEEK_CUR );
    fwrite( &statusByte, 1, 1, mFile );
    CloseMMBFileInternal();

    return true;
}

bool CMMBFile::NameDisk( size_t _slot, const std::string& _diskName, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    std::string finalName = _diskName;
    if( finalName.length() > 12 )
    {
        finalName = finalName.substr( 0, 12 );
    }
    else if( finalName.length() < 12 )
    {
        finalName.insert( finalName.end(), 12 - finalName.length(), ' ' );
    }

    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;

    fseek( mFile, MMB_CHUNKSIZE * chunk + (dnum + 1) * MMB_DIRECTORYENTRYSIZE, SEEK_SET );
    fwrite( finalName.c_str(), 1, 12, mFile );

    fseek( mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE), SEEK_SET );
    fwrite( finalName.c_str(), 1, 8, mFile );

    fseek( mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE) + MMB_SECTORSIZE, SEEK_SET );
    fwrite( &finalName.c_str()[8], 1, 4, mFile );
    CloseMMBFileInternal();

    ReadDirectory();

    return true;
}

bool CMMBFile::SetBootOption( size_t _slot, unsigned char _bootOption, std::string& _errorString )
{
    if( !OpenMMBFileInternal() )
    {
        _errorString = "No MMB file opened.";
        Close();
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
        CloseMMBFileInternal();
        return false;
    }

    size_t chunk = _slot / MMB_MAXNUMBEROFDISKS;
    size_t dnum = _slot % MMB_MAXNUMBEROFDISKS;
    unsigned char optionsByte = 0;
    fseek( mFile, MMB_CHUNKSIZE * chunk + MMB_DIRECTORYSIZE + (dnum * MMB_DISKSIZE) + MMB_SECTORSIZE + 6, SEEK_SET );
    size_t bytesRead = fread( &optionsByte, 1, 1, mFile );

    _bootOption &= 0x03;
    optionsByte &= 0xCF;
    optionsByte |= (_bootOption << 4);

    fseek( mFile, -1, SEEK_CUR );
    fwrite( &optionsByte, 1, 1, mFile );
    CloseMMBFileInternal();

    return true;
}
