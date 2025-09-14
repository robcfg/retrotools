//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DragonDOS_FS.cpp - Implementation for CDragonDOS_FS, a helper 
//                    class that allows file operations on a disk
//                    image formatted with the DragonDOS file system
//
// For info on the DragonDOS file system go to:
//              http://dragon32.info/info/drgndos.txt
//              http://dragon32.info/info/binformt.txt
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Last update: 12/11/2023
//
// File entries in directory table are 25 byte long.
// Example:
//  00504D4F 44453200 0042494E 01440D00 00000000 00000000 09
//  | | | |  | | | |  | | | |  | | | |  | | | |  | | | |  |
//  | | | |  | | | |  | | | |  | | | |  | | | |  | | | |  +-> Bytes on last sector
//  | +-+-+--+-+-+-+--+-+-+-+--+-+-+-+--+-+-+-+--+-+-+-+----> File header block or continuation block
//  +-------------------------------------------------------> Flags
//
//  File header block
//  --504D4F 44453200 0042494E 01440D00 00000000 00000000 --
//    | | |  | | | |  | | | |  | | | |  | | | |  | | | | 
//    | | |  | | | |  | | | |  | | | |  | | | |  | +-+-+----> Sector Allocation Block 4
//    | | |  | | | |  | | | |  | | | |  | | +-+--+----------> Sector Allocation Block 3
//    | | |  | | | |  | | | |  | | | +--+-+-----------------> Sector Allocation Block 2
//    | | |  | | | |  | | | |  +-+-+------------------------> Sector Allocation Block 1
//    | | |  | | | |  | +-+-+-------------------------------> extension, padded with 0x00 ("BIN")
//    +-+-+--+-+-+-+--+-------------------------------------> filename, padded with 0x00  ("PMODE2")
//
//  Continuation block
//  --112233 44556677 8899AABB CCDDEEFF GGHHIIJJ KKLLMMNN --
//    | | |  | | | |  | | | |  | | | |  | | | |  | | | | 
//    | | |  | | | |  | | | |  | | | |  | | | |  | | +-+----> Unused
//    | | |  | | | |  | | | |  | | | |  | | | +--+-+--------> Sector Allocation Block 7
//    | | |  | | | |  | | | |  | | | |  +-+-+---------------> Sector Allocation Block 6
//    | | |  | | | |  | | | |  | +-+-+----------------------> Sector Allocation Block 5
//    | | |  | | | |  | | +-+--+----------------------------> Sector Allocation Block 4
//    | | |  | | | +--+-+-----------------------------------> Sector Allocation Block 3
//    | | |  +-+-+------------------------------------------> Sector Allocation Block 2
//    +-+-+-------------------------------------------------> Sector Allocation Block 1
//
//  Sector Allocation Block format:
//  -------- -------- -------- 01440D-- -------- -------- --
//                             | | |
//                             | | +-------------------------> Count of contiguous sectors in this block
//                             +-+---------------------------> Logical Sector Number of first sector in this block
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <filesystem>
#include <string.h> // for strcasecmp
#include <sstream>
#include "DragonDOS_FS.h"
#include "../FS_Utils.h"

#ifndef _WIN32
#define _stricmp strcasecmp
#endif

uint8_t count_ones (uint8_t byte)
{
	static const uint8_t NIBBLE_LOOKUP [16] =
	{
		0, 1, 1, 2, 1, 2, 2, 3, 
		1, 2, 2, 3, 2, 3, 3, 4
	};

	return NIBBLE_LOOKUP[byte & 0x0F] + NIBBLE_LOOKUP[byte >> 4];
}

// Get file data
void CDGNDosFile::GetFileData( std::vector<unsigned char>& dst ) const
{
	dst.clear();
	dst.insert( dst.begin(), data.begin(), data.end() );
}

// Set file data
void CDGNDosFile::SetFileData( const std::vector<unsigned char>& src )
{
	data.clear();
	data.insert( data.begin(), src.begin(), src.end() );
}

// Set file data
void CDGNDosFile::SetFileData( const unsigned char* src, size_t size )
{
	data.clear();
	data.insert( data.begin(), src, src + size );
}

// Constructor
CDragonDOS_FS::CDragonDOS_FS()
{
	disk = NULL;
}

// Destructor
CDragonDOS_FS::~CDragonDOS_FS()
{

}

