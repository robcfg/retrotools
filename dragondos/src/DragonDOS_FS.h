////////////////////////////////////////////////////////////////////
//
// DragonDOS_FS.h - Header file for CDragonDOS_FS, a helper class 
//                  that allows file operations on a disk image
//                  formatted with the DragonDOS file system.
//
// For info on the DragonDOS file system go to:
//              http://dragon32.info/info/drgndos.txt
//              http://dragon32.info/info/binformt.txt
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Last update: 12/11/2023
//
////////////////////////////////////////////////////////////////////
#pragma once

#include "DiskImageInterface.h"
#include "FileSystemInterface.h"
#include <vector>
#include <string>

#define DRAGONDOS_DIR_TRACK                 20   // Directory track
#define DRAGONDOS_TEMP_DIR_TRACK            16   // Track to perform operations before copying it to track 20.
#define DRAGONDOS_DIR_ENTRY_SIZE            25   // Directory entry size
#define DRAGONDOS_DIR_START_SECTOR          2    // Start sector of directory info on directory track
#define DRAGONDOS_DIR_ENTRIES_PER_SECT      10   // Number of directory entries per sector
#define DRAGONDOS_DIR_MAX_ENTRIES           160  // Maximum number of entries in a DragonDOS directory track
#define DRAGONDOS_FLAG_DELETED              0x80 // bit 7	Deleted - this entry may be reused
#define DRAGONDOS_FLAG_UNUSED6              0x40 // bit 6	Unused
#define DRAGONDOS_FLAG_CONTINUED            0x20 // bit 5	Continued - byte at offset 0x18 gives next entry number
#define DRAGONDOS_FLAG_UNUSED4              0x10 // bit 4	Unused
#define DRAGONDOS_FLAG_ENDOFDIR             0x08 // bit 3	End of Directory - no further entries need to be scanned
#define DRAGONDOS_FLAG_UNUSED2              0x04 // bit 2	Unused
#define DRAGONDOS_FLAG_PROTECTED            0x02 // bit 1	Protect Flag - file should not be overwritten
#define DRAGONDOS_FLAG_CONTINUATION         0x01 // bit 0	Continuation Entry - this entry is a Continuation Block
#define DRAGONDOS_FILETYPE_DATA             0x00 // .DAT file type
#define DRAGONDOS_FILETYPE_BASIC            0x01 // .BAS file type
#define DRAGONDOS_FILETYPE_BINARY           0x02 // .BIN file type
#define DRAGONDOS_FILEHEADER_SIZE           9
#define DRAGONDOS_SECTOR_SIZE               256
#define DRAGONDOS_SECTORSPERTRACK           18
#define DRAGONDOS_MAX_FILE_NAME_LEN         8
#define DRAGONDOS_MAX_FILE_EXT_LEN          3
#define DRAGONDOS_MAX_FILE_FULL_NAME_LEN    (DRAGONDOS_MAX_FILE_NAME_LEN + DRAGONDOS_MAX_FILE_EXT_LEN + 1)
#define DRAGONDOS_MAX_FILE_SIZE             0xFFFF
#define DRAGONDOS_SECTORSPERBITMAPSECTOR    0x5A0
#define DRAGONDOS_FILE_HEADER_BEGIN         0x55
#define DRAGONDOS_FILE_HEADER_END           0xAA
#define DRAGONDOS_BITMAPSIZE                180

#define DRAGONDOS_INVALID                   0xFFFF // To signal invalid indices. DragonDOS can only have 160 entries max.

// Dragon DOS File Allocation Block
struct SDGNDosFAB
{
    unsigned short int LSN;         // Logical sector number
    unsigned char      numSectors;  // Number of contiguous sectors
};

// Dragon DOS File/Continuation block
struct SDGNDosFileBlock
{
    std::string              fileName;
    std::vector<SDGNDosFAB>  FABs;
};

// Dragon DOS directory entry
struct SDGNDosDirectoryEntry
{
    bool                      bProtected;
    bool                      bDeleted;
    bool                      bContinuation;
    bool                      bContinued;
    SDGNDosFileBlock          fileBlock;

    unsigned char             fileType;
    unsigned short int        fileSize;
    unsigned short int        loadAddress;
    unsigned short int        execAddress;
    unsigned short int        lastSectorSize;
    unsigned char             nextBlock;

    // Location of the directory entry on the directory track
    unsigned int              sector;
    unsigned int              entry;

