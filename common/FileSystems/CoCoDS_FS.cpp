#include "CoCoDS_FS.h"
#include <cstring>

static const unsigned int granuleTracks[COCODS_DISK_MAX_GRANULES] = {
	0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,
	18,18,19,19,20,20,21,21,22,22,23,23,24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31,
	32,32,33,33,34,34
};

static const unsigned int granuleSectors[COCODS_DISK_MAX_GRANULES] = {
	0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,
	0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9
};


// Get file data
void CCoCoDS_File::GetFileData( std::vector<unsigned char>& dst ) const
{
	dst.clear();
	dst.insert( dst.begin(), data.begin(), data.end() );
}

// Set file data
void CCoCoDS_File::SetFileData( const std::vector<unsigned char>& src )
{
	data.clear();
	data.insert( data.begin(), src.begin(), src.end() );
}

// Set file data
void CCoCoDS_File::SetFileData( const unsigned char* src, size_t size )
{
	data.clear();
	data.insert( data.begin(), src, src + size );
}

CCoCoDS_FS::CCoCoDS_FS()
{

}

CCoCoDS_FS::~CCoCoDS_FS()
{

}

bool CCoCoDS_FS::SetDisk( IDiskImageInterface* _disk )
{
	disk = _disk;

	if( false == ParseDirectory() )
	{
		disk = nullptr;
		return false;
	}

	if( false == ParseFiles() )
	{
		disk = nullptr;
		return false;
	}

	return true;
}

unsigned short int GetFileIdx( const std::string& _fileName )
{
	return 0;
}

bool CCoCoDS_FS::InsertFile( const std::string& _fileName, const std::vector<unsigned char>& src, bool _binaryFile )
{
	return false;
}

bool CCoCoDS_FS::DeleteFile( const std::string& _fileName )
{
	return false;
}

bool CCoCoDS_FS::ParseDirectory()
{
	if( !disk )
	{
		return false;
	}

	// Read FAT
	const unsigned char* sector = disk->GetSector( COCODS_DISK_DIRECTORY_TRACK, 0, COCODS_DIR_FAT_SECTOR );
	if( !sector )
	{
		return false;
	}
	memcpy( fat, sector, COCODS_DISK_MAX_GRANULES );

	// Read directory
	for( unsigned int dirSector = COCODS_DIR_START_SECTOR; dirSector < COCODS_DIR_START_SECTOR + COCODS_DIR_SECTORS_NUM; ++dirSector )
	{
		sector = disk->GetSector( COCODS_DISK_DIRECTORY_TRACK, 0, dirSector );

		for( unsigned int entryIdx = 0; entryIdx < COCODS_DIR_ENTRIES_PER_SECTOR; ++entryIdx )
		{
			const unsigned char* entry = sector + (entryIdx * COCODS_DIR_ENTRY_SIZE);
			if( entry[0] == COCODS_DIR_ENTRIES_END )
			{
				return true;
			}
			else if( entry[0] != COCODS_DIR_ENTRY_UNUSED )
			{
				SCoCoDSDirectoryEntry tmpEntry;
				unsigned char offset = 0;
				char tmpChar = ' ';

				tmpEntry.name.insert( 0, (const char*)&entry[offset], COCODS_DIR_NAME_SIZE );
				offset += COCODS_DIR_NAME_SIZE;

				// Add extension characters, substituting any outside the normal 32-127 range for the space character.
				for( size_t extCharIdx = 0; extCharIdx < COCODS_DIR_EXT_SIZE; ++extCharIdx )
				{
					tmpChar = entry[offset+extCharIdx];
					tmpEntry.extension += ((tmpChar >= 32 && tmpChar <= 127) ? tmpChar : ' ');
				}
				offset += COCODS_DIR_EXT_SIZE;

				tmpEntry.type = entry[offset++];
				tmpEntry.format = entry[offset++];
				tmpEntry.firstGranule = entry[offset++];

				tmpEntry.bytesInLastSector = entry[offset++] << 8;
				tmpEntry.bytesInLastSector |= entry[offset];

				directory.push_back( tmpEntry );
			}
		}
	}

	return true;
}

bool CCoCoDS_FS::ParseFiles()
{
	for( const auto& dirEntry : directory )
	{
		std::string tmpStr;
		std::vector<unsigned char> tmpData;
		CCoCoDS_File tmpFile;
		
		tmpStr = dirEntry.name;
		tmpStr += ".";
		tmpStr += dirEntry.extension;
		tmpFile.SetFileName( tmpStr );
		tmpFile.SetFileType( dirEntry.type );

		unsigned char curGranule = dirEntry.firstGranule;
		while( curGranule < COCODS_DISK_MAX_GRANULES )
		{
			ReadGranule( curGranule, tmpData );
			curGranule = fat[curGranule];
		}

		// Read last granule and trim excess data
		ReadGranule( curGranule, tmpData );
		size_t lastSectorSize = (COCODS_DISK_SECTOR_SIZE - dirEntry.bytesInLastSector);
		if( tmpData.size() >= lastSectorSize )
		{
			tmpData.resize( tmpData.size() - lastSectorSize );
		}

		//unsigned short int size = (tmpData[1] * 256) + tmpData[2];
		if( tmpData.size() >= 7 )
		{
			tmpFile.SetLoadAddress( (tmpData[3] * 256) + tmpData[4]);
			tmpFile.SetExecAddress( (tmpData[tmpData.size()-2] * 256) + tmpData[tmpData.size()-1]);
		}
		tmpFile.SetFileData( tmpData );
		files.push_back( tmpFile );
	}

	return true;
}

