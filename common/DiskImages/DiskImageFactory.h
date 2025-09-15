#ifndef __DISK_IMAGE_FACTORY__
#define __DISK_IMAGE_FACTORY__

#include <string>
#include <vector>

#include "DiskImageInterface.h"

class DiskImageFactory
{
public:
	DiskImageFactory() = default;
	~DiskImageFactory();

	void RegisterDiskImageFormat( IDiskImageInterface* _diskImageInterface );

	IDiskImageInterface* LoadDiskImage( const std::string& _filename );
	IDiskImageInterface* GetDiskImage ( size_t _index );

	size_t Size() { return m_DiskImages.size(); }

private:
	std::vector<IDiskImageInterface*> m_DiskImages;
};

#endif
