#include "FileSystemFactory.h"

void FileSystemFactory::RegisterFileSystem( IFileSystemInterface* _fileSystemInterface )
{
	m_FileSystems.push_back(_fileSystemInterface);
}

IFileSystemInterface* FileSystemFactory::LoadFileSystem( IDiskImageInterface* _disk )
{
	for( auto FileSystem: m_FileSystems )
	{
		IFileSystemInterface* retVal = FileSystem->NewFileSystem();
		if( retVal->Load( _disk ) )
		{
			return retVal;
		}
		delete retVal;
	}

	return nullptr;
}

IFileSystemInterface* FileSystemFactory::GetFileSystem( size_t _index )
{
	return (_index >= m_FileSystems.size()) ? nullptr : m_FileSystems[_index];
}