unsigned short int CCoCoDS_FS::GetFileEntry( std::string _fileName )
{
	return 0;
}

bool CCoCoDS_FS::ExtractFile( const std::string& _fileName, std::vector<unsigned char>& _dst, bool _withBinaryHeader ) const
{
	return false;
}

void CCoCoDS_FS::ReadGranule( unsigned char _granule, std::vector<unsigned char>& _dst )
{
	unsigned char numSectors = COCODS_DISK_GRANULE_SECTORS_NUM;

	if( _granule >= 0xC0 && _granule <= 0xC9 )
	{
		numSectors = _granule & 0x3F;
		size_t newSize = ((COCODS_DISK_GRANULE_SECTORS_NUM - numSectors) * COCODS_DISK_SECTOR_SIZE);
		
		if( _dst.size() >= newSize )
		{
			_dst.resize( _dst.size() - newSize );
		}
	}
	else
	{
		for( unsigned char sectorIdx = 0; sectorIdx < numSectors; ++sectorIdx )
		{
			const unsigned char* sector = disk->GetSector( granuleTracks[_granule], 0, granuleSectors[_granule] + sectorIdx );
			if( sector )
			{
				for( auto byteIdx = 0; byteIdx < COCODS_DISK_SECTOR_SIZE; ++byteIdx )
				{
					_dst.push_back( sector[byteIdx] );
				}
			}
		}
	}
}

bool CCoCoDS_FS::Load(IDiskImageInterface* _disk)
{
	bool retVal = SetDisk( _disk );
	
	rootDir.SetIsDirectory(true);
	rootDir.SetName( GetFSName() );

	for( size_t fileIdx = 0; fileIdx < files.size(); ++fileIdx )
	{
		const CCoCoDS_File& file = GetFile( static_cast<unsigned short>(fileIdx) );

		CDirectoryEntryWrapper* entry = new CDirectoryEntryWrapper;

		entry->SetIsDirectory( false );
		entry->SetName( file.GetFileName() );
		rootDir.AddChild( entry );
	}

	return retVal;
}

bool CCoCoDS_FS::Save(const std::string& _filename)
{
	return false;
}

size_t CCoCoDS_FS::GetFilesNum() const
{
	return files.size();
}

std::string CCoCoDS_FS::GetFileName(size_t _fileIdx) const
{
	const CCoCoDS_File& file = GetFile( static_cast<unsigned short>(_fileIdx) );
	
	return file.GetFileName();
}

size_t CCoCoDS_FS::GetFileSize( size_t _fileIdx ) const
{
	if( _fileIdx > files.size() ) return 0;

	const CCoCoDS_File& file = GetFile( static_cast<unsigned short>(_fileIdx) );
	return file.GetFileSize();
}

SFileInfo CCoCoDS_FS::GetFileInfo(size_t _fileIdx) const
{
	SFileInfo retVal;

	const CCoCoDS_File& file = GetFile( static_cast<unsigned short>(_fileIdx) );

	retVal.isOk = true;
	retVal.name = file.GetFileName();
	retVal.size = file.GetFileSize();

	return retVal;
}

std::string CCoCoDS_FS::GetFSName() const
{
	return "CoCo Disk System";
}

std::string CCoCoDS_FS::GetFSVariant() const
{
	return "";
}

std::string CCoCoDS_FS::GetVolumeLabel() const
{
	return "CoCo Disk System";
}

const CDirectoryEntryWrapper& CCoCoDS_FS::GetFSRoot() const
{
	return rootDir;
}

bool CCoCoDS_FS::InitDisk( IDiskImageInterface* _disk )
{
	return false;
}

size_t CCoCoDS_FS::GetFreeSize() const
{
	return 0;
}

std::string CCoCoDS_File::GetFileTypeString() const
{
	std::string retVal;

	switch( fileType )
	{
	case COCODS_FILETYPE_BASIC_DATA  		:retVal = "DAT"; break;
	case COCODS_FILETYPE_BASIC_PROGRAM 		:retVal = "BAS"; break;
	case COCODS_FILETYPE_MACHINE_LANGUAGE	:retVal = "BIN"; break;
	case COCODS_FILETYPE_TEXT_EDITOR_SOURCE	:retVal = "SRC"; break;
	default:retVal = "???";
	}

	return retVal;
}

std::string CCoCoDS_File::GetFileFormatString() const
{
	std::string retVal;

	switch( fileFormat )
	{
	case COCODS_FILEFORMAT_BINARY	:retVal = "BIN"; break;
	case COCODS_FILEFORMAT_ASCII 	:retVal = "ASC"; break;
	default:retVal = "???";
	}

	return retVal;
}
