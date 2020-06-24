#pragma once
#include <string>

const unsigned char MMB_DISKATTRIBUTE_INVALID     = 0xFF; // Disk does not exist
const unsigned char MMB_DISKATTRIBUTE_UNFORMATTED = 0xF0; // Unformatted
const unsigned char MMB_DISKATTRIBUTE_UNLOCKED    = 0x0F; // Unlocked
const unsigned char MMB_DISKATTRIBUTE_LOCKED      = 0x00; // Locked
const size_t        MMB_MAXDISKNAMELENGTH         = 12;   // As per the Acorn Disk Filing System
const size_t        MMB_MAXNUMBEROFDISKS          = 511;
const size_t        MMB_DIRECTORYSIZE             = 8192;
const size_t        MMB_DIRECTORYENTRYSIZE        = 16;
const size_t        MMB_DISKSIZE                  = 200 * 1024;

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

    bool InsertImageInSlot  ( const std::string& _filename, size_t _slot, std::string& _errorString );
    bool ExtractImageInSlot ( const std::string& _filename, size_t _slot, std::string& _errorString );
    bool LockImageInSlot    ( size_t _slot, std::string& _errorString );
    bool UnlockImageInSlot  ( size_t _slot, std::string& _errorString );
    bool RemoveImageFromSlot( size_t _slot, std::string& _errorString );

    const std::string& GetFilename();
    
    const char*   GetEntryName     ( size_t _entry );
    unsigned char GetEntryAttribute( size_t _entry );

private:
    void ReadDirectory();
    void ClearDirectory();

    std::string mFilename;
    FILE* mFile = nullptr;
    size_t mFileSize = 0;
    size_t mNumberOfDisks = 0;
    SMMBDirectoryEntry mDirectory[MMB_MAXNUMBEROFDISKS];
};