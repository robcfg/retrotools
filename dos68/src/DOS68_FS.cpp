#include "DOS68_FS.h"

#include <algorithm>
#include <cstring>
#include <filesystem>

using namespace std;

const string DOS68_FS_Name("Smoke Signal Broadcasting DOS68 filesystem");

bool CDOS68_FS::Load(IDiskImageInterface* _disk)
{
    if( nullptr == _disk )
    {
        return false;
    }

    mDisk = _disk;

    mDirectory.clear();

    unsigned char side   = DOS68_DIR_START_SIDE;
    unsigned char track  = DOS68_DIR_START_TRACK;
    unsigned char sector = DOS68_DIR_START_SECTOR;

    bool processNextDirBlock = true;
    bool readDiskInfoFIB = true;

    while( processNextDirBlock )
    {
        size_t offset = 0;

        const unsigned char* sectorData = _disk->GetSector( track, side, sector );
        if( nullptr == sectorData )
        {
            return false;
        }

        // - Each directory block starts with 4 bytes that contain the track and sector numbers
        //   for the next and previous directory blocks.
        // - If nextDirTrack is 0 then there's no more directory blocks.
        // - There are 5 File Information Blocks (FIBs) per sector, but for the very first one
        //   which actually contains the list of available sectors and free space on the disk.
        // - Track and sector numbers need to be masked the right number of bits.
        //   Track range is 0 to 76 (0x4C)
        //   Physical sector range is 1 to 26 (0x1A) according to datasheet.
        //   Logical sector range is 0 to 17;
        unsigned char nextDirTrack  = sectorData[offset++] & DOS68_TRACK_ID_MASK;
        unsigned char nextDirSector = sectorData[offset++] & DOS68_SECTOR_ID_MASK;
        unsigned char prevDirTrack  = sectorData[offset++] & DOS68_TRACK_ID_MASK;
        unsigned char prevDirSector = sectorData[offset++] & DOS68_SECTOR_ID_MASK;
        offset += 4; // Padding?

        // Read FIBs
        char tmpChar;
        for( uint8_t fibIdx = 0; fibIdx < DOS68_FIBS_PER_SECTOR; ++fibIdx )
        {
            if( readDiskInfoFIB )
            {
                offset += 10; // Skip unused and reserved bytes

                mDiskType                 = sectorData[offset++];
                mNextAvailableBlockTrack  = sectorData[offset++] & DOS68_TRACK_ID_MASK;
                mNextAvailableBlockSector = sectorData[offset++] & DOS68_SECTOR_ID_MASK;
                mLastAvailableBlockTrack  = sectorData[offset++] & DOS68_TRACK_ID_MASK;
                mLastAvailableBlockSector = sectorData[offset++] & DOS68_SECTOR_ID_MASK;
                mAvailableSectorsNumHigh  = sectorData[offset++];
                mAvailableSectorsNumLow   = sectorData[offset++];

                offset += 7; // Skip unused bytes

                readDiskInfoFIB = false;
                continue;
            }

            SDOS68_FileInfoBlock fib;

            fib.directoryTrack  = track;
            fib.directorySector = sector;
            fib.directoryIndex  = fibIdx;

            // Read file name (zero terminated, 6 characters max.)
            for( size_t n = 0; n < DOS68_MAX_FILE_NAME_LEN; ++n )
            {
                tmpChar = (char)sectorData[offset++];
                if( 0 != tmpChar )
                {
                    fib.name += tmpChar;
                }
            }

            // Read file extension (zero terminated, 3 characters max.)
            for( size_t e = 0; e < DOS68_MAX_FILE_EXT_LEN; ++e )
            {
                tmpChar = (char)sectorData[offset++];
                if( 0 != tmpChar )
                {
                    fib.ext += tmpChar;
                }
            }

            fib.fullName = fib.name;
            fib.fullName += ".";
            fib.fullName += fib.ext;

            fib.type           = sectorData[offset++];
            fib.status         = sectorData[offset++];
            fib.firstTrack     = sectorData[offset++] & DOS68_TRACK_ID_MASK;
            fib.firstSector    = sectorData[offset++] & DOS68_SECTOR_ID_MASK;
            fib.lastTrack      = sectorData[offset++] & DOS68_TRACK_ID_MASK;
            fib.lastSector     = sectorData[offset++] & DOS68_SECTOR_ID_MASK;
            fib.sectorsNumHigh = sectorData[offset++];
            fib.sectorsNumLow  = sectorData[offset++];

            offset += 7; // Skip reserved data

            if( !fib.name.empty() )
            {
                mDirectory.push_back( fib );
            }
        }

        // Set up next directory block parameters (if applicable)
        track  = nextDirTrack;
        sector = nextDirSector;
        processNextDirBlock = (nextDirTrack != 0);
    }

    return true;
}

