#include <string>
#include <vector>

#include "DiskImageInterface.h"
#include "FileSystemInterface.h"

class FileSystemFactory
{
public:
	FileSystemFactory() = default;
	~FileSystemFactory() = default;

	void RegisterFileSystem( IFileSystemInterface* _fileSystemInterface );

	IFileSystemInterface* LoadFileSystem( IDiskImageInterface* _disk );
	IFileSystemInterface* GetFileSystem ( size_t _index );

	size_t Size() { return m_FileSystems.size(); }

private:
	std::vector<IFileSystemInterface*> m_FileSystems;
};