// Sets the disk to work with and analyzes it
// to check if it's indeed a DragonDOS disk.
//
// Returns false if the disk is not
// DragonDOS formatted.
bool CDragonDOS_FS::SetDisk( IDiskImageInterface* _disk )
{
	disk = _disk;

	if( DRAGONDOS_SECTORSPERTRACK != disk->GetSectorsNum() )
	{
		disk = NULL;
		return false;
	}

	if( false == ParseDirectory() )
	{
		disk = NULL;
		return false;
	}

	if( false == ParseFiles() )
	{
		disk = NULL;
		return false;
	}

	return true;
}

// Returns a file's index based on its name
unsigned short int CDragonDOS_FS::GetFileIdx( std::string _fileName ) const
{
	unsigned short int retVal = DRAGONDOS_INVALID;

	unsigned short int fileIdx = 0;
	while( retVal == DRAGONDOS_INVALID && fileIdx < files.size() )
	{
		if( 0 == _stricmp(_fileName.c_str(),files[fileIdx].GetFileName().c_str()) )
		{
			retVal = fileIdx;
		}

		++fileIdx;
	}

	return retVal;
}

// Returns a file's first directory entry based on its name
unsigned short int CDragonDOS_FS::GetFileEntry( std::string _fileName ) const
{
	unsigned short int retVal = DRAGONDOS_INVALID;

	unsigned short int fileIdx = 0;
	while( retVal == DRAGONDOS_INVALID && fileIdx < directory.size() )
	{
		if( 0 == _stricmp(_fileName.c_str(),directory[fileIdx].fileBlock.fileName.c_str()) )
		{
			retVal = fileIdx;
		}

		++fileIdx;
	}

	return retVal;
}

// Extracts file from the DragonDOS file system to a specified location
bool CDragonDOS_FS::ExtractFile( const std::string& _fileName, std::vector<unsigned char>& _dst, bool _withBinaryHeader ) const
{
	unsigned short int fileIdx = GetFileEntry( _fileName );

	if( fileIdx == DRAGONDOS_INVALID )
	{
		return false;
	}

	SDGNDosDirectoryEntry entry = directory[fileIdx];

	// Skip file types BIN and BAS' header.
	bool skipHeader = false;
	switch( entry.fileType )
	{
		case DRAGONDOS_FILETYPE_BASIC :skipHeader = true; break;
		case DRAGONDOS_FILETYPE_BINARY:skipHeader = !_withBinaryHeader; break;
		default:skipHeader = false; break;
	}

	bool finished = false;
	while( !finished )
	{
		size_t fabNum = entry.fileBlock.FABs.size();
		for( size_t fab = 0; fab < fabNum; ++fab )
		{
			unsigned short int LSN = entry.fileBlock.FABs[fab].LSN;
			unsigned short int sectorsNum = entry.fileBlock.FABs[fab].numSectors;

			for( unsigned short int sector = 0; sector < sectorsNum; ++sector )
			{
				unsigned short int tmpLSN = LSN+sector;

				const unsigned char* data = disk->GetSector(LSNTrack(*disk,tmpLSN),LSNHead(*disk,tmpLSN),LSNSector(*disk,tmpLSN));

				if( skipHeader )
				{
					if( sectorsNum == 1 )
					{
						_dst.insert( _dst.end(), data + DRAGONDOS_FILEHEADER_SIZE, data + entry.lastSectorSize );
					}
					else
					{
						_dst.insert( _dst.end(), data + DRAGONDOS_FILEHEADER_SIZE, data + DRAGONDOS_SECTOR_SIZE );
					}
					skipHeader = false; // Only need to skip header once.
				}
				else if( !entry.bContinued && fab == fabNum - 1 && sector == sectorsNum - 1) // extract the right number of bytes from last sector
				{
					_dst.insert( _dst.end(), data, data + entry.lastSectorSize );
				}
				else
				{
					_dst.insert( _dst.end(), data, data + DRAGONDOS_SECTOR_SIZE );
				}
			}      
		}

		if( entry.bContinued )
		{
			entry = directory[entry.nextBlock];
		}
		else
		{
			finished = true;
		}
	}

	return true;
}

