/////////////////////////////////////////////////////////////
//
// FIDO, the Floppy Image DOctor.
// A graphical utility to edit floppy disk image files.
//
// FIDO_FSWrapper.cpp - Header file for the FSWrapper
//                      interface class.
//
//                      It allows read and write operations
//                      on disk filesystems.
//
// Created 14/02/2017 by Roberto Carlos Fernández Gerhardt.
//
// Last update on 14/02/2017.
//
// Notes:
//
/////////////////////////////////////////////////////////////

#ifndef __FILESYSTEM_INTERFACE__
#define __FILESYSTEM_INTERFACE__

#include <string>
#include <vector>
#include "DiskImageInterface.h"

using namespace std;

#define FA_DIRECTORY (1 << 0)
#define FA_PROTECTED (1 << 1)

struct SFileInfo
{
	string			name;
	size_t			size;
	unsigned int	attr;
	bool			isOk;

	vector<size_t>	kids;
};

class CDirectoryEntryWrapper
{
public:
    CDirectoryEntryWrapper() { isDirectory = false; }
    virtual	~CDirectoryEntryWrapper() {}

    string GetName() const { return name; }
    void   SetName( const string& _name ) { name = _name; }

    bool IsDirectory() const { return isDirectory; }
    void SetIsDirectory( bool _isDirectory ) { isDirectory = _isDirectory; }

    const vector<CDirectoryEntryWrapper*>& GetChildren() const { return children; }

    void AddChild( CDirectoryEntryWrapper* _child ) { children.push_back(_child); }

    void Clear() { isDirectory = false; children.clear(); }

private:
    string name;
    bool isDirectory;

    vector<CDirectoryEntryWrapper*> children;
};

class IFilesystemInterface
{
public:
//	IFilesystemInterface() {};
	virtual	~IFilesystemInterface() {};

	virtual bool Load(IDiskImageInterface* _disk) = 0;
	virtual bool Save(const string& _filename) = 0;

	virtual size_t GetFilesNum() const = 0;
	virtual string GetFileName(size_t _fileIdx) const = 0;
    virtual size_t GetFileSize( size_t _fileIdx ) const = 0;
    virtual size_t GetFreeSize() const = 0;

	virtual SFileInfo GetFileInfo(size_t _fileIdx) = 0;

	virtual string GetFSName() const = 0;
	virtual string GetFSVariant() const = 0;

	virtual string GetVolumeLabel() const = 0;

    virtual const CDirectoryEntryWrapper& GetFSRoot() const = 0;

    virtual bool ExtractFile( string _fileName, vector<unsigned char>& dst ) const { return false; }
    virtual bool InsertFile ( string _fileName, const vector<unsigned char>& src ) { return false; }

    virtual bool NeedManualSetup() { return false; }

    virtual bool InitDisk( IDiskImageInterface* _disk ) = 0;
};

#endif