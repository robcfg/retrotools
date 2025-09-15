#ifndef __OS9RBF_FS__
#define __OS9RBF_FS__

////////////////////////////////////////////////////////////////////
//
// OS9RBF_FS.h - Header file for COS9RBF_FS, a helper class 
//               that allows file operations on a disk image
//               formatted with OS-9 RBF file system.
//
// For info on the OS-9 RBF file system go to:
//               https://colorcomputerarchive.com/repo/Documents/Manuals/Operating%20Systems/Allen-Bradley%20OS-9%20Technical%20(Microware%20Systems%20Corp).pdf
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
////////////////////////////////////////////////////////////////////

#include "DiskImageInterface.h"
#include "FileSystemInterface.h"
#include <vector>
#include <string>

#define OS9RBF_INVALID 0xFFFF // To signal invalid values.

#define OFF_DD_TOT     0x00
#define OFF_DD_TKS     0x03
#define OFF_DD_MAP     0x04
#define OFF_DD_BIT     0x06
#define OFF_DD_DIR     0x08
#define OFF_DD_OWN     0x0B
#define OFF_DD_ATT     0x0D
#define OFF_DD_DSK     0x0E
#define OFF_DD_FMT     0x10
#define OFF_DD_SPT     0x11
#define OFF_DD_RES     0x13
#define OFF_DD_BT      0x15
#define OFF_DD_BSZ     0x18
#define OFF_DD_DAT     0x1A
#define OFF_DD_NAM     0x1F
#define OFF_DD_OPT     0x3F
#define OFF_DD_RES2    0x5F
#define OFF_DD_SYNC    0x60
#define OFF_DD_MapLSN  0x64
#define OFF_DD_LSNSize 0x68
#define OFF_DD_VersID  0x6A

#define OFF_FD_ATT   0x00
#define OFF_FD_OWN   0x01
#define OFF_FD_DAT   0x03
#define OFF_FD_LNK   0x08
#define OFF_FD_SIZ   0x09
#define OFF_FD_CREAT 0x0D
#define OFF_FD_SEG   0x10

#define ATT_FD_DIRECTORY  128 // D, directory
#define ATT_FD_NONSHARED  64  // S, non-shareable file
#define ATT_FD_PUBEXECUTE 32  // PE, public executable
#define ATT_FD_PUBWRITE   16  // PW, public writable
#define ATT_FD_PUBREAD    8   // PR, public readable
#define ATT_FD_EXECUTE    4   // E, executable
#define ATT_FD_WRITE      2   // W, writable
#define ATT_FD_READ       1   // R, readable

#define FD_DIR_SIZE       32
#define FD_DIR_NAME_SIZE  28
#define OFF_FD_DIR_LSN    29
#define FD_SEG_SIZE       5

struct SIdSector
{
    unsigned int        DD_TOT;     // Total number of sectors on media
    unsigned char       DD_TKS;     // Track size in sectors
    unsigned short int  DD_MAP;     // Number of bytes in allocation map
    unsigned short int  DD_BIT;     // Number of sectors/bit (cluster size)
    unsigned long int   DD_DIR;     // LSN of root directory file descriptor
    unsigned short int  DD_OWN;     // Owner ID
    unsigned char       DD_ATT;     // Attributes
    unsigned short int  DD_DSK;     // Disk ID
    unsigned char       DD_FMT;     // Disk Format; density/sides
                                    //      Bit 0: 0 = single side
                                    //             1 = double side
                                    //      Bit 1: 0 = single density (FM)
                                    //             1 = double density (MFM)
                                    //      Bit 2: 1 = double track (96 TPI/135 TPI)
                                    //      Bit 3: 1 = quad track density (192 TPI)
                                    //      Bit 4: 1 = octal track density (384 TPI)
    unsigned short int  DD_SPT;     //  Sectors/track (two byte value DD_TKS)
    unsigned short int  DD_RES;     // Reserved for future use
    unsigned long int   DD_BT;      // System bootstrap LSN
    unsigned short int  DD_BSZ;     // Size of system bootstrap
    unsigned char       DD_DAT[5];  // Creation date
    char                DD_NAM[33]; // Volume name
    char                DD_OPT[32]; // Path descriptor options
    unsigned char       DD_RES2;    // Reserved
    unsigned long int   DD_SYNC;    // Media integrity code
    unsigned long int   DD_MapLSN;  // Bitmap starting sector number (0=LSN 1)
    unsigned short int  DD_LSNSize; // Media logical sector size (0=256)
    unsigned short int  DD_VersID;  // Sector 0 Version ID
};