// Inserts a file into the DragonDOS file system
bool CDragonDOS_FS::InsertFile( const std::string& _fileName, const std::vector<unsigned char>& _data, bool _binaryFile )
{
	// TODO: Maybe should add the type to the parameters and add the header data here...
	if( nullptr == disk || _fileName.empty() || _data.size() > DRAGONDOS_MAX_FILE_SIZE )
	{
		return false;
	}

	size_t freeSize = GetFreeSize();

	// Check file size
	if( _data.size() > freeSize )
	{
		return false;
	}

	// Look for a deleted or unused directory entry
	unsigned char* entryPtr = nullptr;

	for( unsigned int sectorIdx = DRAGONDOS_DIR_START_SECTOR; sectorIdx < DRAGONDOS_SECTORSPERTRACK; ++sectorIdx )
	{
		unsigned char* sector = disk->GetSector( DRAGONDOS_DIR_TRACK, 0, sectorIdx );
		if( nullptr == sector )
		{
			return false;
		}

		for( unsigned int entry = 0; entry < DRAGONDOS_DIR_ENTRIES_PER_SECT; ++entry )
		{
			entryPtr = sector + (entry * DRAGONDOS_DIR_ENTRY_SIZE);

			if( 0 != (*entryPtr & DRAGONDOS_FLAG_DELETED) ) // Usable Entry found
			{
				entry = DRAGONDOS_DIR_ENTRIES_PER_SECT;
				sectorIdx = DRAGONDOS_SECTORSPERTRACK;
			}
		}
	}

	*entryPtr = 0; // Clear all flags

	// Set name and extension
	std::string fileNameUpper = _fileName;
	transform( fileNameUpper.begin(), fileNameUpper.end(), fileNameUpper.begin(), ::toupper );
	std::filesystem::path filePath( fileNameUpper );

	std::string name = filePath.stem().string();
	std::string extension = filePath.extension().string();
	if( name.length() > DRAGONDOS_MAX_FILE_NAME_LEN ) // TODO:Create a sanitize name and ext functions and add padding
	{
		name = name.substr( 0, DRAGONDOS_MAX_FILE_NAME_LEN );
	}
	if( extension[0] == '.' )
	{
		extension = extension.substr(1);
	}
	if( extension.length() > DRAGONDOS_MAX_FILE_EXT_LEN )
	{
		extension = extension.substr( 0, DRAGONDOS_MAX_FILE_EXT_LEN );
	}

	memset( entryPtr + 1, 0                , DRAGONDOS_MAX_FILE_NAME_LEN );
	memcpy( entryPtr + 1, name.c_str()     , name.length()               );
	memset( entryPtr + 9, 0                , DRAGONDOS_MAX_FILE_EXT_LEN  );
	memcpy( entryPtr + 9, extension.c_str(), extension.length()          );

	// Allocate sectors;
	size_t maxSectorNumPerSide = disk->GetTracksNum() * DRAGONDOS_SECTORSPERTRACK;
	unsigned short int sectorsNeeded = (unsigned short int)(_data.size()/DRAGONDOS_SECTOR_SIZE);
	sectorsNeeded += (_data.size()%DRAGONDOS_SECTOR_SIZE) ? 1 : 0;
	size_t bitmapSectorNum = (disk->GetTracksNum() > 40 && disk->GetSidesNum() > 1) ? 2 : 1;
	std::vector<size_t> freeLSNs;

	size_t bitmapBytesNeeded = (sectorsNeeded / 8) + ((sectorsNeeded % 8) != 0 ? 1 : 0);
	bool bFound = false;
	size_t firstBitmapByte = 0;
	size_t bitmapSector = 0;
	unsigned char* bitmapSectorPtr = nullptr;
	for( size_t bmpSec = 0; ((bmpSec < bitmapSectorNum) && !bFound); bmpSec++ )
	{
		bitmapSectorPtr = disk->GetSector( DRAGONDOS_DIR_TRACK, 0, (unsigned int)bmpSec );

		for( size_t bitmapByte = 0; ((bitmapByte < DRAGONDOS_BITMAPSIZE - bitmapBytesNeeded) && !bFound); ++bitmapByte )
		{
			bFound = true;
			for( size_t curByte = 0; curByte < bitmapBytesNeeded; ++curByte )
			{
				if( bitmapSectorPtr[bitmapByte + curByte] != 0xFF )
				{
					bFound = false;
				}
			}

			if( bFound )
			{
				firstBitmapByte = bitmapByte;
				bitmapSector = bmpSec;
				break;
			}
		}
	}

	if( !bFound )
	{
		return false;
	}

	size_t startLSN = ((bitmapSector * DRAGONDOS_BITMAPSIZE) + firstBitmapByte) * 8;
	size_t endLSN = startLSN + sectorsNeeded;
	unsigned short int fabLSN = (unsigned short int)startLSN;
	const unsigned char* data = _data.data();
	size_t dataSize = _data.size();

	unsigned char* dstSector = nullptr;
	for( ; startLSN < endLSN; ++startLSN )
	{
		dstSector = disk->GetSector( LSNTrack(*disk,(unsigned short int)startLSN),  LSNHead(*disk,(unsigned short int)startLSN), LSNSector(*disk,(unsigned short int)startLSN) );
		MarkBitmapLSNUsed( disk, startLSN );

		memcpy( dstSector, data, std::min( (size_t)DRAGONDOS_SECTOR_SIZE, dataSize) );
		data += DRAGONDOS_SECTOR_SIZE;

		if( dataSize >= DRAGONDOS_SECTOR_SIZE )
		{
			dataSize -= DRAGONDOS_SECTOR_SIZE;
		}
		else // needed?
		{
			break;
		}
	}

	// Set flags and sector data
	entryPtr[0x00] = 0;
	entryPtr[0x0C] = (fabLSN >> 8);
	entryPtr[0x0D] = (fabLSN & 0xFF);
	entryPtr[0x0E] = (sectorsNeeded & 0xFF);
	entryPtr[0x18] = (unsigned char)(_data.size() % DRAGONDOS_SECTOR_SIZE);

	BackUpDirTrack( disk );

	return true;
}

