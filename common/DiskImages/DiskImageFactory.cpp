#include "DiskImageFactory.h"

void DiskImageFactory::RegisterDiskImageFormat( IDiskImageInterface* _diskImageInterface )
{
	m_DiskImages.push_back(_diskImageInterface);
}

IDiskImageInterface* DiskImageFactory::LoadDiskImage( const std::string& _filename )
{
	for( auto diskImage: m_DiskImages )
	{
		IDiskImageInterface* retVal = diskImage->NewImage();
		if( retVal->Load( _filename ) )
		{
			return retVal;
		}
		delete retVal;
	}

	return nullptr;
}

IDiskImageInterface* DiskImageFactory::GetDiskImage( size_t _index )
{
	return (_index >= m_DiskImages.size()) ? nullptr : m_DiskImages[_index];
}