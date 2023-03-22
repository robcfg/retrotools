#pragma once
#include <string>

const unsigned char MMB_DISKATTRIBUTE_INVALID     = 0xFF; // Disk does not exist
const unsigned char MMB_DISKATTRIBUTE_UNFORMATTED = 0xF0; // Unformatted
const unsigned char MMB_DISKATTRIBUTE_UNLOCKED    = 0x0F; // Unlocked
const unsigned char MMB_DISKATTRIBUTE_LOCKED      = 0x00; // Locked
const size_t        MMB_MAXDISKNAMELENGTH         = 12;   // As per the Acorn Disk Filing System
const size_t        MMB_MAXNUMBEROFDISKS          = 511;
const size_t        MMB_MAXNUMBEROFDISKS2         = 511*16;
const size_t        MMB_DIRECTORYSIZE             = 8192;
const size_t        MMB_DIRECTORYENTRYSIZE        = 16;
const size_t        MMB_DISKSIZE                  = 200 * 1024;
const size_t        MMB_CHUNKSIZE                 = MMB_DIRECTORYSIZE + (MMB_MAXNUMBEROFDISKS * MMB_DISKSIZE);
const size_t        MMB_SECTORSIZE                = 256;

struct SMMBDirectoryEntry
{
    std::string name;
    unsigned char diskAttributes = MMB_DISKATTRIBUTE_INVALID;
};

class CMMBFile
{
public:
    CMMBFile();
    virtual ~CMMBFile();

    bool Open  ( const std::string& _filename, std::string& _errorString );
    bool Create( const std::string& _filename, size_t _numberOfDisks, std::string& _errorString ) const;
    void Close ();


    const SMMBDirectoryEntry* GetDirectory();
    size_t GetNumberOfDisks() const;

    bool Resize(size_t _numberOfDisks, std::string& _errorString);
    size_t GetDriveBootDisk( size_t _drive ) const;
    void   SetDriveBootDisk( size_t _drive, size_t _disk );
    bool ApplyBootOptionValues(size_t disk0, size_t disk1, size_t disk2, size_t disk3, std::string& _errorString);

    bool InsertImageInSlot  ( const std::string& _filename, size_t _slot, std::string& _errorString );
    bool InsertImageInSlot  ( const unsigned char* _data, size_t _dataSize, size_t _slot, std::string& _errorString );
    bool ExtractImageInSlot ( const std::string& _filename, size_t _slot, std::string& _errorString );
    bool ExtractImageInSlot ( unsigned char* _data, size_t _slot, std::string& _errorString );
    bool LockImageInSlot    ( size_t _slot, std::string& _errorString );
    bool UnlockImageInSlot  ( size_t _slot, std::string& _errorString );
    bool RemoveImageFromSlot( size_t _slot, std::string& _errorString );

    const std::string& GetFilename();
    
    const char*   GetEntryName     ( size_t _entry );
    unsigned char GetEntryAttribute( size_t _entry );

    bool NameDisk     ( size_t _slot, const std::string& _diskName, std::string& _errorString );
    bool LockFile     ( size_t _slot, size_t _fileIndex, std::string& _errorString );
    bool UnlockFile   ( size_t _slot, size_t _fileIndex, std::string& _errorString );
    bool SetBootOption( size_t _slot, unsigned char _bootOption, std::string& _errorString );
    
private:
    bool OpenMMBFileInternal();
    void CloseMMBFileInternal();
    void ReadDirectory();
    void ClearDirectory();

    std::string mFilename;
    FILE* mFile = nullptr;
    size_t mFileSize = 0;
    size_t mNumberOfDisks = 0;
    size_t mNumberOfChunks = 0;
    size_t mDriveBootDisks[4] = { 0, 1, 2, 3 };
    SMMBDirectoryEntry *mDirectory = 0;
};