bool CDOS68_FS::Save(const string& _filename)
{
    if( nullptr == mDisk )
    {
        return false;
    }

    return mDisk->Save( _filename );
}

const CDirectoryEntryWrapper& CDOS68_FS::GetFSRoot() const
{
    return mDummyDirEntryWrapper;
}

size_t CDOS68_FS::GetFilesNum()
{
    return mDirectory.size();
}

string CDOS68_FS::GetFileName(size_t _fileIdx)
{
    string retVal;

    if( _fileIdx < mDirectory.size() )
    {
        retVal = mDirectory[_fileIdx].name;
        retVal += ".";
        retVal += mDirectory[_fileIdx].ext;
    }

    return retVal;
}

SFileInfo CDOS68_FS::GetFileInfo(size_t _fileIdx)
{
    SFileInfo retVal;

    if( _fileIdx < mDirectory.size() )
    {
        retVal.name = mDirectory[_fileIdx].fullName;

        retVal.size = ((mDirectory[_fileIdx].sectorsNumHigh << 8) | mDirectory[_fileIdx].sectorsNumLow) * DOS68_SECTOR_DATA_SIZE;

        retVal.isOk = true;

        retVal.attr = 0;
    }

    return retVal;
}

bool CDOS68_FS::ExtractFile( string _fileName, vector<unsigned char>& _dst )
{
    if( nullptr == mDisk )
    {
        return false;
    }

    auto predicate = [_fileName](SDOS68_FileInfoBlock& _fib){ return 0 == _fib.fullName.compare(_fileName); };
 
    auto result = std::find_if(mDirectory.begin(), mDirectory.end(), predicate);
    if( result != mDirectory.end() )
    {
        uint8_t trackId = result->firstTrack;
        uint8_t sectorId = result->firstSector;
        uint16_t sectorsNum = (result->sectorsNumHigh << 8) | result->sectorsNumLow;

        for( uint16_t sector = 0; sector < sectorsNum; ++sector )
        {
            const unsigned char* sectorData = mDisk->GetSector( trackId, 0, sectorId );

            _dst.insert( _dst.end(), sectorData + 4, sectorData + DOS68_SECTOR_SIZE );

            trackId = sectorData[0] & DOS68_TRACK_ID_MASK;
            sectorId = sectorData[1] & DOS68_SECTOR_ID_MASK;
        }

        return true;
    }

    return false;
}