    SDGNDosDirectoryEntry()
    {
        bProtected     = false;
        bDeleted       = false;
        bContinuation  = false;
        bContinued     = false;
        fileType       = 0;
        fileSize       = 0;
        loadAddress    = 0;
        execAddress    = 0;
        lastSectorSize = 0;
        nextBlock      = 0;
        sector         = 0;
        entry          = 0;
    }
};

// DragonDOS File
class CDGNDosFile
{
public:
    CDGNDosFile() {}
    ~CDGNDosFile() {}

    std::string        GetFileName      () const                                    { return fileName;      }
    void               SetFileName      ( const std::string& _fileName )            { fileName = _fileName; }
    void               GetFileData      ( std::vector<unsigned char>& dst ) const;
    void               SetFileData      ( const std::vector<unsigned char>& src );
    void               SetFileData      ( const unsigned char* src, size_t size );
    size_t             GetFileSize      () const                                    { return data.size(); }
    bool               GetFileProtected ()                                          { return bProtected; }
    void               SetFileProtected ( bool _protected )                         { bProtected = _protected; }
    unsigned char      GetFileType      () const                                    { return fileType; }
    void               SetFileType      ( unsigned char _fileType )                 { fileType = _fileType; }
    unsigned short int GetLoadAddress   () const                                    { return loadAddress; }
    void               SetLoadAddress   ( unsigned short int _loadAddress )         { loadAddress = _loadAddress; }
    unsigned short int GetExecAddress   () const                                    { return execAddress; }
    void               SetExecAddress   ( unsigned short int _execAddress )         { execAddress = _execAddress; }

private:
    std::string                fileName;
    std::vector<unsigned char> data;
    bool                  bProtected;
    unsigned char         fileType;
    unsigned short int    loadAddress;
    unsigned short int    execAddress;
};

// DragonDOS File system
class CDragonDOS_FS : public IFilesystemInterface
{
public:
    CDragonDOS_FS();
    virtual ~CDragonDOS_FS();

    bool                  SetDisk         ( IDiskImageInterface* _disk );
    IDiskImageInterface*  GetDisk         ()                              { return disk; }
    unsigned short int    GetNumberOfFiles() const                        { return (unsigned short int)files.size(); }
    unsigned short int    GetFileIdx      ( std::string _fileName ) const;
    unsigned short int    GetFileEntry    ( std::string _fileName ) const;
    const CDGNDosFile&    GetFile         ( unsigned short int fileIdx )  { if(fileIdx < files.size()) return files[fileIdx]; return emptyFile; }
    bool                  InsertFile      ( std::string _fileName, const std::vector<unsigned char>& _data );
    bool                  DeleteFile      ( std::string _fileName );
    bool                  Save            ( std::string _fileName );

    std::string GetFileTypeString( unsigned short int fileIdx ) const;

    const std::vector<SDGNDosDirectoryEntry>& GetDirectory() { return directory; }

    // IFilesystemInterface
    bool Load(IDiskImageInterface* _disk);
    bool Save(const std::string& _filename);

    const CDirectoryEntryWrapper& GetFSRoot() const;

    size_t      GetFilesNum() const;
    std::string GetFileName(size_t _fileIdx) const;
    size_t      GetFileSize( size_t _fileIdx ) const;
    size_t      GetFreeSize() const;

    SFileInfo   GetFileInfo(size_t _fileIdx);
    bool        ExtractFile( std::string _fileName, std::vector<unsigned char>& _dst, bool _withBinaryHeader ) const;

    std::string GetFSName() const;
    std::string GetFSVariant() const;
    std::string GetVolumeLabel() const;

    bool InitDisk( IDiskImageInterface* _disk );

private:
    IDiskImageInterface*           disk;
    std::vector<SDGNDosDirectoryEntry>  directory;
    std::vector<CDGNDosFile>            files;

    CDGNDosFile                    emptyFile;

    CDirectoryEntryWrapper         rootDir;

    bool                ParseDirectory();
    bool                ParseFiles    ();
    bool                BackUpDirTrack();

    unsigned char GetMaxContiguousFreeSectorsInBitmapByte( unsigned char _bitmapByte );
    bool          IsBitmapLSNFree  ( size_t _LSN );
    void          MarkBitmapLSNFree( size_t _LSN );
    void          MarkBitmapLSNUsed( size_t _LSN );
};
