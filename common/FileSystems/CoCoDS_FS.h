#pragma once
#ifndef _COCODS_FS_
#define _COCODS_FS_

////////////////////////////////////////////////////////////////////
//
// CoCoDS_FS.h - Header file for CCoCoDS_FS, a helper class that
//               allows file operations on a disk image formatted
//               with the Tandy Disk Extended Color Basic file
//				 system.
//
// For info on the DragonDOS file system go to:
//             http://dragon32.info/info/drgndos.txt
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
////////////////////////////////////////////////////////////////////

#include "FileSystemInterface.h"
#include <vector>
#include <string>

#define COCODS_DISK_SIDES_NUM              1
#define COCODS_DISK_TRACKS_NUM             35
#define COCODS_DISK_SECTORS_PER_TRACK      18
#define COCODS_DISK_SECTOR_SIZE            256
#define COCODS_DISK_MAX_GRANULES           68
#define COCODS_DISK_GRANULE_SECTORS_NUM    9    // Number of sectors per granule
#define COCODS_DISK_GRANULE_SIZE           COCODS_DISK_GRANULE_SECTORS_NUM*COCODS_DISK_SECTOR_SIZE
#define COCODS_DISK_DIRECTORY_TRACK        17   // Starting from 0
#define COCODS_DIR_FAT_SECTOR              1    
#define COCODS_DIR_START_SECTOR            2    // Start sector of directory info on directory track
#define COCODS_DIR_SECTORS_NUM             9    // Number of sectors of the directory
#define COCODS_DIR_ENTRIES_PER_SECTOR      8    // Number of directory entries per sector
#define COCODS_DIR_ENTRY_SIZE              32   // Size of directory entry in bytes
#define COCODS_DIR_ENTRY_UNUSED            0
#define COCODS_DIR_ENTRIES_END             0xFF
#define COCODS_DIR_NAME_SIZE               8
#define COCODS_DIR_EXT_SIZE                3
#define COCODS_FILETYPE_BASIC_PROGRAM      0    // Basic program
#define COCODS_FILETYPE_BASIC_DATA         1    // Basic data file
#define COCODS_FILETYPE_MACHINE_LANGUAGE   2    // Machine language program
#define COCODS_FILETYPE_TEXT_EDITOR_SOURCE 3    // Text editor source file
#define COCODS_FILEFORMAT_BINARY           0
#define COCODS_FILEFORMAT_ASCII            0xFF

#define COCODS_INVALID                     0xFFFF

struct SCoCoDSDirectoryEntry
{
	std::string name;
	std::string extension;
	unsigned char type;
	unsigned char format;
	unsigned char firstGranule;
	unsigned short int bytesInLastSector;
};

// DragonDOS File
class CCoCoDS_File
{
public:
	CCoCoDS_File() {}
	~CCoCoDS_File() {}

	std::string        GetFileName      	() const                                    { return fileName;      }
	void               SetFileName      	( const std::string& _fileName )            { fileName = _fileName; }
	void               GetFileData      	( std::vector<unsigned char>& dst ) const;
	void               SetFileData      	( const std::vector<unsigned char>& src );
	void               SetFileData      	( const unsigned char* src, size_t size );
	size_t             GetFileSize      	() const                                    { return data.size(); }
	bool               GetFileProtected 	()                                          { return bProtected; }
	void               SetFileProtected 	( bool _protected )                         { bProtected = _protected; }
	unsigned char      GetFileType 			() const                                    { return fileType; }
	std::string 	   GetFileTypeString	() const;
	void               SetFileType 			( unsigned char _fileType )                 { fileType = _fileType; }
	unsigned char      GetFileFormat 		() const                                    { return fileFormat; }
	std::string 	   GetFileFormatString	() const;
	void               SetFileFormat    	( unsigned char _fileFormat )               { fileFormat = _fileFormat; }
	unsigned short int GetLoadAddress   	() const                                    { return loadAddress; }
	void               SetLoadAddress   	( unsigned short int _loadAddress )         { loadAddress = _loadAddress; }
	unsigned short int GetExecAddress   	() const                                    { return execAddress; }
	void               SetExecAddress   	( unsigned short int _execAddress )         { execAddress = _execAddress; }

private:
	std::string                	fileName;
	std::vector<unsigned char> 	data;
	bool                  		bProtected = false;
	unsigned char         		fileType = COCODS_FILETYPE_BASIC_PROGRAM;
	unsigned char         		fileFormat = COCODS_FILEFORMAT_BINARY;
	unsigned short int    		loadAddress = COCODS_INVALID;
	unsigned short int    		execAddress = COCODS_INVALID;
};

// CoCo Disk file system
class CCoCoDS_FS : public IFileSystemInterface
{
public:
	CCoCoDS_FS();
	virtual ~CCoCoDS_FS();

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

	IFileSystemInterface* NewFileSystem() { return new CCoCoDS_FS; }
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	const CCoCoDS_File& 	GetFile         ( unsigned short int fileIdx )		const { if(fileIdx < files.size()) return files[fileIdx]; return emptyFile; }
	unsigned short int  	GetFileIdx      ( const std::string& _fileName ) 	const;

private:
	IDiskImageInterface*           		disk;
	unsigned char                  		fat[COCODS_DISK_MAX_GRANULES];
	std::vector<SCoCoDSDirectoryEntry>  directory;
	std::vector<CCoCoDS_File>           files;

	CCoCoDS_File                   emptyFile;

	CDirectoryEntryWrapper         rootDir;

	bool                	SetDisk         ( IDiskImageInterface* _disk );
	IDiskImageInterface*	GetDisk         ()                              { return disk; }
	unsigned short int  	GetNumberOfFiles()                              { return (unsigned short int)files.size(); }
	bool                	ParseDirectory	();
	bool                	ParseFiles		();
	unsigned short int  	GetFileEntry	( std::string _fileName ) const;
	void                	ReadGranule		( unsigned char _granule, std::vector<unsigned char>& _dst );

	const std::vector<SCoCoDSDirectoryEntry>& GetDirectory() { return directory; }
};

#endif