// Deletes a file from the DragonDOS file system
bool CDragonDOS_FS::DeleteFile( const std::string& _fileName )
{
	unsigned short int entry = GetFileEntry( _fileName );

	if( entry >= directory.size() )
	{
		return false;
	}

	// Go through all File Allocation Blocks (FABs) belonging
	// to this file mark them as deleted/free and mark the 
	// associated sectors as free on the bitmap.
	unsigned int entrySector = directory[entry].sector;
	unsigned int entryIndex  = directory[entry].entry;

	// On directory table, entry's flag is set to 0x81. Deleted/free + continuation.
	// On the bitmap, sectors belongimg to this files have their bits set (1).
	unsigned char* sector = disk->GetSector( DRAGONDOS_DIR_TRACK, 0, entrySector );
	if( nullptr == sector )
	{
		return false;
	}

	sector[entryIndex * DRAGONDOS_DIR_ENTRY_SIZE] = (DRAGONDOS_FLAG_DELETED | DRAGONDOS_FLAG_CONTINUATION);

	// Track 20, sector 1. Sector bitmap and disk geometry.
	sector = disk->GetSector( DRAGONDOS_DIR_TRACK, 0, 0 );
	if( nullptr == sector )
	{
		return false;
	}

	unsigned short int bitmapPos = 0;
	unsigned short int bytePos = 0;
	unsigned short int lsn = 0;
	
	for( auto fab : directory[entry].fileBlock.FABs )
	{
		// Check for LSNs 0x5a0 - 0xb3f (80 Track, DS only), which are in sector 1
		lsn = fab.LSN;
		if( lsn > DRAGONDOS_SECTORSPERBITMAPSECTOR )
		{
			lsn -= DRAGONDOS_SECTORSPERBITMAPSECTOR;
			sector = disk->GetSector( DRAGONDOS_DIR_TRACK, 0, 1 );
		}
		else
		{
			sector = disk->GetSector( DRAGONDOS_DIR_TRACK, 0, 0 );
		}

		// Mark sectors as free
		for( size_t contSector = 0; contSector < fab.numSectors; ++contSector )
		{
			bitmapPos = (unsigned short int)((lsn + contSector) / 8);
			bytePos   = (lsn + contSector) % 8;

			sector[bitmapPos] |= (1 << bytePos);
		}
	}

	BackUpDirTrack( disk );

	return true;
}