bool CDOS68_FS::InsertFile( string _fileName, const vector<unsigned char>& _src,bool _binaryFile )
{
    // Process file name
    filesystem::path filePath( _fileName );
    string dos68_filename  = filePath.stem     ();
    string dos68_extension = filePath.extension();
    if( !dos68_extension.empty() )
    {
        dos68_extension.erase(0,1);
    }

    if( dos68_filename.length() > DOS68_MAX_FILE_NAME_LEN )
    {
        dos68_filename.resize( DOS68_MAX_FILE_NAME_LEN );
    }
    if( dos68_extension.length() > DOS68_MAX_FILE_EXT_LEN )
    {
        dos68_extension.resize( DOS68_MAX_FILE_EXT_LEN );
    }
    transform( dos68_filename.begin() , dos68_filename.end( ), dos68_filename.begin() , ::toupper );
    transform( dos68_extension.begin(), dos68_extension.end(), dos68_extension.begin(), ::toupper );

    // Add file entry
    size_t offset         = 0;
    unsigned char side    = DOS68_DIR_START_SIDE;
    unsigned char track   = DOS68_DIR_START_TRACK;
    unsigned char sector  = DOS68_DIR_START_SECTOR;
    unsigned char fibSlot = 0;

    bool processNextDirBlock = true;
    bool skipDiskInfoFIB = true;
    bool allocateNewDirSector = true;

    unsigned char* sectorData   = nullptr;
    unsigned char* fibData      = nullptr;
    unsigned char nextDirTrack  = 0;
    unsigned char nextDirSector = 0;
    unsigned char prevDirTrack  = 0;
    unsigned char prevDirSector = 0;

    while( processNextDirBlock )
    {
        sectorData = mDisk->GetSector( track, side, sector );
        if( nullptr == sectorData )
        {
            return false;
        }

        offset = 0;
        nextDirTrack  = sectorData[offset++] & DOS68_TRACK_ID_MASK;
        nextDirSector = sectorData[offset++] & DOS68_SECTOR_ID_MASK;
        prevDirTrack  = sectorData[offset++] & DOS68_TRACK_ID_MASK;
        prevDirSector = sectorData[offset++] & DOS68_SECTOR_ID_MASK;
        offset += 4; // Padding?

        // Check FIBs
        char tmpChar;
        for( uint8_t fibIdx = 0; fibIdx < DOS68_FIBS_PER_SECTOR; ++fibIdx )
        {
            if( skipDiskInfoFIB )
            {
                offset += DOS68_FIB_SIZE;
                skipDiskInfoFIB = false;
                continue;
            }

            if( 0 != sectorData[offset] ) // Entry in use
            {
                offset += DOS68_FIB_SIZE;
                continue;
            }

            fibSlot = fibIdx;
            allocateNewDirSector = false;
            break;
        }

        processNextDirBlock = (nextDirTrack != 0);
        if( processNextDirBlock )
        {
            track  = nextDirTrack;
            sector = nextDirSector;
        }
    }

    if( allocateNewDirSector )
    {
        // Check that we still have room for the file plus the new directory block
        if( ((GetAvailableSectorsNum() - 1) * DOS68_SECTOR_DATA_SIZE) < _src.size() )
        {
            return false;
        }

        uint8_t prevTrack  = track;
        uint8_t prevSector = sector;

        sectorData[0] = mNextAvailableBlockTrack;
        sectorData[1] = mNextAvailableBlockSector;
        track  = sectorData[0];
        sector = sectorData[1];
        fibSlot = 0;

        uint8_t* nextSectorData = mDisk->GetSector( sectorData[0], 0, sectorData[1] );
        memset( &nextSectorData[4], 0, DOS68_SECTOR_DATA_SIZE );
        mNextAvailableBlockTrack  = nextSectorData[0] & DOS68_TRACK_ID_MASK;
        mNextAvailableBlockSector = nextSectorData[1] & DOS68_SECTOR_ID_MASK;
        nextSectorData[0] = 0;
        nextSectorData[1] = 0;
        nextSectorData[2] = prevTrack;
        nextSectorData[3] = prevSector;

        SetAvailableSectorsNum( GetAvailableSectorsNum() - 1 );
    }

    SDOS68_FileInfoBlock newFib;
    fibData = mDisk->GetSector( track, 0, sector );
    offset = 8 + (fibSlot * DOS68_FIB_SIZE);
    uint16_t fileSectors = (uint16_t)(_src.size() / DOS68_SECTOR_DATA_SIZE);
    if( (_src.size() % DOS68_SECTOR_DATA_SIZE) )
    {
        ++fileSectors;
    }

    newFib.name = dos68_filename;
    newFib.ext  = dos68_extension;
    newFib.type = (_binaryFile ? DOS68_FILE_TYPE_SEQ_BINARY : DOS68_FILE_TYPE_SEQ_ASCII);
    newFib.status = DOS68_FILE_STATUS_NOT_ACTIVE;
    newFib.firstTrack = mNextAvailableBlockTrack;
    newFib.firstSector = mNextAvailableBlockSector;
    newFib.sectorsNumHigh = ((fileSectors >> 8) & 0xFF);
    newFib.sectorsNumLow  = (fileSectors  & 0xFF);

    // Look for available sectors and insert file data
    track  = mNextAvailableBlockTrack;
    sector = mNextAvailableBlockSector;

    for( size_t fileBytes = 0; fileBytes < _src.size(); fileBytes += DOS68_SECTOR_DATA_SIZE )
    {
        sectorData = mDisk->GetSector( track, 0, sector );
        if( nullptr == sectorData )
        {
            return false;
        }

        if( 0 == fileBytes )
        {
            sectorData[2] = 0;
            sectorData[3] = 0;
        }

        unsigned char sizeToCopy = DOS68_SECTOR_DATA_SIZE;
        if( _src.size() - fileBytes <= DOS68_SECTOR_DATA_SIZE )
        {
            sizeToCopy = _src.size() - fileBytes;
        
            mNextAvailableBlockTrack  = sectorData[0];
            mNextAvailableBlockSector = sectorData[1];

            memset( &sectorData[4], 0, DOS68_SECTOR_DATA_SIZE );
            sectorData[0] = 0;
            sectorData[1] = 0;
            newFib.lastTrack  = track;
            newFib.lastSector = sector;
        }
        memcpy( &sectorData[4], &_src.data()[fileBytes], sizeToCopy );

        track  = sectorData[0] & DOS68_TRACK_ID_MASK;
        sector = sectorData[1] & DOS68_SECTOR_ID_MASK;
    }

    SetAvailableSectorsNum( GetAvailableSectorsNum() - fileSectors );

    // Save FIB
    memset( &fibData[offset], 0, DOS68_MAX_FILE_NAME_LEN + DOS68_MAX_FILE_EXT_LEN );
    memcpy( &fibData[offset], newFib.name.c_str(), newFib.name.length() );
    offset += DOS68_MAX_FILE_NAME_LEN;
    memcpy( &fibData[offset], newFib.ext.c_str(), newFib.ext.length() );
    offset += DOS68_MAX_FILE_EXT_LEN;

    fibData[offset++] = newFib.type;
    fibData[offset++] = newFib.status;
    fibData[offset++] = newFib.firstTrack     | DOS68_TRACK_ID_MARK;
    fibData[offset++] = newFib.firstSector    | DOS68_SECTOR_ID_MARK;
    fibData[offset++] = newFib.lastTrack      | DOS68_TRACK_ID_MARK;
    fibData[offset++] = newFib.lastSector     | DOS68_SECTOR_ID_MARK;
    fibData[offset++] = newFib.sectorsNumHigh;
    fibData[offset++] = newFib.sectorsNumLow;

    memset( &fibData[offset], 0x55, 6 );
    offset +=6;
    fibData[offset] = 0xAA;

    UpdateDiskInformationBlock();
    
    return true;
}

