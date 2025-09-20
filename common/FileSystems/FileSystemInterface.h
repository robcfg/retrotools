/////////////////////////////////////////////////////////////
//
// IFileSystemInterface.cpp - Header file for the 
//                            IFileSystemInterface interface 
//                            class.
//
//                            It allows read and write
//                            operations on disk fileSystems.
//
// Created 14/02/2017 by Roberto Carlos Fernández Gerhardt.
//
// Notes:
//
/////////////////////////////////////////////////////////////

#ifndef __FileSystem_INTERFACE__
#define __FileSystem_INTERFACE__

#include <string>
#include <vector>
#include "../DiskImages/DiskImageInterface.h"

#define FA_DIRECTORY (1 << 0)
#define FA_PROTECTED (1 << 1)

struct SFileInfo
{
	std::string	        name;
	size_t			    size;
	unsigned int	    attr;
	bool			    isOk;

	std::vector<size_t>	kids;
};

class CDirectoryEntryWrapper
{
public:
	CDirectoryEntryWrapper() { isDirectory = false; }
	virtual	~CDirectoryEntryWrapper() {}

	std::string GetName() const { return name; }
	void        SetName( const std::string& _name ) { name = _name; }

	bool IsDirectory() const { return isDirectory; }
	void SetIsDirectory( bool _isDirectory ) { isDirectory = _isDirectory; }

	const std::vector<CDirectoryEntryWrapper*>& GetChildren() const { return children; }

	void AddChild( CDirectoryEntryWrapper* _child ) { children.push_back(_child); }

	void Clear() { isDirectory = false; children.clear(); }

private:
	std::string name;
	bool isDirectory;

	std::vector<CDirectoryEntryWrapper*> children;
};

class IFileSystemInterface
{
public:
//	IFileSystemInterface() {};
	virtual	~IFileSystemInterface() {};

	// IFileSystemInterface functions to be implemented by derived classes. //////////////////////////////////
	virtual bool Load(IDiskImageInterface* _disk) = 0;
	virtual bool Save(const std::string& _filename) = 0;

	virtual size_t      GetFilesNum() const = 0;
	virtual std::string GetFileName(size_t _fileIdx) const = 0;
	virtual size_t      GetFileSize( size_t _fileIdx ) const = 0;
	virtual size_t      GetFreeSize() const = 0;

	virtual SFileInfo   GetFileInfo(size_t _fileIdx) const = 0;

	virtual std::string GetFSName() const = 0;
	virtual std::string GetFSVariant() const = 0;

	virtual std::string GetVolumeLabel() const = 0;

	virtual const CDirectoryEntryWrapper& GetFSRoot() const = 0;

	virtual bool ExtractFile( const std::string& _fileName, std::vector<unsigned char>& dst, bool _withBinaryHeader ) const = 0;
	virtual bool InsertFile ( const std::string& _fileName, const std::vector<unsigned char>& src, bool _binaryFile ) = 0;
	virtual bool DeleteFile ( const std::string& _fileName ) = 0;

	virtual bool NeedManualSetup() { return false; }

	virtual bool InitDisk( IDiskImageInterface* _disk ) = 0;

	virtual IFileSystemInterface* NewFileSystem() = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Copy and uncomment the block below into your derived class' header file.

	// IFileSystemInterface //////////////////////////////////////////////////////////////////////////////////
	// bool Load(IDiskImageInterface* _disk);
	// bool Save(const std::string& _filename);

	// size_t      GetFilesNum() const;
	// std::string GetFileName(size_t _fileIdx) const;
	// size_t      GetFileSize( size_t _fileIdx ) const;
	// size_t      GetFreeSize() const;

	// SFileInfo   GetFileInfo(size_t _fileIdx) const;

	// std::string GetFSName() const;
	// std::string GetFSVariant() const;

	// std::string GetVolumeLabel() const;

	// const CDirectoryEntryWrapper& GetFSRoot() const;

	// bool ExtractFile( const std::string& _fileName, std::vector<unsigned char>& dst, bool _withBinaryHeader ) const;
	// bool InsertFile ( const std::string& _fileName, const std::vector<unsigned char>& src, bool _binaryFile );
	// bool DeleteFile ( const std::string& _fileName );

	// bool NeedManualSetup() { return false; }

	// bool InitDisk( IDiskImageInterface* _disk );

	// IFileSystemInterface* NewFileSystem();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
};

#endif