// Analyzes the disk image and decodes the DragonDOS directory information.
bool CDragonDOS_FS::ParseDirectory()
{
	if( 0 == disk ) // Must use SetDisk() first with a valid image
	{
		return false;
	}

	directory.clear();
	rootDir.Clear();
	rootDir.SetIsDirectory( true );
	rootDir.SetName( GetVolumeLabel() );

	const unsigned char* sector = disk->GetSector(DRAGONDOS_DIR_TRACK,0,0);
	if( !sector )
	{
		return false;
	}

	// Check disk geometry
	unsigned char numTracks    = sector[0xFC];  
	unsigned char secsPerTrack = sector[0xFD];

	if( sector[0xFE] != (~numTracks    & 0xFF) )
	{
		return false;
	}
	if( sector[0xFF] != (~secsPerTrack & 0xFF) )
	{
		return false;
	}
	if( secsPerTrack != DRAGONDOS_SECTORSPERTRACK * disk->GetSidesNum() )
	{
		return false;
	}

	// Directory info, Sectors 3-18 (20,0,3..18)
	bool bEndOfDir = false;
	unsigned int dirSector = DRAGONDOS_DIR_START_SECTOR;

	while( !bEndOfDir && dirSector < DRAGONDOS_SECTORSPERTRACK )
	{
		sector = disk->GetSector(DRAGONDOS_DIR_TRACK,0,dirSector);

		// 10 directory entries per sector
		unsigned int entry = 0;
		while( !bEndOfDir && entry < DRAGONDOS_DIR_ENTRIES_PER_SECT )
		{
			unsigned char entryBase = entry * DRAGONDOS_DIR_ENTRY_SIZE; // The entry size is 25 bytes
			unsigned char flag = sector[entryBase];
			bEndOfDir = (flag & DRAGONDOS_FLAG_ENDOFDIR) != 0;
			if( bEndOfDir )
			{
				continue;
			}

			SDGNDosDirectoryEntry dirEntry;
			dirEntry.sector = dirSector;
			dirEntry.entry  = entry;

			// Flags
			dirEntry.bDeleted      = (flag & DRAGONDOS_FLAG_DELETED     ) != 0;
			dirEntry.bProtected    = (flag & DRAGONDOS_FLAG_PROTECTED   ) != 0;
			dirEntry.bContinuation = (flag & DRAGONDOS_FLAG_CONTINUATION) != 0;
			dirEntry.bContinued    = (flag & DRAGONDOS_FLAG_CONTINUED   ) != 0;

			if( dirEntry.bContinued )
			{
				dirEntry.nextBlock = sector[entryBase+24];
			}
			else
			{
				dirEntry.lastSectorSize = sector[entryBase+24];
				dirEntry.lastSectorSize = (dirEntry.lastSectorSize == 0) ? 256 : dirEntry.lastSectorSize;
			}

			// File Header entry or Continuation entry
			if( !dirEntry.bContinuation ) // File Header entry
			{
				const unsigned char* blockPointer = &sector[entryBase+1];
				unsigned char uChar;

				// File Name
				for( uChar = 0; uChar < 8; ++uChar )
				{
					if( 0 != *blockPointer )
					{
						dirEntry.fileBlock.fileName += *blockPointer;
					}
					++blockPointer;
				}

				dirEntry.fileBlock.fileName += ".";

				for( uChar = 0; uChar < 3; ++uChar )
				{
					if( 0 != *blockPointer )
					{
						dirEntry.fileBlock.fileName += *blockPointer;
					}
					++blockPointer;
				}

				// Sector Allocation Blocks
				for( uChar = 0; uChar < 4; ++uChar )
				{
					unsigned short int firstByte = *blockPointer++;
					unsigned short int secondByte = *blockPointer++;
					unsigned short int LSN = (firstByte << 8) | secondByte;
					unsigned char      sectorsNum = *blockPointer++;

					if( 0 != sectorsNum )
					{
						SDGNDosFAB fab;

						fab.LSN        = LSN;
						fab.numSectors = sectorsNum;

						dirEntry.fileBlock.FABs.push_back(fab);
					}
				}

				// File size, start, end and exec addresses
				if( !dirEntry.fileBlock.FABs.empty() )
				{
					const SDGNDosFAB& fab = dirEntry.fileBlock.FABs[0];
					if( fab.numSectors != 0 ) // check for empty file
					{
						const unsigned char* fileInfoSec = disk->GetSector(LSNTrack(*disk,fab.LSN),LSNHead(*disk,fab.LSN),LSNSector(*disk,fab.LSN));
						unsigned short int firstByte  = 0;
						unsigned short int secondByte = 0;

						// Standard BAS or BIN file
						if( fileInfoSec[0x0] == 0x55 || fileInfoSec[0x08] == 0xAA )
						{
							// File Type
							dirEntry.fileType = fileInfoSec[0x01];

							// Load Address
							firstByte  = fileInfoSec[0x02];
							secondByte = fileInfoSec[0x03];
							dirEntry.loadAddress = (firstByte << 8) | secondByte;

							// File Length
							firstByte  = fileInfoSec[0x04];
							secondByte = fileInfoSec[0x05];
							dirEntry.fileSize = (firstByte << 8) | secondByte;

							// Exec Address
							firstByte  = fileInfoSec[0x06];
							secondByte = fileInfoSec[0x07];
							dirEntry.execAddress = (firstByte << 8) | secondByte;
						}
						else
						{
							if( !dirEntry.bContinued )
							{
								dirEntry.fileSize = dirEntry.lastSectorSize;
							}
						}
					}
				}

				directory.push_back(dirEntry);

				// DirectoryWrapper version
				CDirectoryEntryWrapper* newEntry = new CDirectoryEntryWrapper;
				newEntry->SetName( dirEntry.fileBlock.fileName );
				rootDir.AddChild( newEntry );
			}
			else // Continuation entry
			{
				const unsigned char* blockPointer = &sector[entryBase+1];

				// Sector Allocation Blocks
				for( unsigned char uChar = 0; uChar < 7; ++uChar )
				{
					unsigned short int firstByte = *blockPointer++;
					unsigned short int secondByte = *blockPointer++;
					unsigned short int LSN = (firstByte << 8) | secondByte;
					unsigned char      sectorsNum = *blockPointer++;

					if( 0 != sectorsNum )
					{
						SDGNDosFAB fab;

						fab.LSN        = LSN;
						fab.numSectors = sectorsNum;

						dirEntry.fileBlock.FABs.push_back(fab);
					}
				}

				directory.push_back(dirEntry);
			}

			++entry;
		}

		++dirSector;
	}

	return true;
}