bool CDOS68_FS::DeleteFile( string _fileName )
{
    if( nullptr == mDisk )
    {
        return false;
    }

    auto predicate = [_fileName](SDOS68_FileInfoBlock& _fib){ return 0 == _fib.fullName.compare(_fileName); };
 
    auto result = std::find_if(mDirectory.begin(), mDirectory.end(), predicate);
    if( result != mDirectory.end() )
    {
        uint8_t trackId      = result->firstTrack  & DOS68_TRACK_ID_MASK;
        uint8_t sectorId     = result->firstSector & DOS68_SECTOR_ID_MASK;
        uint8_t sectorParams[4];
        uint16_t sectorsNum  = (result->sectorsNumHigh << 8) | result->sectorsNumLow;
        uint16_t freeSectorCount = GetAvailableSectorsNum();

        unsigned char* sectorData = nullptr;

        for( uint16_t sector = 0; sector < sectorsNum; ++sector )
        {
            sectorData = mDisk->GetSector( trackId, 0, sectorId );

            memset( sectorData + 4, 0, DOS68_SECTOR_DATA_SIZE );

            // Update first and last track/sector numbers
            if( trackId < mNextAvailableBlockTrack || sectorId < mNextAvailableBlockSector )
            {
                mNextAvailableBlockTrack  = trackId;
                mNextAvailableBlockSector = sectorId;
            }
            if( trackId > mLastAvailableBlockTrack || sectorId > mLastAvailableBlockSector )
            {
                mLastAvailableBlockTrack  = trackId;
                mLastAvailableBlockSector = sectorId;
            }

            // Restore Previous and next sector addresses
            ComputeNextAndPreviousSectorParams( trackId, sectorId, sectorParams );

            trackId = sectorData[0] & DOS68_TRACK_ID_MASK;
            sectorId = sectorData[1] & DOS68_SECTOR_ID_MASK;
            ++freeSectorCount;

            memcpy( sectorData, sectorParams, 4 );
        }

        SetAvailableSectorsNum( freeSectorCount );

        mDirectory.erase( result );
        
        sectorData = mDisk->GetSector( result->directoryTrack, 0, result->directorySector );
        if( nullptr == sectorData )
        {
            return false;
        }
        memset( sectorData + 8 + (result->directoryIndex * DOS68_FIB_SIZE), 0, DOS68_FIB_SIZE );

        UpdateDiskInformationBlock();

        return true;
    }

    return false;
}

