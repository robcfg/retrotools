#include "AcornDFS.h"
#include <string.h>

const size_t DFS_SECTOR0_OFFSET  = 0;
const size_t DFS_SECTOR1_OFFSET  = 256;
const size_t DFS_SECTOR_SIZE     = 256;
const size_t DFS_FILENAME_LENGTH = 7;

DFSEntry::DFSEntry()
{
    directory   = '$';
    loadAddress = 0;
    execAddress = 0;
    fileSize    = 0;
    startSector = 2;
    locked      = false;
}

void DFSRead( unsigned char* _data, size_t _size, DFSDisk& _disk )
{
    // Read disk name and parameters
    char diskName[13];
    memcpy( &diskName[0], &_data[DFS_SECTOR0_OFFSET], 8 );
    memcpy( &diskName[8], &_data[DFS_SECTOR1_OFFSET], 4 );
    diskName[12] = 0;
    _disk.name = diskName;
    _disk.sequence = _data[DFS_SECTOR1_OFFSET+4];
    unsigned int entriesNum = ((unsigned int)_data[DFS_SECTOR1_OFFSET+5] / 8);
    _disk.sectorsNum = (unsigned short int)((_data[DFS_SECTOR1_OFFSET+6] & 7) << 8);
    _disk.sectorsNum |= (unsigned short int) _data[DFS_SECTOR1_OFFSET+7];
    _disk.bootOption = ((_data[DFS_SECTOR1_OFFSET+6] & 0x30) >> 4);

    // Read files' info
    char fileName[8];
    fileName[7] = 0;
    size_t sector_0_offset = 8 + DFS_SECTOR0_OFFSET;
    size_t sector_1_offset = 8 + DFS_SECTOR1_OFFSET;
    for( size_t fileNum = 0; fileNum < entriesNum; ++fileNum )
    {
        DFSEntry tmpEntry;

        // Sector 0
        memcpy( &fileName[0], &_data[sector_0_offset], DFS_FILENAME_LENGTH );
        tmpEntry.name = fileName;
        sector_0_offset += DFS_FILENAME_LENGTH;

        tmpEntry.directory = _data[sector_0_offset] & 0x7F;
        tmpEntry.locked    = (_data[sector_0_offset++] & 0x80) != 0;

        // Sector 1
        tmpEntry.loadAddress =   _data[sector_1_offset++];
        tmpEntry.loadAddress |= (_data[sector_1_offset++] << 8);
        tmpEntry.execAddress =   _data[sector_1_offset++];
        tmpEntry.execAddress |= (_data[sector_1_offset++] << 8);
        tmpEntry.fileSize    =   _data[sector_1_offset++];
        tmpEntry.fileSize    |= (_data[sector_1_offset++] << 8);
        tmpEntry.startSector = ((_data[sector_1_offset] & 3)  << 8 );
        tmpEntry.loadAddress |=((_data[sector_1_offset] & 12 )<< 14);
        tmpEntry.fileSize    |=((_data[sector_1_offset] & 48 )<< 12);
        tmpEntry.execAddress |=((_data[sector_1_offset++] & 192)<< 10);
        tmpEntry.startSector |=  _data[sector_1_offset++];

        // Data
        tmpEntry.data.resize( tmpEntry.fileSize, 0 );
        memcpy( tmpEntry.data.data(), &_data[tmpEntry.startSector * 256], tmpEntry.fileSize );

        _disk.files.push_back( tmpEntry );
    }
}