// Analyzes the disk image directory and extract files
bool CDragonDOS_FS::ParseFiles()
{
	files.clear();

	std::vector<SDGNDosDirectoryEntry>::const_iterator entryIter;
	for( entryIter = directory.begin(); entryIter != directory.end(); ++entryIter )
	{
		if( !entryIter->bDeleted && !entryIter->bContinuation )
		{
			CDGNDosFile file;
			std::vector<unsigned char> fileData;

			ExtractFile( entryIter->fileBlock.fileName, fileData, true );

			file.SetFileName      ( entryIter->fileBlock.fileName );
			file.SetFileData      ( fileData                      );
			file.SetFileProtected ( entryIter->bProtected         );
			file.SetFileType      ( entryIter->fileType           );
			file.SetLoadAddress   ( entryIter->loadAddress        );
			file.SetExecAddress   ( entryIter->execAddress        );

			files.push_back(file);
		}
	}

	return true;
}

// IFileSystemInterface implementation
bool CDragonDOS_FS::Load(IDiskImageInterface* _disk)
{
	return SetDisk( _disk );
}

// Saves changes to the DragonDOS file system to the specified file
bool CDragonDOS_FS::Save( const std::string& _fileName )
{
	if( 0 == disk )
	{
		return false;
	}

	return disk->Save( _fileName.c_str() );
}

size_t CDragonDOS_FS::GetFilesNum() const
{
	return GetNumberOfFiles();
}

std::string CDragonDOS_FS::GetFileName(size_t _fileIdx) const
{
	if( _fileIdx < GetNumberOfFiles() )
	{
		return files[_fileIdx].GetFileName();
	}

	return "";
}

size_t CDragonDOS_FS::GetFileSize( size_t _fileIdx ) const
{
	if( _fileIdx < GetNumberOfFiles() )
	{
		return files[_fileIdx].GetFileSize();
	}

	return 0;
}

size_t CDragonDOS_FS::GetFreeSize() const
{
	if( nullptr == disk )
	{
		return 0;
	}
	
	size_t retVal = 0;
	size_t freeSectors = 0;

	for( size_t side = 0; side < disk->GetSidesNum(); ++side )
	{
		unsigned char* pSector = disk->GetSector(DRAGONDOS_DIR_TRACK, 0, (unsigned int)side);
		for( int byte=0; byte < 0xB4; ++byte )
		{
			freeSectors += count_ones( pSector[byte] );
		}
	}

	retVal = freeSectors * DRAGONDOS_SECTOR_SIZE;

	return retVal;
}

std::string CDragonDOS_FS::GetFSName() const
{
	return "DragonDOS";
}

std::string CDragonDOS_FS::GetFSVariant() const
{
	return "";
}

std::string CDragonDOS_FS::GetVolumeLabel() const
{
	return "DragonDOS Disk";
}

