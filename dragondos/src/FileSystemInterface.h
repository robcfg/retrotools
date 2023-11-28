/////////////////////////////////////////////////////////////
//
// IFilesystemInterface.cpp - Header file for the 
//                            IFilesystemInterface interface 
//                            class.
//
//                              It allows read and write
//                              operations on disk filesystems.
//
// Created 14/02/2017 by Roberto Carlos Fernández Gerhardt.
//
// Last update on 28/11/2023.
//
// Notes:
//
/////////////////////////////////////////////////////////////

#ifndef __FILESYSTEM_INTERFACE__
#define __FILESYSTEM_INTERFACE__

#include <string>
#include <vector>
#include "DiskImageInterface.h"

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

class IFilesystemInterface
{
public:
//	IFilesystemInterface() {};
	virtual	~IFilesystemInterface() {};

	virtual bool Load(IDiskImageInterface* _disk) = 0;
	virtual bool Save(const std::string& _filename) = 0;

	virtual size_t      GetFilesNum() const = 0;
	virtual std::string GetFileName(size_t _fileIdx) const = 0;
    virtual size_t      GetFileSize( size_t _fileIdx ) const = 0;
    virtual size_t      GetFreeSize() const = 0;

	virtual SFileInfo   GetFileInfo(size_t _fileIdx) = 0;

	virtual std::string GetFSName() const = 0;
	virtual std::string GetFSVariant() const = 0;

	virtual std::string GetVolumeLabel() const = 0;

    virtual const CDirectoryEntryWrapper& GetFSRoot() const = 0;

    virtual bool ExtractFile( std::string _fileName, std::vector<unsigned char>& dst ) const { return false; }
    virtual bool InsertFile ( std::string _fileName, const std::vector<unsigned char>& src ) { return false; }
    virtual bool DeleteFile ( std::string _fileName ) { return false; }

    virtual bool NeedManualSetup() { return false; }

    virtual bool InitDisk( IDiskImageInterface* _disk ) = 0;
};

#endif