string CDOS68_FS::GetFSName()
{
    return DOS68_FS_Name;
}

string CDOS68_FS::GetFSVariant()
{
    string retVal;

    return retVal;
}

string CDOS68_FS::GetVolumeLabel() const
{
    string retVal;

    return retVal;
}

uint8_t CDOS68_FS::GetDiskType()
{
    return mDiskType;
}

uint8_t CDOS68_FS::GetNextAvailableTrack()
{
    return mNextAvailableBlockTrack;
}

uint8_t CDOS68_FS::GetNextAvailableSector()
{
    return mNextAvailableBlockSector;
}

uint8_t CDOS68_FS::GetLastAvailableTrack()
{
    return mLastAvailableBlockTrack;
}

uint8_t CDOS68_FS::GetLastAvailableSector()
{
    return mLastAvailableBlockSector;
}

uint16_t CDOS68_FS::GetAvailableSectorsNum()
{
    return ((mAvailableSectorsNumHigh << 8) | mAvailableSectorsNumLow);
}

SDOS68_FileInfoBlock CDOS68_FS::GetFIB( size_t _fileIdx )
{
    SDOS68_FileInfoBlock retVal;

    if( _fileIdx < mDirectory.size() )
    {
        return mDirectory[_fileIdx];
    }

    return retVal;
}

void CDOS68_FS::SetAvailableSectorsNum( uint16_t _sectorsNum )
{
    mAvailableSectorsNumLow  = (uint8_t)(_sectorsNum & 0xFF);
    mAvailableSectorsNumHigh = (uint8_t)((_sectorsNum >> 8) & 0xFF);
}

bool CDOS68_FS::UpdateDiskInformationBlock()
{
    unsigned char* sectorData = mDisk->GetSector( DOS68_DIR_START_TRACK, DOS68_DIR_START_SIDE, DOS68_DIR_START_SECTOR );
    if( nullptr == sectorData )
    {
        return false;
    }

    size_t offset = 18; // Next dir track/sector, prev dir track/sector, 4 bytes padding and 10 reserved bytes

    sectorData[offset++] = mDiskType;  
    sectorData[offset++] = mNextAvailableBlockTrack  | DOS68_TRACK_ID_MARK;
    sectorData[offset++] = mNextAvailableBlockSector | DOS68_SECTOR_ID_MARK;
    sectorData[offset++] = mLastAvailableBlockTrack  | DOS68_TRACK_ID_MARK;
    sectorData[offset++] = mLastAvailableBlockSector | DOS68_SECTOR_ID_MARK;
    sectorData[offset++] = mAvailableSectorsNumHigh;  
    sectorData[offset++] = mAvailableSectorsNumLow;   

    return true;
}

void CDOS68_FS::ComputeNextAndPreviousSectorParams( uint8_t _trackId, uint8_t _sectorId, uint8_t* _dst )
{
    // Next sector address
    _dst[0] = _trackId;
    _dst[1] = _sectorId + 1;
    if( _dst[1] >= mDisk->GetSectorsNum() )
    {
        ++_dst[0];
        _dst[1] = 0;
    }
    if( _dst[0] >= mDisk->GetTracksNum() )
    {
        _dst[0] = 0;
        _dst[1] = 0;
    }

    _dst[0] |= DOS68_TRACK_ID_MARK;
    _dst[1] |= DOS68_SECTOR_ID_MARK;

    // Previous sector address
    int tmpTrack  = (int)_trackId;
    int tmpSector = (int)_sectorId - 1;
    if( tmpSector < 0 )
    {
        --tmpTrack;
        tmpSector = mDisk->GetSectorsNum() - 1;
    }
    if( tmpTrack < 0 )
    {
        tmpTrack = 0;
        tmpSector = 0;
    }
    _dst[2] = ((uint8_t)tmpTrack ) | DOS68_TRACK_ID_MARK;
    _dst[3] = ((uint8_t)tmpSector) | DOS68_SECTOR_ID_MARK;
}