bool CDragonDOS_FS::InitDisk( IDiskImageInterface* _disk )
{
	if( nullptr == _disk )
	{
		return false;
	}

	if( _disk->GetTracksNum() < DRAGONDOS_DIR_TRACK || _disk->GetSectorsNum() != DRAGONDOS_SECTORSPERTRACK )
	{
		return false;
	}

	unsigned char* sector;

	// The sector bitmap is split across sectors 1 and 2 of track 20.
	// And copied on track 16.
	
	// Track 20, sector 1. Sector bitmap and disk geometry.
	sector = _disk->GetSector( DRAGONDOS_DIR_TRACK, 0, 0 );
	if( nullptr == sector )
	{
		return false;
	}

	// 0x00 - 0x59	Bitmap for LSNs 0x000 - 0x2CF (40 tracks, 720 total sectors on side 1)
	// 0x00 - 0xb3	Bitmap for LSNs 0x000 - 0x59F (80 tracks, 1440 total sectors on side 1)
	// Each bit in the sector bitmap represents a single logical sector number
	// 0 = used, 1 = free
	unsigned char numSides = (unsigned char)_disk->GetSidesNum();
	unsigned char numTracks = (unsigned char)_disk->GetTracksNum();
	unsigned char secsPerTrack = (unsigned char)(DRAGONDOS_SECTORSPERTRACK * _disk->GetSidesNum());

	memset( (void*)sector, 0xFF, DRAGONDOS_BITMAPSIZE );
	if( numSides == 1 && numTracks <= 40)
	{
		memset( (void*)(sector+DRAGONDOS_HALFBITMAPSIZE), 0x0, DRAGONDOS_HALFBITMAPSIZE );
	}
	memset( (void*)(&sector[DRAGONDOS_BITMAPSIZE]), 0x0, 256-DRAGONDOS_BITMAPSIZE );

	sector[0xFC] = numTracks;
	sector[0xFD] = secsPerTrack;
	sector[0xFE] = (~numTracks    & 0xFF);
	sector[0xFF] = (~secsPerTrack & 0xFF);

	// Mark tracks 16 and 20 as in use. Track 16 is used as a temporary storage for file system 
	// changes which are then copied over to track 20.
	unsigned short int lsn = 0;
	unsigned short int bitmapPos = 0;
	unsigned short int bytePos = 0;

	for( unsigned short int trackNum = 16; trackNum <= 20; trackNum += 4 )
	{
		for( unsigned short int sectorNum = 0; sectorNum < DRAGONDOS_SECTORSPERTRACK; ++sectorNum )
		{
			lsn = LSN( *_disk, trackNum, 0, sectorNum );

			MarkBitmapLSNUsed( _disk, lsn );
		}
	}

	// Track 20, sector 2. Extended sector bitmap for double-sided disks.
	sector = _disk->GetSector( DRAGONDOS_DIR_TRACK, 0, 1 );
	if( nullptr == sector )
	{
		return false;
	}

	memset( (void*)sector, (numSides == 1 ? 0x0 : 0xFF), DRAGONDOS_BITMAPSIZE );
	if( numSides == 2 && numTracks <= 40)
	{
		memset( (void*)sector, 0x0, DRAGONDOS_BITMAPSIZE );
	}
	memset( (void*)(&sector[DRAGONDOS_BITMAPSIZE]), 0x0, 256-DRAGONDOS_BITMAPSIZE );

	// Track 20, sectors 3-18. Directory entries.
	unsigned char emptyDirSectorValue = DRAGONDOS_FLAG_DELETED | DRAGONDOS_FLAG_ENDOFDIR | DRAGONDOS_FLAG_CONTINUATION;

	for( unsigned short int sectorNum = 2; sectorNum < DRAGONDOS_SECTORSPERTRACK; ++sectorNum )
	{
		sector = _disk->GetSector( DRAGONDOS_DIR_TRACK, 0, sectorNum );
		if( nullptr == sector )
		{
			return false;
		}

		memset( (void*)sector, 0x00, DRAGONDOS_SECTOR_SIZE );

		for ( size_t entry = 0; entry < DRAGONDOS_DIR_ENTRIES_PER_SECT; ++entry )
		{
			sector[entry * DRAGONDOS_DIR_ENTRY_SIZE] = emptyDirSectorValue;
		}
	}

	// Copy track 20 to track 16
	BackUpDirTrack( _disk );

	return true;
}

SFileInfo CDragonDOS_FS::GetFileInfo(size_t _fileIdx) const
{
	SFileInfo retVal;

	retVal.isOk = false;

	if( _fileIdx < GetNumberOfFiles() )
	{
		retVal.isOk = true;
		retVal.name = files[_fileIdx].GetFileName();
		retVal.size = files[_fileIdx].GetFileSize();
		retVal.attr = files[_fileIdx].GetFileProtected() ? FA_PROTECTED : 0;
	}

	return retVal;
}

const CDirectoryEntryWrapper& CDragonDOS_FS::GetFSRoot() const
{
	return rootDir;
}

