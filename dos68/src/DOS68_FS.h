#ifndef __DOS68_FS__
#define __DOS68_FS__

/////////////////////////////////////////////////////////////////////////////////////////////
//
// DOS68_FS.h - Header file for CDOS68_FS, a file system class 
//              for SWTPC DOS68 formatted disk images.
//
// For info on the DOS68 file system go to:
//              https://deramp.com/downloads/swtpc/software/DOS68/Manuals/DOS09%20Manual.pdf
//              (DOS09 is almost identical to DOS68, but keep an eye on differences)
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
/////////////////////////////////////////////////////////////////////////////////////////////


#include "DiskImageInterface.h"
#include "FileSystemInterface.h"
#include <vector>
#include <string>

using namespace std;

#define DOS68_SECTORS_PER_TRACK         18
#define DOS68_SECTOR_SIZE               128
#define DOS68_SECTOR_DATA_SIZE          124
#define DOS68_MAX_SECTOR_ID             17
#define DOS68_DIR_START_SIDE            0
#define DOS68_DIR_START_TRACK           0
#define DOS68_DIR_START_SECTOR          1
#define DOS68_MAX_FILE_NAME_LEN         6
#define DOS68_MAX_FILE_EXT_LEN          3
#define DOS68_MAX_FILE_FULL_NAME_LEN    10 // 6 (name) + 1 (.) + 3 (ext)
#define DOS68_FIB_SIZE                  24
#define DOS68_FIBS_PER_SECTOR           5
#define DOS68_TRACK_ID_MASK             0x7F
#define DOS68_SECTOR_ID_MASK            0x1F
#define DOS68_TRACK_ID_MARK             0x80
#define DOS68_SECTOR_ID_MARK            0x40
#define DOS68_DISK_TYPE_SIDES_MASK      (1 << 0)
#define DOS68_DISK_TYPE_INCHES_MASK     (1 << 1)
#define DOS68_DISK_TYPE_DENSITY_MASK    (1 << 2)
#define DOS68_DISK_TYPE_TPI_MASK        (1 << 3)
#define DOS68_DISK_TYPE_PROTECTED_MASK  (1 << 7)
#define DOS68_DISK_TYPE_SINGLE_SIDE     0
#define DOS68_DISK_TYPE_DOUBLE_SIDE     1
#define DOS68_DISK_TYPE_5_INCHES        0
#define DOS68_DISK_TYPE_8_INCHES        1
#define DOS68_DISK_TYPE_SINGLE_DENSITY  0
#define DOS68_DISK_TYPE_DOUBLE_DENSITY  1
#define DOS68_DISK_TYPE_48_TPI          0
#define DOS68_DISK_TYPE_96_TPI          1
#define DOS68_DISK_TYPE_UNPROTECTED     0
#define DOS68_DISK_TYPE_PROTECTED       1 // Also called a 'MASTER' disk.
#define DOS68_FILE_TYPE_SEQ_ASCII       1 // Lower 4 bits of file type
#define DOS68_FILE_TYPE_SEQ_BINARY      2
#define DOS68_FILE_TYPE_RND_BYTE_MODE   4
#define DOS68_FILE_TYPE_RND_RECORD_MODE 5
#define DOS68_FILE_STATUS_NOT_ACTIVE    0 // Lower 4 bits of file status
#define DOS68_FILE_STATUS_SEQ_READ      1
#define DOS68_FILE_STATUS_SEQ_WRITE     2
#define DOS68_FILE_STATUS_RND_ACCESS    3

struct SDOS68_FileInfoBlock
{
    string name;
    string ext;
    string fullName;
    uint8_t type = 0;
    uint8_t status = 0;
    uint8_t firstTrack = 0;
    uint8_t firstSector = 0; 
    uint8_t lastTrack = 0;
    uint8_t lastSector = 0; 
    uint8_t sectorsNumHigh = 0;
    uint8_t sectorsNumLow = 0;
    uint8_t directoryTrack = 0;
    uint8_t directorySector = 0;
    uint8_t directoryIndex = 0;
};

// DOS68 File system
class CDOS68_FS : public IFilesystemInterface
{
public:
    CDOS68_FS() = default;
    virtual ~CDOS68_FS() = default;

    // FileSystemInterface ///////////////////////////////////////////////////
    bool Load( IDiskImageInterface* _disk );
    bool Save( const string& _filename );

    const CDirectoryEntryWrapper& GetFSRoot() const;

    size_t GetFilesNum();
    string GetFileName( size_t _fileIdx );

    SFileInfo GetFileInfo( size_t _fileIdx );

    bool ExtractFile( string _fileName, vector<unsigned char>& _dst );
    bool InsertFile ( string _fileName, const vector<unsigned char>& src, bool _binaryFile );
    bool RemoveFile ( string _fileName );

    string GetFSName();
    string GetFSVariant();
    string GetVolumeLabel() const;
    //////////////////////////////////////////////////////////////////////////

    uint8_t  GetDiskType();
    uint8_t  GetNextAvailableTrack();
    uint8_t  GetNextAvailableSector();
    uint8_t  GetLastAvailableTrack();
    uint8_t  GetLastAvailableSector();
    uint16_t GetAvailableSectorsNum();

    SDOS68_FileInfoBlock GetFIB( size_t _fileIdx );

    void SetAvailableSectorsNum( uint16_t _sectorsNum );

private:
    bool UpdateDiskInformationBlock();
    void ComputeNextAndPreviousSectorParams( uint8_t _trackId, uint8_t _sectorId, uint8_t* _dst );

    IDiskImageInterface* mDisk = nullptr;
    vector<SDOS68_FileInfoBlock> mDirectory;
    CDirectoryEntryWrapper mDummyDirEntryWrapper;

    uint8_t mDiskType = 0;
    uint8_t mNextAvailableBlockTrack = 0;
    uint8_t mNextAvailableBlockSector = 0;
    uint8_t mLastAvailableBlockTrack = 0;
    uint8_t mLastAvailableBlockSector = 0;
    uint8_t mAvailableSectorsNumHigh = 0;
    uint8_t mAvailableSectorsNumLow = 0;
};

#endif