bool DFSWrite( unsigned char* _data, size_t _size, const DFSDisk& _disk )
{
    if( nullptr == _data || _size < (size_t)(_disk.sectorsNum * 256) )
    {
        return false;
    }

    memset( _data, 0, _size );

    // Write name
    std::string name0;
    std::string name1;
    
    if( _disk.name.length() <= 8 )
    {
        name0 = _disk.name;
    }
    else
    {
        name0 = _disk.name.substr( 0, 8 );
        name1 = _disk.name.substr( 8, std::string::npos );
    }

    if( name0.length() < 8 )
    {
        name0.insert( name0.end(), 8 - name0.length(), ' ' );
    }
    if( name1.length() < 4 )
    {
        name1.insert( name1.end(), 4 - name1.length(), ' ' );
    }

    memcpy( &_data[DFS_SECTOR0_OFFSET], name0.data(), 8 );
    memcpy( &_data[DFS_SECTOR1_OFFSET], name1.data(), 4 );

    _data[DFS_SECTOR1_OFFSET+4] = _disk.sequence;
    _data[DFS_SECTOR1_OFFSET+5] = (unsigned char)(_disk.files.size() * 8);
    
    _data[DFS_SECTOR1_OFFSET+6] = (((_disk.sectorsNum >> 8) & 0x07) | (_disk.bootOption << 4));
    _data[DFS_SECTOR1_OFFSET+7] = (unsigned char)(_disk.sectorsNum & 0xFF);

    // Write files' info and data
    size_t sector_0_offset = 8 + DFS_SECTOR0_OFFSET;
    size_t sector_1_offset = 8 + DFS_SECTOR1_OFFSET;

    for( auto entry : _disk.files )
    {
        // Sector 0
        std::string tmpName = entry.name;
        if( tmpName.length() > DFS_FILENAME_LENGTH )
        {
            tmpName = tmpName.substr( 0, DFS_FILENAME_LENGTH );
        }
        if( tmpName.length() < DFS_FILENAME_LENGTH )
        {
            tmpName.insert( tmpName.end(), DFS_FILENAME_LENGTH - tmpName.length(), ' ' );
        }
        
        memcpy( &_data[sector_0_offset], tmpName.c_str(), DFS_FILENAME_LENGTH );
        sector_0_offset += DFS_FILENAME_LENGTH;

        _data[sector_0_offset++] = entry.directory | (entry.locked ? 0x80 : 0x00);

        // Sector 1
        _data[sector_1_offset++]  = (unsigned char) (entry.loadAddress & 0xFF);
        _data[sector_1_offset++]  = (unsigned char)((entry.loadAddress >> 8) & 0xFF);
        _data[sector_1_offset++]  = (unsigned char) (entry.execAddress & 0xFF);
        _data[sector_1_offset++]  = (unsigned char)((entry.execAddress >> 8) & 0xFF);
        _data[sector_1_offset++]  = (unsigned char) (entry.fileSize & 0xFF);
        _data[sector_1_offset++]  = (unsigned char)((entry.fileSize >> 8) & 0xFF);
        _data[sector_1_offset  ]  = (unsigned char)((entry.startSector >> 8) & 3  );
        _data[sector_1_offset  ] |= (unsigned char)((entry.loadAddress >> 14) & 12 );
        _data[sector_1_offset  ] |= (unsigned char)((entry.fileSize    >> 12) & 48 );
        _data[sector_1_offset++] |= (unsigned char)((entry.execAddress >> 10) & 192);
        _data[sector_1_offset++]  = (unsigned char) (entry.startSector & 0xFF);

        // Data
        if( !entry.data.empty() )
        {
            memcpy( &_data[entry.startSector * 256], entry.data.data(), entry.data.size() );
        }
    }

    return true;
}

std::string BootOptionToString( unsigned char _bootOption )
{
    switch( _bootOption )
    {
    case 1: return "*Load $.!BOOT";
    case 2: return "*Run $.!BOOT";
    case 3: return "*Exec $.!BOOT";
    }

    return "*None";
}

void DFSPackFiles( DFSDisk& _disk )
{
    size_t startSector = 2;

    for( auto& file : _disk.files )
    {
        file.startSector = startSector;

        startSector += (file.fileSize / DFS_SECTOR_SIZE);
        if( 0 != (file.fileSize % DFS_SECTOR_SIZE) )
        {
            ++startSector;
        }
    }    
}