struct SFileDescriptorSegment
{
    unsigned long int LSN;
    unsigned short int size;
};

class CFileDescriptor : public CDirectoryEntryWrapper
{
public:
    CFileDescriptor() {}
    ~CFileDescriptor() {}

    void Load( IDiskImageInterface* _disk, unsigned long int _lsn, size_t _sectorSize );

    unsigned long int GetFileSize() const { return FD_SIZ; }
    const std::vector<SFileDescriptorSegment>& GetFileSegments() const { return segments; }

private:
    unsigned char      FD_ATT;      //  File Attributes: D S PE PW PR E W R
                                    //        D - file is a directory
                                    //        E - only owner can execute
                                    //        R - only owner can read
                                    //        S - non-shareable file
                                    //        W - only owner can write
                                    //        A 'p' in front of e,r, or w means anyone (public) can access file
    unsigned short int FD_OWN;      //  Owner’s User ID
    unsigned char      FD_DAT[5];   // Date Last Modified: Y M D H M
    unsigned char      FD_LNK;      // Link Count
    unsigned long int  FD_SIZ;      // File Size (number of bytes)
    unsigned char      FD_CREAT[3]; // Date Created: Y M D
                                    //$10 240 FD_SEG Segment List: see below
    std::vector<SFileDescriptorSegment> segments;
    std::vector<CFileDescriptor> entries;
};

struct SOS9RBFFile
{
    std::string                name;
    std::vector<unsigned char> data;
};

// OS-9 RBF File system
class COS9RBF_FS : public IFileSystemInterface
{
public:
    COS9RBF_FS();
    virtual ~COS9RBF_FS();

    bool                  SetDisk         ( IDiskImageInterface* _disk );
    IDiskImageInterface*  GetDisk         ()                              { return disk; }

	// IFileSystemInterface //////////////////////////////////////////////////////////////////////////////////
	bool Load(IDiskImageInterface* _disk);
	bool Save(const std::string& _filename);

	size_t      GetFilesNum() const;
	std::string GetFileName(size_t _fileIdx) const;
	size_t      GetFileSize( size_t _fileIdx ) const;
	size_t      GetFreeSize() const;

	SFileInfo   GetFileInfo(size_t _fileIdx) const;

	std::string GetFSName() const;
	std::string GetFSVariant() const;

	std::string GetVolumeLabel() const;

	const CDirectoryEntryWrapper& GetFSRoot() const;

	bool ExtractFile( const std::string& _fileName, std::vector<unsigned char>& dst, bool _withBinaryHeader ) const;
	bool InsertFile ( const std::string& _fileName, const std::vector<unsigned char>& src, bool _binaryFile );
	bool DeleteFile ( const std::string& _fileName );

	bool NeedManualSetup() { return false; }

	bool InitDisk( IDiskImageInterface* _disk );

	IFileSystemInterface* NewFileSystem();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    IDiskImageInterface*    disk;
    SIdSector               idSector;

    CFileDescriptor         root;

    bool                    ParseDirectory();
    void                    ParseFiles    ();
    void                    ParseFileNode ( const CDirectoryEntryWrapper& _entry, const std::string& _parentName );
    unsigned short int      GetFileEntry  ( std::string _fileName );

    std::vector<SOS9RBFFile> mFiles;
};

#endif