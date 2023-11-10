////////////////////////////////////////////////////////////////////
//
// DragonDOS_FS.cpp - Implementation for CDragonDOS_FS, a helper 
//                    class that allows file operations on a disk
//                    image formatted with the DragonDOS file system
//
// For info on the DragonDOS file system go to:
//             http://dragon32.info/info/drgndos.txt
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Last update: 10/11/2023
//
////////////////////////////////////////////////////////////////////
#include <string.h> // for strcasecmp
#include <sstream>
#include "DragonDOS_FS.h"
#include "FS_Utils.h"

#ifndef _WIN32
#define _stricmp strcasecmp
#endif

// Get file data
void CDGNDosFile::GetFileData( vector<unsigned char>& dst ) const
{
    dst.clear();
    dst.insert( dst.begin(), data.begin(), data.end() );
}

// Set file data
void CDGNDosFile::SetFileData( const vector<unsigned char>& src )
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
unsigned short int CDragonDOS_FS::GetFileIdx( string _fileName )
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
unsigned short int CDragonDOS_FS::GetFileEntry( string _fileName ) const
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
bool CDragonDOS_FS::ExtractFile( string _fileName, vector<unsigned char>& dst ) const
{
    unsigned short int fileIdx = GetFileEntry( _fileName );

    if( fileIdx == DRAGONDOS_INVALID )
    {
        return false;
    }

    SDGNDosDirectoryEntry entry = directory[fileIdx];

    // Skip file types BIN and BAS' header.
    bool skipHeader = false;
    if( entry.fileType == DRAGONDOS_FILETYPE_BASIC || entry.fileType == DRAGONDOS_FILETYPE_BINARY )
    {
        skipHeader = true;
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
                    dst.insert( dst.end(), data + DRAGONDOS_FILEHEADER_SIZE, data + DRAGONDOS_SECTOR_SIZE );
                    skipHeader = false; // Only need to skip header once.
                }
                else if( !entry.bContinued && fab == fabNum - 1 && sector == sectorsNum - 1) // extract the right number of bytes from last sector
                {
                    dst.insert( dst.end(), data, data + entry.lastSectorSize );
                }
                else
                {
                    dst.insert( dst.end(), data, data + DRAGONDOS_SECTOR_SIZE );
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
bool CDragonDOS_FS::InsertFile( string _fileName )
{
    return false;
}

// Deletes a file from the DragonDOS file system
bool CDragonDOS_FS::DeleteFile( string _fileName )
{
    return false;
}

// Saves changes to the DragonDOS file system to the specified file
bool CDragonDOS_FS::Save( string _fileName )
{
    if( 0 == disk )
    {
        return false;
    }

    return disk->Save( _fileName.c_str() );
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
                // TODO: need to derive the DirectoryEntryWrapper to add more info
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

    vector<SDGNDosDirectoryEntry>::const_iterator entryIter;
    for( entryIter = directory.begin(); entryIter != directory.end(); ++entryIter )
    {
        if( !entryIter->bDeleted && !entryIter->bContinuation )
        {
            CDGNDosFile file;
            vector<unsigned char> fileData;

            ExtractFile( entryIter->fileBlock.fileName, fileData  );

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

// FSWrapper implementation
bool CDragonDOS_FS::Load(IDiskImageInterface* _disk)
{
    return SetDisk( _disk );
}

bool CDragonDOS_FS::Save(const string& _filename)
{
    return false;
}

size_t CDragonDOS_FS::GetFilesNum() const
{
    return GetNumberOfFiles();
}

string CDragonDOS_FS::GetFileName(size_t _fileIdx) const
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

uint8_t count_ones (uint8_t byte)
{
    static const uint8_t NIBBLE_LOOKUP [16] =
    {
        0, 1, 1, 2, 1, 2, 2, 3, 
        1, 2, 2, 3, 2, 3, 3, 4
    };

    return NIBBLE_LOOKUP[byte & 0x0F] + NIBBLE_LOOKUP[byte >> 4];
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
        unsigned char* pSector = disk->GetSector(DRAGONDOS_DIR_TRACK, 0, side);
        for( int byte=0; byte < 0xB4; ++byte )
        {
            freeSectors += count_ones( pSector[byte] );
        }
    }

    retVal = freeSectors * DRAGONDOS_SECTOR_SIZE;

    return retVal;
}

string CDragonDOS_FS::GetFSName() const
{
    return "DragonDOS";
}

string CDragonDOS_FS::GetFSVariant() const
{
    return "";
}

string CDragonDOS_FS::GetVolumeLabel() const
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
    unsigned char numTracks = (unsigned char)_disk->GetTracksNum();
    unsigned char secsPerTrack = (unsigned char)(DRAGONDOS_SECTORSPERTRACK * _disk->GetSidesNum());

    size_t bitmapSize = (numTracks * secsPerTrack / 8);
    if( numTracks > 40)
    {
        bitmapSize  /= _disk->GetSidesNum();
    }
    memset( (void*)sector, 0xFF, bitmapSize );
    memset( (void*)(&sector[bitmapSize]), 0x0, 256-bitmapSize );

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

            bitmapPos = lsn / 8;
            bytePos   = lsn % 8;

            sector[bitmapPos] &= ~(1 << bytePos);
        }
    }

    // Track 20, sector 2. Extended sector bitmap for double-sided disks.
    sector = _disk->GetSector( DRAGONDOS_DIR_TRACK, 0, 1 );
    if( nullptr == sector )
    {
        return false;
    }

    memset( (void*)sector, ((_disk->GetTracksNum() <= 40) ? 0x00 : 0xFF), bitmapSize );
    memset( (void*)(&sector[bitmapSize]), 0x0, 256-bitmapSize );

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
    for( size_t sectorNum = 0; sectorNum < DRAGONDOS_SECTORSPERTRACK; ++sectorNum )
    {
        memcpy( _disk->GetSector(16,0,sectorNum), _disk->GetSector(20,0,sectorNum), DRAGONDOS_SECTOR_SIZE );
    }

	return true;
}

SFileInfo CDragonDOS_FS::GetFileInfo(size_t _fileIdx)
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