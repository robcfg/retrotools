#ifndef __FAT12_FS__
#define __FAT12_FS__

///////////////////////////////////////////////////////////////////////
//
// FAT12_FS.h - Header file for CFAT12_FS, a helper class that allows
//              file operations on a disk image formatted with the 
//              FAT12 file system.
//
// By Roberto Carlos Fernandez Gerhardt aka robcfg
//
// Notes:
//
// Information:
//   http://www.disc.ua.es/~gil/FAT12Description.pdf
//   http://www.maverick-os.dk/FileSystemFormats/FAT12_FileSystem.html
//
///////////////////////////////////////////////////////////////////////

#include "FileSystemInterface.h"
#include <vector>
#include <string>

#define FAT12_VOLUME_LABEL_LENGTH	11

#define SFAT12Attribute_ReadOnly    0x01
#define SFAT12Attribute_Hidden      0x02
#define SFAT12Attribute_System      0x04
#define SFAT12Attribute_VolumeLabel 0x08
#define SFAT12Attribute_Subdir		0x10
#define SFAT12Attribute_Archive     0x20
#define SFAT12Attribute_Invalid     0xFF

struct SFAT12_BootSector
{
	unsigned short int bytesPerSector;
	unsigned char      sectorsPerCluster;
	unsigned short int reservedSectorsNum;
	unsigned char      numberOfFATs;
	unsigned short int maxRootDirEntries;
	unsigned short int totalSectorCount;
	// unsigned char      ignore;
	unsigned short int sectorsPerFAT;
	unsigned short int sectorsPerTrack;
	unsigned short int numberOfHeads;
	// unsigned int       ignore;
	unsigned int       totalSectorCountFAT32;
	//unsigned short int ignore;
	unsigned char      bootSignature;
	unsigned int       volumeID;
	unsigned char      volumeLabel[FAT12_VOLUME_LABEL_LENGTH+1];
	unsigned char      fsType[9];
};

struct SFAT12_Directory
{
	unsigned char      name[9];   // 8+1 as string terminator
	unsigned char      ext[4];    // 3+1 as string terminator
	unsigned char      attributes;
	unsigned short int reserved;
	unsigned short int creationTime;
	unsigned short int creationDate;
	unsigned short int lastAccessDate;
	unsigned short int ignore;
	unsigned short int lastWriteTime;
	unsigned short int lastWriteDate;
	unsigned short int firstLogicalCluster;
	unsigned int       fileSize;

	std::vector<SFAT12_Directory> children;
};

class CFAT12_FS : public IFileSystemInterface
{
public:
	CFAT12_FS();
	virtual ~CFAT12_FS();

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

	IFileSystemInterface* NewFileSystem() { return new CFAT12_FS; }
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
	void ExploreDirectory( SFAT12_Directory& _dir );

    void ExportHierarchy();
    void ExportDirectoryEntry( const SFAT12_Directory& _source , CDirectoryEntryWrapper* _target );
    bool IsDirectory( const SFAT12_Directory& _entry ) const;

    void FAT12_FindDirectoryEntry(	const SFAT12_Directory& 	_parent, 
                                	std::vector<std::string>&   _tokens, 
                                	size_t                  	_curToken, 
                                	SFAT12_Directory&       	_dst ) const;

	IDiskImageInterface* 							disk;
	SFAT12_BootSector                   			bs;
	std::vector<SFAT12_Directory>            		directory;
	std::vector<std::vector<unsigned short int> >	fats;

    CDirectoryEntryWrapper         rootDir;
};

#endif