bool CDragonDOS_FS::BackUpDirTrack( IDiskImageInterface* _disk )
{
	if( nullptr == _disk )
	{
		return false;
	}

	for( size_t sectorNum = 0; sectorNum < DRAGONDOS_SECTORSPERTRACK; ++sectorNum )
	{
		memcpy( _disk->GetSector(DRAGONDOS_TEMP_DIR_TRACK,0,(unsigned int)sectorNum), _disk->GetSector(DRAGONDOS_DIR_TRACK,0,(unsigned int)sectorNum), DRAGONDOS_SECTOR_SIZE );
	}

	return true;
}

unsigned char CDragonDOS_FS::GetMaxContiguousFreeSectorsInBitmapByte( unsigned char _bitmapByte )
{
	unsigned char byteMask = 0;
	unsigned char retVal = 0;
	unsigned char count = 0;

	for( unsigned char bit = 0; bit < 8; ++bit )
	{
		byteMask = (1 << bit);

		if( _bitmapByte & byteMask )
		{
			++count;
		}
		else
		{
			if( count > retVal )
			{
				retVal = count;
			}
			count = 0;
		}
	}

	if( count > retVal )
	{
		retVal = count;
	}

	return retVal;
}

bool CDragonDOS_FS::IsBitmapLSNFree( IDiskImageInterface* _disk, size_t _LSN )
{
	if( nullptr == _disk || _LSN >= (DRAGONDOS_SECTORSPERBITMAPSECTOR * 2) )
	{
		return false;
	}

	size_t sectorIdx = (_LSN / DRAGONDOS_SECTORSPERBITMAPSECTOR);

	if( _LSN >= DRAGONDOS_SECTORSPERBITMAPSECTOR )
	{
		_LSN -= DRAGONDOS_SECTORSPERBITMAPSECTOR;
	}

	unsigned char* sectorPtr = _disk->GetSector( DRAGONDOS_DIR_TRACK, 0, (unsigned int)sectorIdx );
	size_t sectorOffset = _LSN / 8;
	size_t bitOffset    = _LSN % 8;

	return ((sectorPtr[sectorOffset] & (1 << bitOffset)) != 0);
}

void CDragonDOS_FS::MarkBitmapLSNFree( IDiskImageInterface* _disk, size_t _LSN )
{
	if( nullptr == _disk || _LSN >= (DRAGONDOS_SECTORSPERBITMAPSECTOR * 2) )
	{
		return;
	}

	size_t sectorIdx = (_LSN / DRAGONDOS_SECTORSPERBITMAPSECTOR);

	if( _LSN >= DRAGONDOS_SECTORSPERBITMAPSECTOR )
	{
		_LSN -= DRAGONDOS_SECTORSPERBITMAPSECTOR;
	}

	unsigned char* sectorPtr = _disk->GetSector( DRAGONDOS_DIR_TRACK, 0, (unsigned int)sectorIdx );
	size_t sectorOffset = _LSN / 8;
	size_t bitOffset    = _LSN % 8;

	sectorPtr[sectorOffset] |= (1 << bitOffset);
}

void CDragonDOS_FS::MarkBitmapLSNUsed( IDiskImageInterface* _disk, size_t _LSN )
{
	if( nullptr == _disk || _LSN >= (DRAGONDOS_SECTORSPERBITMAPSECTOR * 2) )
	{
		return;
	}

	size_t sectorIdx = (_LSN / DRAGONDOS_SECTORSPERBITMAPSECTOR);

	if( _LSN >= DRAGONDOS_SECTORSPERBITMAPSECTOR )
	{
		_LSN -= DRAGONDOS_SECTORSPERBITMAPSECTOR;
	}

	unsigned char* sectorPtr = _disk->GetSector( DRAGONDOS_DIR_TRACK, 0, (unsigned int)sectorIdx );
	size_t sectorOffset = _LSN / 8;
	size_t bitOffset    = _LSN % 8;

	sectorPtr[sectorOffset] &= ~(1 << bitOffset);
}

std::string CDragonDOS_FS::GetFileTypeString( unsigned short int fileIdx ) const
{
	std::string retVal;

	if( fileIdx < files.size() )
	{
		switch( files[fileIdx].GetFileType() )
		{
		case DRAGONDOS_FILETYPE_DATA  :retVal = "DAT"; break;
		case DRAGONDOS_FILETYPE_BASIC :retVal = "BAS"; break;
		case DRAGONDOS_FILETYPE_BINARY:retVal = "BIN"; break;
		default:retVal = "???";
		}
	}

	return retVal;
}